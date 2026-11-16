#!/usr/bin/env bash
# Tests for the commit-msg hook bundle in data/gitconfig.

set -u -o pipefail

f_LOG_error() { printf '[FAIL] %s\n' "$*"; }
f_LOG_ok() { printf '[  OK  ] %s\n' "$*"; }
f_LOG_info() { printf '[ INFO ] %s\n' "$*"; }
f_LOG_warn() { printf '[ WARN ] %s\n' "$*"; }

LOCAL_testsDir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
LOCAL_sourceDir="$(cd -- "${LOCAL_testsDir}/../data/gitconfig" && pwd -P)"
LOCAL_workDir="$(mktemp -d)"
trap 'rm -rf -- "$LOCAL_workDir"' EXIT

# Isolate tests from the user's Git configuration.
export GIT_CONFIG_NOSYSTEM=1
export GIT_CONFIG_GLOBAL=/dev/null

LOCAL_status=0
LOCAL_caseIndex=0
LOCAL_caseDir=""
LOCAL_repo=""
LOCAL_hooks=""
LOCAL_output=""
LOCAL_rc=0

f_case_begin() {
  LOCAL_caseIndex=$(( LOCAL_caseIndex + 1 ))
  LOCAL_caseDir="${LOCAL_workDir}/case-${LOCAL_caseIndex}"
  LOCAL_repo="${LOCAL_caseDir}/repo"
  LOCAL_hooks="${LOCAL_caseDir}/hooks"
  mkdir -p -- "$LOCAL_repo" "$LOCAL_hooks"

  cp -a -- "${LOCAL_sourceDir}/check-commit-format.sh" "$LOCAL_hooks/"
  cp -a -- "${LOCAL_sourceDir}/check-debug-words.sh" "$LOCAL_hooks/"
  cp -a -- "${LOCAL_sourceDir}/check-secrets.sh" "$LOCAL_hooks/"
  cp -a -- "${LOCAL_sourceDir}/check-secrets_impl" "$LOCAL_hooks/"
  cp -a -- "${LOCAL_sourceDir}/secrets-rules" "$LOCAL_hooks/"
  cp -a -- "${LOCAL_sourceDir}/update-cmake-version.sh" "$LOCAL_hooks/"
  cp -a -- "${LOCAL_sourceDir}/commit-msg" "$LOCAL_hooks/"
  printf 'canary\n' > "${LOCAL_hooks}/forbidden-words.txt"

  git -C "$LOCAL_repo" init -q
  f_write_file "CMakeLists.txt" 'project(TestProject VERSION 1.0.0)'
  f_stage "CMakeLists.txt"
}

f_write_file() {
  local LOCAL_relPath="$1"
  local LOCAL_content="$2"
  mkdir -p -- "$(dirname -- "${LOCAL_repo}/${LOCAL_relPath}")"
  printf '%s\n' "$LOCAL_content" > "${LOCAL_repo}/${LOCAL_relPath}"
}

f_stage() {
  git -C "$LOCAL_repo" add -- "$1"
}

f_run_debug_checker_from() {
  local LOCAL_runDir="$1"
  shift
  (
    cd -- "$LOCAL_runDir" || exit 1
    bash -- "${LOCAL_hooks}/check-debug-words.sh" "$@"
  ) 2>&1
}

f_run_debug_checker() {
  f_run_debug_checker_from "$LOCAL_repo" "$@"
}

f_run_commit_msg() {
  local LOCAL_msgFile="$1"
  local LOCAL_output LOCAL_rc LOCAL_firstOutput
  LOCAL_output="$(
    cd -- "$LOCAL_repo" || exit 1
    bash -- "${LOCAL_hooks}/commit-msg" "$LOCAL_msgFile" 2>&1
  )"
  LOCAL_rc=$?
  if (( LOCAL_rc != 0 )) &&
     [[ $LOCAL_output == *"Please, retry to commit - CMake version updated"* ]]; then
    LOCAL_firstOutput=$LOCAL_output
    LOCAL_output="$(
      cd -- "$LOCAL_repo" || exit 1
      bash -- "${LOCAL_hooks}/commit-msg" "$LOCAL_msgFile" 2>&1
    )"
    LOCAL_rc=$?
    LOCAL_output="${LOCAL_firstOutput}"$'\n'"${LOCAL_output}"
  fi
  printf '%s\n' "$LOCAL_output"
  return "$LOCAL_rc"
}

