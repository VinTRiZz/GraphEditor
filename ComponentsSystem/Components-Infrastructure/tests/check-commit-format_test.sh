#!/usr/bin/env bash
# Tests for data/gitconfig/check-commit-format.sh.

set -u -o pipefail

f_LOG_info() { printf '[ INFO ] %s\n' "$*"; }
f_LOG_error() { printf '[ FAIL ] %s\n' "$*" >&2; }

LOCAL_testsDir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
LOCAL_sourceDir="$(cd -- "${LOCAL_testsDir}/../data/gitconfig" && pwd -P)"
LOCAL_workDir="$(mktemp -d)"
trap 'rm -rf -- "$LOCAL_workDir"' EXIT

LOCAL_failures=0
LOCAL_caseIndex=0
LOCAL_msgIndex=0
LOCAL_caseDir=""
LOCAL_checkerDir=""
LOCAL_checker=""
LOCAL_runDir=""
LOCAL_msgFile=""
LOCAL_output=""
LOCAL_rc=0

f_case_begin() {
  LOCAL_caseIndex=$(( LOCAL_caseIndex + 1 ))
  LOCAL_caseDir="${LOCAL_workDir}/case-${LOCAL_caseIndex}"
  LOCAL_checkerDir="${LOCAL_caseDir}/hooks"
  LOCAL_runDir="${LOCAL_caseDir}/run"
  mkdir -p -- "$LOCAL_checkerDir" "$LOCAL_runDir"
  cp -a -- "${LOCAL_sourceDir}/check-commit-format.sh" "$LOCAL_checkerDir/"
  LOCAL_checker="${LOCAL_checkerDir}/check-commit-format.sh"
}

f_msg() {
  LOCAL_msgIndex=$(( LOCAL_msgIndex + 1 ))
  LOCAL_msgFile="${LOCAL_caseDir}/msg-${LOCAL_msgIndex}.txt"
  printf '%s\n' "$1" > "$LOCAL_msgFile"
}

f_config() {
  local LOCAL_path="$1"
  local LOCAL_content="$2"
  printf '%s\n' "$LOCAL_content" > "$LOCAL_path"
}

f_run() {
  local LOCAL_dir="$1"
  local LOCAL_config="${2:-}"
  shift 2
  (
    cd -- "$LOCAL_dir" || exit 1
    if [[ -n "$LOCAL_config" ]]; then
      GIT_HOOK_COMMIT_FORMAT="$LOCAL_config" bash -- "$LOCAL_checker" "$@"
    else
      bash -- "$LOCAL_checker" "$@"
    fi
  ) > "${LOCAL_caseDir}/output.txt" 2>&1
  LOCAL_rc=$?
  LOCAL_output="$(<"${LOCAL_caseDir}/output.txt")"
}

f_check_status() {
  local LOCAL_expected="$1"
  local LOCAL_actual="$2"
  local LOCAL_name="$3"
  if [[ "$LOCAL_actual" -eq "$LOCAL_expected" ]]; then
    f_LOG_info "PASS: $LOCAL_name"
  else
    f_LOG_error "FAIL: $LOCAL_name (expected $LOCAL_expected, got $LOCAL_actual)"
    f_LOG_error "  output: $LOCAL_output"
    LOCAL_failures=$(( LOCAL_failures + 1 ))
  fi
}

f_check_output() {
  local LOCAL_needle="$1"
  local LOCAL_name="$2"
  if [[ "$LOCAL_output" == *"$LOCAL_needle"* ]]; then
    f_LOG_info "PASS: $LOCAL_name"
  else
    f_LOG_error "FAIL: $LOCAL_name (missing: $LOCAL_needle)"
    f_LOG_error "  output: $LOCAL_output"
    LOCAL_failures=$(( LOCAL_failures + 1 ))
  fi
}

f_check_file() {
  local LOCAL_path="$1"
  local LOCAL_name="$2"
  if [[ -f "$LOCAL_path" ]]; then
    f_LOG_info "PASS: $LOCAL_name"
  else
    f_LOG_error "FAIL: $LOCAL_name (missing file: $LOCAL_path)"
    LOCAL_failures=$(( LOCAL_failures + 1 ))
  fi
}

f_case_begin
f_msg 'feat: add feature'
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "the default config is placed and a valid header passes"
f_check_file "${LOCAL_checkerDir}/commit-text-format.ini" "the checker places commit-text-format.ini"
f_check_output "created default config" "reports the placed config"

for LOCAL_header in \
  'fix: correct typo' \
  'fix(core): correct typo' \
  'feat(api)!: change contract' \
  'chore: update deps'; do
  f_case_begin
  f_msg "$LOCAL_header"
  f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
  f_check_status 0 "$LOCAL_rc" "valid header passes: $LOCAL_header"
done

f_case_begin
f_msg 'Add feature'
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "a header without a conventional type is rejected"
f_check_output "message-regexp" "reports the header rule"

