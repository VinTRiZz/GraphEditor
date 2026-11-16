#!/usr/bin/env bash
# commit-msg hook helper: validate the commit message format.
# The format is defined in commit-text-format.ini next to this script.
# If the file is missing, the checker places a default one.
#
# Install from the repository root:
#   chmod +x data/gitconfig/check-commit-format.sh data/gitconfig/commit-msg
#   git config --local core.hooksPath data/gitconfig
#
# Format file: data/gitconfig/commit-text-format.ini
# Bypass:      git commit --no-verify

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
LOCAL_configFile="${GIT_HOOK_COMMIT_FORMAT:-${LOCAL_scriptBasedir}/commit-text-format.ini}"

LOCAL_defaultRegexp='^\(feat\|fix\|docs\|style\|refactor\|perf\|test\|build\|ci\|chore\|revert\)\(([a-zA-Z0-9._-]*)\)\?!\?: .\+$'
LOCAL_defaultMaxHeaderLength=72
LOCAL_defaultMaxBodyLineLength=72
LOCAL_defaultRequireBlank=true
LOCAL_defaultForbidTrailing=true
LOCAL_defaultIgnoreComments=true
LOCAL_defaultIgnoreUrls=true
LOCAL_defaultSkipMerge=true
LOCAL_defaultSkipFixup=true
LOCAL_defaultSkipRevert=true

f_write_default_config() {
  cat <<EOF
# Commit message format for check-commit-format.sh.
# Values are plain key=value pairs; the [commit] section is optional.
# message-regexp is a sed basic regular expression matched against the header.
# Escape / inside message-regexp as \/ because it is used with the / delimiter.
# Lines starting with # or ; are comments.
#
# max-header-length / max-body-line-length: 0 disables the limit.
# skip-* options allow generated commits (merge/fixup/revert) to pass.

[commit]
message-regexp=${LOCAL_defaultRegexp}
max-header-length=${LOCAL_defaultMaxHeaderLength}
max-body-line-length=${LOCAL_defaultMaxBodyLineLength}
require-blank-line-after-header=${LOCAL_defaultRequireBlank}
forbid-trailing-whitespace=${LOCAL_defaultForbidTrailing}
ignore-comment-lines=${LOCAL_defaultIgnoreComments}
ignore-url-lines=${LOCAL_defaultIgnoreUrls}
skip-merge-commits=${LOCAL_defaultSkipMerge}
skip-fixup-squash=${LOCAL_defaultSkipFixup}
skip-revert-commits=${LOCAL_defaultSkipRevert}
EOF
}

f_place_default_config() {
  local LOCAL_dir
  LOCAL_dir="$(dirname -- "$LOCAL_configFile")"
  [[ -d "$LOCAL_dir" ]] || mkdir -p -- "$LOCAL_dir" 2>/dev/null || true
  if f_write_default_config > "$LOCAL_configFile" 2>/dev/null; then
    f_LOG_warn "check-commit-format: created default config: $LOCAL_configFile"
    return 0
  fi
  f_LOG_warn "check-commit-format: cannot create config, using built-in defaults: $LOCAL_configFile"
  return 1
}

f_trim() {
  local LOCAL_text="$1"
  LOCAL_text="${LOCAL_text#"${LOCAL_text%%[![:space:]]*}"}"
  LOCAL_text="${LOCAL_text%"${LOCAL_text##*[![:space:]]}"}"
  printf '%s' "$LOCAL_text"
}

f_normalize_bool() {
  case "${1,,}" in
    1|true|yes|on) printf 'true' ;;
    0|false|no|off) printf 'false' ;;
    *) return 1 ;;
  esac
}