f_check_status() {
  local LOCAL_expected="$1"
  local LOCAL_actual="$2"
  local LOCAL_name="$3"
  if [[ "$LOCAL_actual" == "$LOCAL_expected" ]]; then
    f_LOG_ok "$LOCAL_name"
  else
    f_LOG_error "$LOCAL_name: expected status $LOCAL_expected, got $LOCAL_actual"
    f_LOG_error "  output: $LOCAL_output"
    LOCAL_status=1
  fi
}

f_check_output() {
  local LOCAL_needle="$1"
  local LOCAL_name="$2"
  if [[ "$LOCAL_output" == *"$LOCAL_needle"* ]]; then
    f_LOG_ok "$LOCAL_name"
  else
    f_LOG_error "$LOCAL_name: output does not contain: $LOCAL_needle"
    LOCAL_status=1
  fi
}

f_LOG_info "running hook tests"

f_case_begin
f_write_file "clean.txt" "nothing suspicious"
f_stage "clean.txt"
mkdir -p -- "$LOCAL_repo/sub/dir"
LOCAL_output="$(f_run_debug_checker_from "$LOCAL_repo/sub/dir")"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "clean staged file passes from another directory"

f_case_begin
cp -a -- "${LOCAL_sourceDir}/forbidden-words.txt" "${LOCAL_hooks}/forbidden-words.txt"
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "the shipped word list parses"
f_write_file "leak.txt" "TODO"
f_stage "leak.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "the shipped word list is active"
f_check_output "TODO" "reports a shipped word"

f_case_begin
f_write_file "leak.txt" "value canary here"
f_stage "leak.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "staged forbidden word blocks the commit"
f_check_output "canary" "reports the matched word"
f_check_output "leak.txt" "reports the file"

f_case_begin
printf 'canary bird\n' > "${LOCAL_hooks}/forbidden-words.txt"
f_write_file "leak.txt" "a canary   bird here"
f_stage "leak.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "phrase matches across extra whitespace"

f_case_begin
printf 'her\n' > "${LOCAL_hooks}/forbidden-words.txt"
f_write_file "clean.txt" "there is no match"
f_stage "clean.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "whole-word matching ignores substrings"
f_write_file "leak.txt" "her"
f_stage "leak.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "whole-word matching reports the exact word"

f_case_begin
printf 'one two three four\n' > "${LOCAL_hooks}/forbidden-words.txt"
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "entries with more than 3 words are rejected"
f_check_output "at most 3 words" "explains the word-count limit"

f_case_begin
rm -f -- "${LOCAL_hooks}/forbidden-words.txt"
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
LOCAL_output="$(unset GIT_HOOK_FORBIDDEN_WORDS; f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "missing word list skips the check"
f_check_output "not found" "reports the missing word list"

f_case_begin
mv -- "$LOCAL_hooks" "${LOCAL_repo}/.githooks"
LOCAL_hooks="${LOCAL_repo}/.githooks"
printf 'realpath\n' > "${LOCAL_hooks}/forbidden-words.txt"
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
f_stage ".githooks/check-debug-words.sh"
f_stage ".githooks/forbidden-words.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "hook implementation files are skipped"
f_write_file "leak.txt" "realpath"
f_stage "leak.txt"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "the same word in a project file is reported"
f_check_output "leak.txt" "reports the project file"

f_case_begin
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
LOCAL_msgFile="${LOCAL_caseDir}/msg.txt"
printf 'feat: add canary support\n' > "$LOCAL_msgFile"
LOCAL_output="$(f_run_commit_msg "$LOCAL_msgFile")"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "commit-msg blocks a forbidden word in the message"
f_check_output "COMMIT_MSG" "reports the commit message source"

f_case_begin
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
LOCAL_msgFile="${LOCAL_caseDir}/msg.txt"
printf 'Add invalid header\n' > "$LOCAL_msgFile"
LOCAL_output="$(f_run_commit_msg "$LOCAL_msgFile")"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "commit-msg blocks an invalid message header"
f_check_output "check-commit-format" "the format checker runs from commit-msg"

if command -v python3 >/dev/null 2>&1; then
  f_case_begin
  f_write_file "clean.txt" "clean"
  f_stage "clean.txt"
  LOCAL_msgFile="${LOCAL_caseDir}/msg.txt"
  printf 'feat: add config\n\npass%s = "%s"\n' 'word' 'canary-secret-value' > "$LOCAL_msgFile"
  LOCAL_output="$(f_run_commit_msg "$LOCAL_msgFile")"
  LOCAL_rc=$?
  f_check_status 1 "$LOCAL_rc" "commit-msg blocks a synthetic secret in the message"
  f_check_output "check-secrets" "check-secrets runs from commit-msg"