f_case_begin
f_msg "feat: $(printf 'a%.0s' {1..70})"
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "an overlong header is rejected"
f_check_output "max-header-length" "reports the header length rule"

f_case_begin
f_msg $'feat: add feature\n\nshort body'
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "a body after a blank line passes"

f_case_begin
f_msg $'feat: add feature\nbody without blank line'
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "a missing blank line after the header is rejected"
f_check_output "blank line" "reports the blank line rule"

f_case_begin
f_msg $'feat: add feature\n\nbody with trailing space '
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "trailing whitespace is rejected"
f_check_output "trailing whitespace" "reports the whitespace rule"

f_case_begin
f_msg $'feat: add feature\n\n'"$(printf 'b%.0s' {1..73})"
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "an overlong body line is rejected"
f_check_output "max-body-line-length" "reports the body length rule"

f_case_begin
f_msg $'feat: add feature\n\nSee https://example.com/'"$(printf 'c%.0s' {1..80})"
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "long URL lines are ignored by default"

f_case_begin
f_config "${LOCAL_caseDir}/strict.ini" $'[commit]\nignore-url-lines=false'
f_msg $'feat: add feature\n\nSee https://example.com/'"$(printf 'c%.0s' {1..80})"
f_run "$LOCAL_runDir" "${LOCAL_caseDir}/strict.ini" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "long URL lines fail when the exception is disabled"

f_case_begin
f_msg $'docs: describe change\n\n# ------------------------ >8 ------------------------\nnot part of the message '"$(printf 'd%.0s' {1..100})"
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "the scissors section is ignored"

f_case_begin
f_msg $'feat: add feature\n\n# a comment line that is longer than the body limit '"$(printf 'e%.0s' {1..100})"
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "comment lines are ignored"

f_case_begin
f_msg 'Merge branch main'
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "merge commits are skipped"

f_case_begin
f_msg 'fixup! feat: add feature'
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "fixup commits are skipped"

f_case_begin
f_msg 'Revert "feat: add feature"'
f_run "$LOCAL_runDir" "" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "revert commits are skipped"

f_case_begin
f_config "${LOCAL_caseDir}/custom.ini" $'[commit]\nmessage-regexp=^[A-Z].*$\nmax-header-length=0'
f_msg 'Capitalized header'
f_run "$LOCAL_runDir" "${LOCAL_caseDir}/custom.ini" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "a custom message-regexp accepts a matching header"
f_msg 'lowercase header'
f_run "$LOCAL_runDir" "${LOCAL_caseDir}/custom.ini" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "a custom message-regexp rejects a non-matching header"

f_case_begin
f_config "${LOCAL_caseDir}/short.ini" $'[commit]\nmax-header-length="10"'
f_msg 'feat: a long subject'
f_run "$LOCAL_runDir" "${LOCAL_caseDir}/short.ini" "$LOCAL_msgFile"
f_check_status 1 "$LOCAL_rc" "a quoted numeric config value is honored"

f_case_begin
f_config "${LOCAL_caseDir}/noblank.ini" $'[commit]\nrequire-blank-line-after-header=false'
f_msg $'feat: add feature\nbody without a blank line'
f_run "$LOCAL_runDir" "${LOCAL_caseDir}/noblank.ini" "$LOCAL_msgFile"
f_check_status 0 "$LOCAL_rc" "the blank line requirement can be disabled"

f_case_begin
f_config "${LOCAL_caseDir}/invalid.ini" $'[commit]\nmax-header-length=lots'
f_msg 'feat: add feature'
f_run "$LOCAL_runDir" "${LOCAL_caseDir}/invalid.ini" "$LOCAL_msgFile"
f_check_status 2 "$LOCAL_rc" "an invalid numeric value is a config error"
f_check_output "invalid config" "reports the invalid config"

f_case_begin
f_config "${LOCAL_caseDir}/invalid-regexp.ini" $'[commit]\nmessage-regexp=['
f_msg 'feat: add feature'
f_run "$LOCAL_runDir" "${LOCAL_caseDir}/invalid-regexp.ini" "$LOCAL_msgFile"
f_check_status 2 "$LOCAL_rc" "an invalid sed regexp is a config error"
f_check_output "invalid message-regexp" "reports the invalid regexp"

f_case_begin
f_run "$LOCAL_runDir" "" "${LOCAL_caseDir}/missing.txt"
f_check_status 2 "$LOCAL_rc" "a missing message file is a usage error"

f_case_begin
f_run "$LOCAL_runDir" "" --help
f_check_status 0 "$LOCAL_rc" "help exits with zero"
f_check_output "Usage:" "help prints usage"

if (( LOCAL_failures > 0 )); then
  f_LOG_error "${LOCAL_failures} test(s) failed"
  exit 1
fi

f_LOG_info "All check-commit-format tests passed"