if (( $# > 0 )); then
  case "$1" in
    -h|--help)
      f_LOG_info "Usage: check-commit-format.sh COMMIT_MSG_FILE"
      f_LOG_info ""
      f_LOG_info "Validates the commit message header and body."
      f_LOG_info "Config: ${LOCAL_configFile}"
      f_LOG_info "Environment:"
      f_LOG_info "  GIT_HOOK_COMMIT_FORMAT  override the config file path"
      exit 0
      ;;
  esac
fi

LOCAL_messageRegexp="$LOCAL_defaultRegexp"
LOCAL_maxHeaderLength="$LOCAL_defaultMaxHeaderLength"
LOCAL_maxBodyLineLength="$LOCAL_defaultMaxBodyLineLength"
LOCAL_requireBlank="$LOCAL_defaultRequireBlank"
LOCAL_forbidTrailing="$LOCAL_defaultForbidTrailing"
LOCAL_ignoreComments="$LOCAL_defaultIgnoreComments"
LOCAL_ignoreUrls="$LOCAL_defaultIgnoreUrls"
LOCAL_skipMerge="$LOCAL_defaultSkipMerge"
LOCAL_skipFixup="$LOCAL_defaultSkipFixup"
LOCAL_skipRevert="$LOCAL_defaultSkipRevert"

LOCAL_configReady=0
if [[ -f "$LOCAL_configFile" && -r "$LOCAL_configFile" ]]; then
  LOCAL_configFile="$(realpath -- "$LOCAL_configFile")"
  LOCAL_configReady=1
elif f_place_default_config; then
  LOCAL_configFile="$(realpath -- "$LOCAL_configFile")"
  LOCAL_configReady=1
fi

declare -a LOCAL_configErrors=()

if (( LOCAL_configReady == 1 )); then
  LOCAL_lineNumber=0
  while IFS= read -r LOCAL_rawLine || [[ -n "$LOCAL_rawLine" ]]; do
    LOCAL_lineNumber=$(( LOCAL_lineNumber + 1 ))
    LOCAL_line="$(f_trim "${LOCAL_rawLine%$'\r'}")"
    [[ -z "$LOCAL_line" ]] && continue
    case "$LOCAL_line" in
      '#'*|';'*|'['*) continue ;;
    esac
    if [[ "$LOCAL_line" != *=* ]]; then
      LOCAL_configErrors+=("line ${LOCAL_lineNumber}: expected key=value")
      continue
    fi

    LOCAL_key="$(f_trim "${LOCAL_line%%=*}")"
    LOCAL_value="$(f_trim "${LOCAL_line#*=}")"
    case "$LOCAL_value" in
      \"*\") LOCAL_value="${LOCAL_value:1:${#LOCAL_value}-2}" ;;
      \'*\') LOCAL_value="${LOCAL_value:1:${#LOCAL_value}-2}" ;;
    esac

    case "$LOCAL_key" in
      message-regexp)
        if [[ -n "$LOCAL_value" ]]; then
          LOCAL_messageRegexp="$LOCAL_value"
        else
          LOCAL_configErrors+=("line ${LOCAL_lineNumber}: message-regexp is empty")
        fi
        ;;
      max-header-length|max-body-line-length)
        if [[ "$LOCAL_value" =~ ^[0-9]+$ ]]; then
          if [[ "$LOCAL_key" == "max-header-length" ]]; then
            LOCAL_maxHeaderLength="$LOCAL_value"
          else
            LOCAL_maxBodyLineLength="$LOCAL_value"
          fi
        else
          LOCAL_configErrors+=("line ${LOCAL_lineNumber}: ${LOCAL_key} must be a non-negative integer")
        fi
        ;;
      require-blank-line-after-header|forbid-trailing-whitespace|ignore-comment-lines|ignore-url-lines|skip-merge-commits|skip-fixup-squash|skip-revert-commits)
        if LOCAL_normalized="$(f_normalize_bool "$LOCAL_value")"; then
          case "$LOCAL_key" in
            require-blank-line-after-header) LOCAL_requireBlank="$LOCAL_normalized" ;;
            forbid-trailing-whitespace) LOCAL_forbidTrailing="$LOCAL_normalized" ;;
            ignore-comment-lines) LOCAL_ignoreComments="$LOCAL_normalized" ;;
            ignore-url-lines) LOCAL_ignoreUrls="$LOCAL_normalized" ;;
            skip-merge-commits) LOCAL_skipMerge="$LOCAL_normalized" ;;
            skip-fixup-squash) LOCAL_skipFixup="$LOCAL_normalized" ;;
            skip-revert-commits) LOCAL_skipRevert="$LOCAL_normalized" ;;
          esac
        else
          LOCAL_configErrors+=("line ${LOCAL_lineNumber}: ${LOCAL_key} must be true or false")
        fi
        ;;
      *)
        f_LOG_warn "check-commit-format: ignoring unknown key: ${LOCAL_key}"
        ;;
    esac
  done < "$LOCAL_configFile"