else
  f_LOG_warn "skipping secret integration test: python3 not found"
fi

f_case_begin
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
LOCAL_msgFile="${LOCAL_caseDir}/msg.txt"
printf 'feat: add clean file\n' > "$LOCAL_msgFile"
LOCAL_output="$(f_run_commit_msg "$LOCAL_msgFile")"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "commit-msg passes clean input"
f_check_output "all checks passed" "reports success"

f_case_begin
f_write_file "clean.txt" "clean"
f_stage "clean.txt"
LOCAL_msgFile="${LOCAL_caseDir}/msg.txt"
printf 'docs: clean commit\n\n# ------------------------ >8 ------------------------\ndiff --git a/file b/file\n-canary\n+clean\n' > "$LOCAL_msgFile"
LOCAL_output="$(f_run_commit_msg "$LOCAL_msgFile")"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "verbose diff after the scissors line is ignored"

f_case_begin
LOCAL_altWords="${LOCAL_caseDir}/alt-words.txt"
printf 'zebra\n' > "$LOCAL_altWords"
f_write_file "canary.txt" "canary"
f_stage "canary.txt"
LOCAL_output="$(export GIT_HOOK_FORBIDDEN_WORDS="$LOCAL_altWords"; f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "the override word list replaces the default"
f_write_file "zebra.txt" "zebra"
f_stage "zebra.txt"
LOCAL_output="$(export GIT_HOOK_FORBIDDEN_WORDS="$LOCAL_altWords"; f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "the override word list is used"

f_case_begin
printf 'canary\n' > "${LOCAL_hooks}/forbidden-words.txt"
f_write_file ".gitignore" "canary"
f_write_file "nested/.gitignore" "canary"
f_stage ".gitignore"
f_stage "nested/.gitignore"
LOCAL_output="$(f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" ".gitignore files are not scanned for forbidden words"

f_case_begin
printf 'привет\n' > "${LOCAL_hooks}/forbidden-words.txt"
f_write_file "leak.txt" "ПРИВЕТ"
f_stage "leak.txt"
LOCAL_output="$(export LC_ALL=C.UTF-8; f_run_debug_checker)"
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "case-insensitive Cyrillic matching works under C.UTF-8"

f_case_begin
f_write_file "CMakeLists.txt" 'project(TestProject VERSION 1.0.0)'
f_stage "CMakeLists.txt"
f_write_file "feature.txt" "feature"
f_stage "feature.txt"
LOCAL_msgFile="${LOCAL_caseDir}/msg.txt"
printf 'feat: add feature file\n' > "$LOCAL_msgFile"
LOCAL_output="$(f_run_commit_msg "$LOCAL_msgFile")"
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "commit-msg passes a versioned CMake project"
LOCAL_output="$(git -C "$LOCAL_repo" show :CMakeLists.txt)"
f_check_output "1.02.0000" "commit-msg stages the bumped CMake version"

f_case_begin
git -C "$LOCAL_repo" config core.hooksPath "$LOCAL_hooks"
f_write_file "feature.txt" "feature"
f_stage "feature.txt"
(
  cd -- "$LOCAL_repo" || exit 1
  git commit -m "feat: add feature file"
) > "${LOCAL_caseDir}/commit-output.txt" 2>&1
LOCAL_rc=$?
LOCAL_output="$(<"${LOCAL_caseDir}/commit-output.txt")"
f_check_status 1 "$LOCAL_rc" "first real commit attempt is refused after staging the bump"
f_check_output "Please, retry to commit - CMake version updated" "first attempt prints the retry message"

(
  cd -- "$LOCAL_repo" || exit 1
  git commit -m "feat: add feature file"
) > "${LOCAL_caseDir}/commit-output.txt" 2>&1
LOCAL_rc=$?
LOCAL_output="$(<"${LOCAL_caseDir}/commit-output.txt")"
f_check_status 0 "$LOCAL_rc" "retry commit succeeds"
LOCAL_output="$(git -C "$LOCAL_repo" show HEAD:CMakeLists.txt 2>/dev/null || true)"
f_check_output "1.02.0000" "retry commit contains the staged version bump"

if (( LOCAL_status == 0 )); then
  f_LOG_ok "all hook tests passed"
else
  f_LOG_error "hook tests failed"
fi
exit "$LOCAL_status"
