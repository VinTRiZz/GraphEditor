#!/usr/bin/env bash
# CMake version updater for the commit-msg hook.
#
# Git includes index changes made by commit-msg only in the NEXT commit. This
# script therefore stages the bump and aborts the first commit attempt; the
# user retries and the already staged bump becomes part of that retry commit.
# A small state file makes the retry idempotent.
#
# Accepted version sources, in priority order:
#   project(... VERSION X.Y.Z ...)
#   set(LIB_VERSION "X.Y.Z")
#   COMPONENTS_CCR_SET_LIBRARY_VERSION(<target> "X.Y.Z" ...)
#   COMPONENTS_CCR_SET_LIBRARY_VERSION(<target> ${VAR} ...) with set(VAR "X.Y.Z")
#   COMPONENTS_CONFIGURE_COMPONENT(<name> "X.Y.Z" ...)
#
# Version format: major.minor.patch -> major.00.0000 (minor is 2 digits,
# patch is 4 digits).
#
# Bump rules:
#   fix-like (fix/docs/style/test/build/ci/chore/revert/...): patch + changed files
#   feature-like (feat/perf/refactor): minor + added files (or +1 per commit)
#   merge: the newer of the two branch versions gets patch +1

set -u -o pipefail

if ! declare -F f_LOG_info >/dev/null 2>&1; then
  f_LOG_info() { printf '%s\n' "$*"; }
fi
if ! declare -F f_LOG_warn >/dev/null 2>&1; then
  f_LOG_warn() { printf '%s\n' "$*" >&2; }
fi
if ! declare -F f_LOG_error >/dev/null 2>&1; then
  f_LOG_error() { printf '%s\n' "$*" >&2; }
fi

LOCAL_scriptPath="$(realpath -- "${BASH_SOURCE[0]}")"
LOCAL_scriptBasedir="$(dirname -- "$LOCAL_scriptPath")"
LOCAL_messageFile="${1:-}"

if [[ -z "$LOCAL_messageFile" || ! -f "$LOCAL_messageFile" || ! -r "$LOCAL_messageFile" ]]; then
  f_LOG_error "update-cmake-version: commit message file is missing or unreadable: ${LOCAL_messageFile:-<none>}"
  exit 2
fi

LOCAL_repoRoot="$(git rev-parse --show-toplevel 2>/dev/null)" || {
  f_LOG_error "update-cmake-version: not inside a Git repository"
  exit 2
}
cd -- "$LOCAL_repoRoot" || exit 2

if [[ -z "${LC_ALL:-}" ]]; then
  export LC_ALL="C.UTF-8"
fi

# ---------------------------------------------------------------- #
# Version parsing and formatting
# ---------------------------------------------------------------- #