fi

if (( ${#LOCAL_configErrors[@]} > 0 )); then
  f_LOG_error ""
  f_LOG_error "✖ check-commit-format: invalid config: ${LOCAL_configFile}"
  f_LOG_error ""
  for LOCAL_configError in "${LOCAL_configErrors[@]}"; do
    f_LOG_error "  • ${LOCAL_configError}"
  done
  f_LOG_error ""
  exit 2
fi

LOCAL_messageFile=""
for LOCAL_arg in "$@"; do
  case "$LOCAL_arg" in
    -h|--help)
      exit 0
      ;;
    --*)
      f_LOG_error "check-commit-format: unknown option: $LOCAL_arg"
      exit 2
      ;;
    *)
      if [[ -n "$LOCAL_messageFile" ]]; then
        f_LOG_error "check-commit-format: unexpected extra argument: $LOCAL_arg"
        exit 2
      fi
      LOCAL_messageFile="$LOCAL_arg"
      ;;
  esac
done

if [[ -z "$LOCAL_messageFile" || ! -f "$LOCAL_messageFile" || ! -r "$LOCAL_messageFile" ]]; then
  f_LOG_error "check-commit-format: commit message file is missing or unreadable: ${LOCAL_messageFile:-<none>}"
  exit 2
fi
LOCAL_messageFile="$(realpath -- "$LOCAL_messageFile")"

if [[ -z "${LC_ALL:-}" ]]; then
  export LC_ALL="C.UTF-8"
fi

declare -a LOCAL_lines=()
if ! mapfile -t LOCAL_lines < "$LOCAL_messageFile"; then
  f_LOG_error "check-commit-format: cannot read the commit message: $LOCAL_messageFile"
  exit 2
fi

for LOCAL_index in "${!LOCAL_lines[@]}"; do
  LOCAL_lines[$LOCAL_index]="${LOCAL_lines[$LOCAL_index]%$'\r'}"
done

# The verbose diff after the scissors line is not part of the commit message.
LOCAL_limit=${#LOCAL_lines[@]}
for (( LOCAL_index = 0; LOCAL_index < LOCAL_limit; LOCAL_index++ )); do
  if [[ "${LOCAL_lines[$LOCAL_index]}" =~ ^#[[:space:]]+-+[[:space:]]+\>8 ]]; then
    LOCAL_limit=$LOCAL_index
    break
  fi
done

LOCAL_headerIndex=-1
for (( LOCAL_index = 0; LOCAL_index < LOCAL_limit; LOCAL_index++ )); do
  LOCAL_line="${LOCAL_lines[$LOCAL_index]}"
  if [[ "$LOCAL_ignoreComments" == "true" && "$LOCAL_line" =~ ^[[:space:]]*# ]]; then
    continue
  fi
  LOCAL_headerIndex=$LOCAL_index
  break
done

LOCAL_header=""
[[ "$LOCAL_headerIndex" -ge 0 ]] && LOCAL_header="${LOCAL_lines[$LOCAL_headerIndex]}"

if [[ -n "$LOCAL_header" ]]; then
  LOCAL_skip=0
  case "$LOCAL_header" in
    "Merge "*) [[ "$LOCAL_skipMerge" == "true" ]] && LOCAL_skip=1 ;;
    fixup!" "*|squash!" "*|amend!" "*) [[ "$LOCAL_skipFixup" == "true" ]] && LOCAL_skip=1 ;;
    'Revert "'*) [[ "$LOCAL_skipRevert" == "true" ]] && LOCAL_skip=1 ;;
  esac
  if (( LOCAL_skip == 1 )); then
    f_LOG_info "check-commit-format: generated commit, format check skipped."
    exit 0
  fi
fi

declare -a LOCAL_violations=()

if (( LOCAL_headerIndex < 0 )); then
  LOCAL_violations+=("the commit message is empty")
else
  LOCAL_matched="$(printf '%s\n' "$LOCAL_header" | sed -n "/${LOCAL_messageRegexp}/p" 2>/dev/null)"
  LOCAL_sedStatus=$?
  if (( LOCAL_sedStatus != 0 )); then
    f_LOG_error ""
    f_LOG_error "✖ check-commit-format: invalid message-regexp in ${LOCAL_configFile}"
    f_LOG_error "  value: ${LOCAL_messageRegexp}"
    f_LOG_error ""
    exit 2
  fi
  if [[ -z "$LOCAL_matched" ]]; then
    LOCAL_violations+=("header does not match message-regexp: ${LOCAL_messageRegexp}")
  fi

  if (( LOCAL_maxHeaderLength > 0 && ${#LOCAL_header} > LOCAL_maxHeaderLength )); then
    LOCAL_violations+=("max-header-length: header is ${#LOCAL_header} characters, maximum is ${LOCAL_maxHeaderLength}")
  fi

  if [[ "$LOCAL_forbidTrailing" == "true" && "$LOCAL_header" == *[[:space:]] ]]; then
    LOCAL_violations+=("header has trailing whitespace")
  fi

  LOCAL_bodyStart=$(( LOCAL_headerIndex + 1 ))
  if (( LOCAL_bodyStart < LOCAL_limit )); then
    if [[ "$LOCAL_requireBlank" == "true" ]]; then
      LOCAL_hasBody=0
      for (( LOCAL_index = LOCAL_bodyStart; LOCAL_index < LOCAL_limit; LOCAL_index++ )); do
        LOCAL_line="${LOCAL_lines[$LOCAL_index]}"
        [[ "$LOCAL_ignoreComments" == "true" && "$LOCAL_line" =~ ^[[:space:]]*# ]] && continue
        [[ -z "${LOCAL_line//[[:space:]]/}" ]] && continue
        LOCAL_hasBody=1
        break
      done
      if (( LOCAL_hasBody == 1 )) && [[ -n "${LOCAL_lines[$LOCAL_bodyStart]//[[:space:]]/}" ]]; then
        LOCAL_violations+=("expected a blank line after the header")
      fi
    fi

    for (( LOCAL_index = LOCAL_bodyStart; LOCAL_index < LOCAL_limit; LOCAL_index++ )); do
      LOCAL_line="${LOCAL_lines[$LOCAL_index]}"
      if [[ "$LOCAL_ignoreComments" == "true" && "$LOCAL_line" =~ ^[[:space:]]*# ]]; then
        continue
      fi
      [[ -z "$LOCAL_line" ]] && continue

      if [[ "$LOCAL_forbidTrailing" == "true" && "$LOCAL_line" == *[[:space:]] ]]; then
        LOCAL_violations+=("line $(( LOCAL_index + 1 )) has trailing whitespace")
      fi

      if (( LOCAL_maxBodyLineLength > 0 && ${#LOCAL_line} > LOCAL_maxBodyLineLength )); then
        if [[ "$LOCAL_ignoreUrls" != "true" || "$LOCAL_line" != *'://'* ]]; then
          LOCAL_violations+=("max-body-line-length: line $(( LOCAL_index + 1 )) is ${#LOCAL_line} characters, maximum is ${LOCAL_maxBodyLineLength}")
        fi
      fi
    done
  fi
fi

if (( ${#LOCAL_violations[@]} == 0 )); then
  exit 0
fi

f_LOG_error ""
f_LOG_error "----------------------------------------------------------------------"
f_LOG_error " ✖  COMMIT BLOCKED"
f_LOG_error "    Commit message does not match the required format."
f_LOG_error "----------------------------------------------------------------------"
f_LOG_error ""
f_LOG_error "  Message: ${LOCAL_messageFile}"
f_LOG_error "  Config:  ${LOCAL_configFile}"
f_LOG_error ""
for LOCAL_violation in "${LOCAL_violations[@]}"; do
  f_LOG_error "  • ${LOCAL_violation}"
done
f_LOG_error ""
f_LOG_error "  Header: ${LOCAL_header}"
f_LOG_error ""
f_LOG_error "  Fix the message (or update the config file) and commit again."
f_LOG_error "  Intentional bypass: git commit --no-verify"
f_LOG_error ""

exit 1