function VERSION_PARTS() {
  IFS=. read -r LOCAL_major LOCAL_minor LOCAL_patch <<< "$1"
  LOCAL_major=${LOCAL_major//[^0-9]/}
  LOCAL_minor=${LOCAL_minor//[^0-9]/}
  LOCAL_patch=${LOCAL_patch//[^0-9]/}
  LOCAL_major=${LOCAL_major:-0}
  LOCAL_minor=${LOCAL_minor:-0}
  LOCAL_patch=${LOCAL_patch:-0}
}

function VERSION_FORMAT() {
  printf '%d.%02d.%04d' "$LOCAL_major" "$LOCAL_minor" "$LOCAL_patch"
}

function VERSION_KEY() {
  VERSION_PARTS "$1"
  printf '%05d%05d%08d' "$LOCAL_major" "$LOCAL_minor" "$LOCAL_patch"
}

# ---------------------------------------------------------------- #
# Version detection in a CMakeLists.txt content string
# ---------------------------------------------------------------- #

function VERSION_DETECT() {
  local LOCAL_content=$1
  local LOCAL_re

  LOCAL_versionKind=""
  LOCAL_versionValue=""
  LOCAL_versionPrefix=""
  LOCAL_versionSuffix=""

  LOCAL_re='(project[[:space:]]*\([^)]*[Vv][Ee][Rr][Ss][Ii][Oo][Nn][[:space:]]+"?)([0-9]+(\.[0-9]+){0,2})("?[^)]*\))'
  if [[ $LOCAL_content =~ $LOCAL_re ]]; then
    LOCAL_versionKind="project"
    LOCAL_versionValue=${BASH_REMATCH[2]}
    LOCAL_versionPrefix=${BASH_REMATCH[1]}
    LOCAL_versionSuffix=${BASH_REMATCH[4]}
    return 0
  fi

  LOCAL_re='(set[[:space:]]*\([[:space:]]*[Ll][Ii][Bb]_[Vv][Ee][Rr][Ss][Ii][Oo][Nn][[:space:]]+"?)([0-9]+(\.[0-9]+){0,2})("?)'
  if [[ $LOCAL_content =~ $LOCAL_re ]]; then
    LOCAL_versionKind="lib-version"
    LOCAL_versionValue=${BASH_REMATCH[2]}
    LOCAL_versionPrefix=${BASH_REMATCH[1]}
    LOCAL_versionSuffix=${BASH_REMATCH[4]}
    return 0
  fi

  LOCAL_re='(COMPONENTS_CCR_SET_LIBRARY_VERSION[[:space:]]*\([^)]*[[:space:]]+"?)([0-9]+(\.[0-9]+){0,2})("?[^)]*\))'
  if [[ $LOCAL_content =~ $LOCAL_re ]]; then
    LOCAL_versionKind="library"
    LOCAL_versionValue=${BASH_REMATCH[2]}
    LOCAL_versionPrefix=${BASH_REMATCH[1]}
    LOCAL_versionSuffix=${BASH_REMATCH[4]}
    return 0
  fi

  LOCAL_re='(COMPONENTS_CCR_SET_LIBRARY_VERSION[[:space:]]*\([^)]*[[:space:]]+\$\{?)([A-Za-z_][A-Za-z0-9_]*)(\}?[^)]*\))'
  if [[ $LOCAL_content =~ $LOCAL_re ]]; then
    local LOCAL_var=${BASH_REMATCH[2]}
    LOCAL_re='(set[[:space:]]*\([[:space:]]*'"$LOCAL_var"'[[:space:]]+"?)([0-9]+(\.[0-9]+){0,2})("?)'
    if [[ $LOCAL_content =~ $LOCAL_re ]]; then
      LOCAL_versionKind="library-variable"
      LOCAL_versionValue=${BASH_REMATCH[2]}
      LOCAL_versionPrefix=${BASH_REMATCH[1]}
      LOCAL_versionSuffix=${BASH_REMATCH[4]}
      return 0
    fi
  fi

  LOCAL_re='(COMPONENTS_CONFIGURE_COMPONENT[[:space:]]*\([[:space:]]*[^,()[:space:]]+[[:space:],]+"?)([0-9]+(\.[0-9]+){0,2})("?[^)]*\))'
  if [[ $LOCAL_content =~ $LOCAL_re ]]; then
    LOCAL_versionKind="component"
    LOCAL_versionValue=${BASH_REMATCH[2]}
    LOCAL_versionPrefix=${BASH_REMATCH[1]}
    LOCAL_versionSuffix=${BASH_REMATCH[4]}
    return 0
  fi

  return 1
}

function VERSION_DECLARATION_PRESENT() {
  local LOCAL_content=$1
  [[ $LOCAL_content =~ (^|[^[:alnum:]_])project[[:space:]]*\( ]] && return 0
  [[ $LOCAL_content =~ set[[:space:]]*\([[:space:]]*[Ll][Ii][Bb]_[Vv][Ee][Rr][Ss][Ii][Oo][Nn] ]] && return 0
  [[ $LOCAL_content =~ COMPONENTS_CCR_SET_LIBRARY_VERSION[[:space:]]*\( ]] && return 0
  [[ $LOCAL_content =~ COMPONENTS_CONFIGURE_COMPONENT[[:space:]]*\( ]] && return 0
  return 1
}

function VERSION_REPLACE() {
  local LOCAL_content=$1
  local LOCAL_newVersion=$2
  local LOCAL_old="${LOCAL_versionPrefix}${LOCAL_versionValue}${LOCAL_versionSuffix}"
  local LOCAL_new="${LOCAL_versionPrefix}${LOCAL_newVersion}${LOCAL_versionSuffix}"
  LOCAL_replacedContent=${LOCAL_content/"$LOCAL_old"/"$LOCAL_new"}
}

# ---------------------------------------------------------------- #
# Git content helpers
# ---------------------------------------------------------------- #

function F_LOAD_CONTENT() {
  local LOCAL_path=$1
  if git cat-file -e ":${LOCAL_path}" 2>/dev/null; then
    LOCAL_fileContent=$(git show ":${LOCAL_path}"; printf 'x')
    LOCAL_fileContent=${LOCAL_fileContent%x}
    return 0
  fi
  [[ -f $LOCAL_path ]] || return 1
  LOCAL_fileContent=$(<"$LOCAL_path"; printf 'x')
  LOCAL_fileContent=${LOCAL_fileContent%x}
  return 0
}

# Resolves the nearest ancestor CMakeLists.txt that declares a version.
# Sets LOCAL_resolveTarget / LOCAL_resolveError and returns: 0 found, 2 malformed, 1 none.
function F_RESOLVE_TARGET() {
  local LOCAL_rel=$1
  local LOCAL_dir

  LOCAL_resolveTarget=""
  LOCAL_resolveError=""

  if [[ $LOCAL_rel == */* ]]; then
    LOCAL_dir=${LOCAL_rel%/*}
  else
    LOCAL_dir=.
  fi

  while :; do
    local LOCAL_cand
    if [[ $LOCAL_dir == . ]]; then
      LOCAL_cand="CMakeLists.txt"
    else
      LOCAL_cand="${LOCAL_dir}/CMakeLists.txt"
    fi

    if git cat-file -e ":${LOCAL_cand}" 2>/dev/null || [[ -f $LOCAL_cand ]]; then
      if F_LOAD_CONTENT "$LOCAL_cand"; then
        if VERSION_DETECT "$LOCAL_fileContent"; then
          LOCAL_resolveTarget=$LOCAL_cand
          return 0
        fi
        if VERSION_DECLARATION_PRESENT "$LOCAL_fileContent"; then
          LOCAL_resolveError="invalid version declaration in ${LOCAL_cand}"
          return 2
        fi
      fi
    fi

    [[ $LOCAL_dir == . ]] && break
    if [[ $LOCAL_dir == */* ]]; then
      LOCAL_dir=${LOCAL_dir%/*}
    else
      LOCAL_dir=.
    fi
  done

  LOCAL_resolveError="no CMakeLists.txt with a supported version declaration for ${LOCAL_rel}"
  return 1
}

function F_WRITE_TARGET() {
  local LOCAL_target=$1
  local LOCAL_content=$2
  local LOCAL_entry LOCAL_mode LOCAL_baseHash LOCAL_workHash LOCAL_newHash

  LOCAL_entry=$(git ls-files -s -- "$LOCAL_target" 2>/dev/null || true)

  if [[ -n $LOCAL_entry ]]; then
    LOCAL_mode=${LOCAL_entry%% *}
    LOCAL_entry=${LOCAL_entry#* }
    LOCAL_baseHash=${LOCAL_entry%% *}
    if [[ -f $LOCAL_target ]]; then
      LOCAL_workHash=$(git hash-object -- "$LOCAL_target" 2>/dev/null || true)
    else
      LOCAL_workHash=""
    fi

    LOCAL_newHash=$(printf '%s' "$LOCAL_content" | git hash-object -w --stdin) || return 1
    git update-index --cacheinfo "$LOCAL_mode,$LOCAL_newHash,$LOCAL_target" || return 1
  else
    LOCAL_newHash=$(printf '%s' "$LOCAL_content" | git hash-object -w --stdin) || return 1
    git update-index --add --cacheinfo "100644,$LOCAL_newHash,$LOCAL_target" || return 1
    LOCAL_workHash=""
    LOCAL_baseHash=""
  fi

  if [[ -f $LOCAL_target ]] && { [[ -z $LOCAL_baseHash ]] || [[ $LOCAL_workHash == "$LOCAL_baseHash" ]]; }; then
    printf '%s' "$LOCAL_content" > "$LOCAL_target" || return 1
  elif [[ -f $LOCAL_target ]]; then
    f_LOG_warn "update-cmake-version: left unstaged working-tree changes in ${LOCAL_target}; index updated"
  fi

  return 0
}

# ---------------------------------------------------------------- #
# Commit classification
# ---------------------------------------------------------------- #

LOCAL_isMerge=0
if git rev-parse -q --verify MERGE_HEAD >/dev/null 2>&1; then
  LOCAL_isMerge=1
fi

LOCAL_header=""
while IFS= read -r LOCAL_line || [[ -n $LOCAL_line ]]; do
  LOCAL_line=${LOCAL_line%$'\r'}
  [[ $LOCAL_line =~ ^[[:space:]]*# ]] && continue
  [[ -z ${LOCAL_line//[[:space:]]/} ]] && continue
  LOCAL_header=$LOCAL_line
  break
done < "$LOCAL_messageFile"

LOCAL_bump="fix"
if (( LOCAL_isMerge )); then
  LOCAL_bump="merge"
elif [[ $LOCAL_header =~ ^([A-Za-z]+)(\([^\)]*\))?!?: ]]; then
  case "${BASH_REMATCH[1],,}" in
    feat|feature|perf|refactor) LOCAL_bump="feature" ;;
  esac
fi

LOCAL_headHash="$(git rev-parse --verify -q HEAD 2>/dev/null || true)"
LOCAL_headHash=${LOCAL_headHash:--}
LOCAL_statePath="$(git rev-parse --git-path update-cmake-version.state 2>/dev/null || true)"

function STATE_HASH() {
  git ls-files -s -z | git hash-object --stdin 2>/dev/null
}

function STATE_MATCHES() {
  [[ -n $LOCAL_statePath && -f $LOCAL_statePath ]] || return 1
  local LOCAL_sHead LOCAL_sMode LOCAL_sHash
  IFS=$'\t' read -r LOCAL_sHead LOCAL_sMode LOCAL_sHash < "$LOCAL_statePath" || return 1
  [[ $LOCAL_sHead == "$LOCAL_headHash" && $LOCAL_sMode == "$LOCAL_bump" && $LOCAL_sHash == "$1" ]]
}

function STATE_WRITE() {
  [[ -n $LOCAL_statePath ]] || return 0
  printf '%s\t%s\t%s\n' "$LOCAL_headHash" "$LOCAL_bump" "$1" > "$LOCAL_statePath"
}

LOCAL_stateHash=$(STATE_HASH)
if STATE_MATCHES "$LOCAL_stateHash"; then
  f_LOG_info "update-cmake-version: version update is already staged for this commit."
  exit 0
fi

declare -A LOCAL_targetChanged=()
declare -A LOCAL_targetAdded=()
declare -A LOCAL_newContent=()
declare -A LOCAL_targetCache=()
declare -A LOCAL_targetCacheError=()
declare -A LOCAL_addedSet=()
declare -a LOCAL_errors=()

# ---------------------------------------------------------------- #
# Collect targets
# ---------------------------------------------------------------- #

if (( LOCAL_isMerge )); then
  declare -A LOCAL_mergeCandidates=()

  while IFS= read -r -d '' LOCAL_path; do
    [[ -n $LOCAL_path ]] && LOCAL_mergeCandidates[$LOCAL_path]=1
  done < <(git diff --cached --name-only -z --diff-filter=ACMR)

  while IFS= read -r -d '' LOCAL_path; do
    [[ -n $LOCAL_path ]] && LOCAL_mergeCandidates[$LOCAL_path]=1
  done < <(git diff --name-only -z --diff-filter=ACMRD HEAD MERGE_HEAD -- '*CMakeLists.txt')

  for LOCAL_path in "${!LOCAL_mergeCandidates[@]}"; do
    [[ ${LOCAL_path##*/} == "CMakeLists.txt" ]] || continue
    LOCAL_targetChanged[$LOCAL_path]=1
  done
else
  while IFS= read -r -d '' LOCAL_path; do
    [[ -n $LOCAL_path ]] && LOCAL_addedSet[$LOCAL_path]=1
  done < <(git diff --cached --name-only -z --diff-filter=A)

  while IFS= read -r -d '' LOCAL_path; do
    [[ -z $LOCAL_path ]] && continue
    if [[ ${LOCAL_path##*/} == "CMakeLists.txt" ]] &&
       ! git cat-file -e ":${LOCAL_path}" 2>/dev/null; then
      continue
    fi

    LOCAL_target=""
    if [[ ${LOCAL_targetCache[$LOCAL_path]+set} ]]; then
      LOCAL_target=${LOCAL_targetCache[$LOCAL_path]}
    elif [[ ${LOCAL_targetCacheError[$LOCAL_path]+set} ]]; then
      LOCAL_errors+=("${LOCAL_targetCacheError[$LOCAL_path]}")
      continue
    elif F_RESOLVE_TARGET "$LOCAL_path"; then
      LOCAL_target=$LOCAL_resolveTarget
      LOCAL_targetCache[$LOCAL_path]=$LOCAL_target
    else
      LOCAL_targetCacheError[$LOCAL_path]=${LOCAL_resolveError:-"cannot resolve CMake project for ${LOCAL_path}"}
      LOCAL_errors+=("${LOCAL_targetCacheError[$LOCAL_path]}")
      continue
    fi

    LOCAL_targetChanged[$LOCAL_target]=$(( ${LOCAL_targetChanged[$LOCAL_target]:-0} + 1 ))
    if [[ ${LOCAL_addedSet[$LOCAL_path]:-0} == 1 ]]; then
      LOCAL_targetAdded[$LOCAL_target]=$(( ${LOCAL_targetAdded[$LOCAL_target]:-0} + 1 ))
    fi
  done < <(git diff --cached --name-only -z --diff-filter=ACMRD)
fi

if (( ${#LOCAL_errors[@]} > 0 )); then
  for LOCAL_error in "${LOCAL_errors[@]}"; do
    f_LOG_error "ERROR: cannot update version: ${LOCAL_error}"
  done
  exit 1
fi

# ---------------------------------------------------------------- #
# Compute new versions
# ---------------------------------------------------------------- #

if (( LOCAL_isMerge )); then
  for LOCAL_target in "${!LOCAL_targetChanged[@]}"; do
    LOCAL_oursVersion=""
    LOCAL_theirsVersion=""
    LOCAL_malformed=""

    if LOCAL_branchContent=$(git show "HEAD:${LOCAL_target}" 2>/dev/null); then
      if VERSION_DETECT "$LOCAL_branchContent"; then
        LOCAL_oursVersion=$LOCAL_versionValue
      elif VERSION_DECLARATION_PRESENT "$LOCAL_branchContent"; then
        LOCAL_malformed="HEAD"
      fi
    fi

    if LOCAL_branchContent=$(git show "MERGE_HEAD:${LOCAL_target}" 2>/dev/null); then
      if VERSION_DETECT "$LOCAL_branchContent"; then
        LOCAL_theirsVersion=$LOCAL_versionValue
      elif VERSION_DECLARATION_PRESENT "$LOCAL_branchContent"; then
        LOCAL_malformed="${LOCAL_malformed:+${LOCAL_malformed}, }MERGE_HEAD"
      fi
    fi

    if [[ -n $LOCAL_malformed ]]; then
      LOCAL_errors+=("cannot update version: invalid version declaration in ${LOCAL_malformed}:${LOCAL_target}")
      continue
    fi

    if [[ -z $LOCAL_oursVersion && -z $LOCAL_theirsVersion ]]; then
      continue
    fi

    if ! F_LOAD_CONTENT "$LOCAL_target"; then
      f_LOG_warn "update-cmake-version: ${LOCAL_target} is absent from the merge result; skipped"
      continue
    fi
    if ! VERSION_DETECT "$LOCAL_fileContent"; then
      LOCAL_errors+=("cannot update version: merged file has no supported version declaration: ${LOCAL_target}")
      continue
    fi

    LOCAL_chosen=$LOCAL_versionValue
    if [[ -n $LOCAL_oursVersion ]] &&
       [[ $(VERSION_KEY "$LOCAL_oursVersion") > $(VERSION_KEY "$LOCAL_chosen") ]]; then
      LOCAL_chosen=$LOCAL_oursVersion
    fi
    if [[ -n $LOCAL_theirsVersion ]] &&
       [[ $(VERSION_KEY "$LOCAL_theirsVersion") > $(VERSION_KEY "$LOCAL_chosen") ]]; then
      LOCAL_chosen=$LOCAL_theirsVersion
    fi

    VERSION_PARTS "$LOCAL_chosen"
    LOCAL_patch=$(( LOCAL_patch + 1 ))
    LOCAL_newVersion=$(VERSION_FORMAT)

    VERSION_DETECT "$LOCAL_fileContent"
    VERSION_REPLACE "$LOCAL_fileContent" "$LOCAL_newVersion"
    LOCAL_newContent[$LOCAL_target]=$LOCAL_replacedContent
    f_LOG_info "update-cmake-version: ${LOCAL_target}: ${LOCAL_chosen} -> ${LOCAL_newVersion} (merge)"
  done
else
  for LOCAL_target in "${!LOCAL_targetChanged[@]}"; do
    if ! F_LOAD_CONTENT "$LOCAL_target"; then
      LOCAL_errors+=("cannot read ${LOCAL_target}")
      continue
    fi
    if ! VERSION_DETECT "$LOCAL_fileContent"; then
      LOCAL_errors+=("cannot update version: no supported version declaration in ${LOCAL_target}")
      continue
    fi

    LOCAL_oldVersion=$LOCAL_versionValue
    VERSION_PARTS "$LOCAL_oldVersion"

    if [[ $LOCAL_bump == "fix" ]]; then
      LOCAL_patch=$(( LOCAL_patch + ${LOCAL_targetChanged[$LOCAL_target]:-0} ))
    else
      LOCAL_added=${LOCAL_targetAdded[$LOCAL_target]:-0}
      (( LOCAL_added > 0 )) || LOCAL_added=1
      LOCAL_minor=$(( LOCAL_minor + LOCAL_added ))
    fi

    LOCAL_newVersion=$(VERSION_FORMAT)
    VERSION_REPLACE "$LOCAL_fileContent" "$LOCAL_newVersion"
    LOCAL_newContent[$LOCAL_target]=$LOCAL_replacedContent
    f_LOG_info "update-cmake-version: ${LOCAL_target}: ${LOCAL_oldVersion} -> ${LOCAL_newVersion}"
  done
fi

if (( ${#LOCAL_errors[@]} > 0 )); then
  for LOCAL_error in "${LOCAL_errors[@]}"; do
    f_LOG_error "ERROR: cannot update version: ${LOCAL_error}"
  done
  exit 1
fi

if (( ${#LOCAL_newContent[@]} == 0 )); then
  f_LOG_info "update-cmake-version: no CMake versions to update."
  exit 0
fi

for LOCAL_target in "${!LOCAL_newContent[@]}"; do
  if ! F_WRITE_TARGET "$LOCAL_target" "${LOCAL_newContent[$LOCAL_target]}"; then
    f_LOG_error "ERROR: cannot update version: failed to stage ${LOCAL_target}"
    exit 1
  fi
done

LOCAL_newStateHash=$(STATE_HASH)
if ! STATE_WRITE "$LOCAL_newStateHash"; then
  f_LOG_warn "update-cmake-version: cannot write state file: ${LOCAL_statePath:-<unknown>}"
fi

f_LOG_info "update-cmake-version: staged ${#LOCAL_newContent[@]} CMakeLists.txt version update(s)."
f_LOG_error "Please, retry to commit - CMake version updated"
exit 1
