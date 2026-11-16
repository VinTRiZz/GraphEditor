#!/usr/bin/env bash
# Tests for data/gitconfig/update-cmake-version.sh.

set -u -o pipefail

f_LOG_ok() { printf '[  OK  ] %s\n' "$*"; }
f_LOG_error() { printf '[ FAIL ] %s\n' "$*" >&2; }
f_LOG_info() { printf '[ INFO ] %s\n' "$*"; }

LOCAL_testsDir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
LOCAL_sourceDir="$(cd -- "${LOCAL_testsDir}/../data/gitconfig" && pwd -P)"
LOCAL_workDir="$(mktemp -d)"
trap 'rm -rf -- "$LOCAL_workDir"' EXIT

export GIT_CONFIG_NOSYSTEM=1
export GIT_CONFIG_GLOBAL=/dev/null

LOCAL_status=0
LOCAL_caseIndex=0
LOCAL_caseDir=""
LOCAL_repo=""
LOCAL_script=""
LOCAL_msgFile=""
LOCAL_output=""
LOCAL_rc=0

f_case_begin() {
  LOCAL_caseIndex=$(( LOCAL_caseIndex + 1 ))
  LOCAL_caseDir="${LOCAL_workDir}/case-${LOCAL_caseIndex}"
  LOCAL_repo="${LOCAL_caseDir}/repo"
  LOCAL_script="${LOCAL_caseDir}/update-cmake-version.sh"
  mkdir -p -- "$LOCAL_repo"
  cp -a -- "${LOCAL_sourceDir}/update-cmake-version.sh" "$LOCAL_script"

  git -C "$LOCAL_repo" init -q -b main
  git -C "$LOCAL_repo" config user.email test@example.com
  git -C "$LOCAL_repo" config user.name "Test User"
  git -C "$LOCAL_repo" config commit.gpgsign false
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

f_commit_initial() {
  git -C "$LOCAL_repo" add -A
  git -C "$LOCAL_repo" commit -q -m "chore: initial"
}

f_message() {
  LOCAL_msgFile="${LOCAL_caseDir}/msg.txt"
  printf '%s\n' "$1" > "$LOCAL_msgFile"
}

f_run_once() {
  local LOCAL_runDir="${1:-$LOCAL_repo}"
  (
    cd -- "$LOCAL_runDir" || exit 1
    bash -- "$LOCAL_script" "$LOCAL_msgFile"
  ) > "${LOCAL_caseDir}/output.txt" 2>&1
  LOCAL_rc=$?
  LOCAL_output="$(<"${LOCAL_caseDir}/output.txt")"
}

f_run() {
  local LOCAL_firstOutput
  f_run_once "$@"
  LOCAL_firstOutput=$LOCAL_output
  if (( LOCAL_rc != 0 )) &&
     [[ $LOCAL_output == *"Please, retry to commit - CMake version updated"* ]]; then
    f_run_once "$@"
    LOCAL_output="${LOCAL_firstOutput}"$'\n'"${LOCAL_output}"
  fi
}

f_run_subdir() {
  mkdir -p -- "${LOCAL_repo}/some/deep/dir"
  f_run "${LOCAL_repo}/some/deep/dir"
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
    f_LOG_error "  output: $LOCAL_output"
    LOCAL_status=1
  fi
}

f_check_index() {
  local LOCAL_relPath="$1"
  local LOCAL_expected="$2"
  local LOCAL_name="$3"
  local LOCAL_content
  LOCAL_content="$(git -C "$LOCAL_repo" show ":${LOCAL_relPath}" 2>/dev/null || true)"
  if [[ "$LOCAL_content" == *"$LOCAL_expected"* ]]; then
    f_LOG_ok "$LOCAL_name"
  else
    f_LOG_error "$LOCAL_name: index ${LOCAL_relPath} does not contain: $LOCAL_expected"
    f_LOG_error "  index: $LOCAL_content"
    LOCAL_status=1
  fi
}

f_check_worktree() {
  local LOCAL_relPath="$1"
  local LOCAL_expected="$2"
  local LOCAL_name="$3"
  local LOCAL_content
  LOCAL_content="$(<"${LOCAL_repo}/${LOCAL_relPath}")"
  if [[ "$LOCAL_content" == *"$LOCAL_expected"* ]]; then
    f_LOG_ok "$LOCAL_name"
  else
    f_LOG_error "$LOCAL_name: worktree ${LOCAL_relPath} does not contain: $LOCAL_expected"
    f_LOG_error "  content: $LOCAL_content"
    LOCAL_status=1
  fi
}

# ---------------------------------------------------------------- #
f_LOG_info "running update-cmake-version tests"
# ---------------------------------------------------------------- #

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.1)'
f_write_file "Components/Alpha/CMakeLists.txt" 'COMPONENTS_CONFIGURE_COMPONENT(Alpha "1.0.5")'
f_write_file "Components/Beta/CMakeLists.txt" 'COMPONENTS_CONFIGURE_COMPONENT(Beta "1.0.0")'
f_write_file "Components/Alpha/src/a.cpp" 'int a;'
f_write_file "Components/Beta/src/b.cpp" 'int b;'
f_commit_initial
f_write_file "Components/Alpha/src/a.cpp" 'int a2;'
f_stage "Components/Alpha/src/a.cpp"
f_message "fix: correct a"
f_run
f_check_status 0 "$LOCAL_rc" "fix commit succeeds"
f_check_index "Components/Alpha/CMakeLists.txt" '1.00.0006' "fix bumps the affected component patch"
f_check_index "Components/Beta/CMakeLists.txt" '1.0.0' "unaffected component is not bumped"
f_check_index "CMakeLists.txt" '1.0.1' "root is not bumped when a component owns the file"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "a.cpp" 'int a;'
f_commit_initial
f_write_file "a.cpp" 'int a2;'
f_stage "a.cpp"
f_message "fix: retry the same attempt"
f_run_once
f_check_status 1 "$LOCAL_rc" "first attempt blocks with the retry request"
f_check_output "Please, retry to commit - CMake version updated" "first attempt prints the retry request"
f_check_index "CMakeLists.txt" '1.00.0001' "first attempt stages the bump"
f_run_once
f_check_status 0 "$LOCAL_rc" "retry succeeds"
f_check_output "already staged" "retry detects the staged bump"
f_check_index "CMakeLists.txt" '1.00.0001' "retry does not bump twice"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "Components/Alpha/CMakeLists.txt" 'COMPONENTS_CONFIGURE_COMPONENT(Alpha "1.0.5")'
f_write_file "Components/Alpha/src/a.cpp" 'int a;'
f_write_file "Components/Alpha/src/b.cpp" 'int b;'
f_commit_initial
f_write_file "Components/Alpha/src/a.cpp" 'int a2;'
f_write_file "Components/Alpha/src/b.cpp" 'int b2;'
f_stage "Components/Alpha/src/a.cpp"
f_stage "Components/Alpha/src/b.cpp"
f_message "fix: correct two files"
f_run
f_check_status 0 "$LOCAL_rc" "multi-file fix succeeds"
f_check_index "Components/Alpha/CMakeLists.txt" '1.00.0007' "fix adds one patch per changed file"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "A/CMakeLists.txt" 'set(LIB_VERSION "1.0.0")
COMPONENTS_CCR_SET_LIBRARY_VERSION(A ${LIB_VERSION} true)'
f_write_file "B/CMakeLists.txt" 'COMPONENTS_CCR_SET_LIBRARY_VERSION(B "2.3.4" true)'
f_write_file "C/CMakeLists.txt" 'COMPONENTS_CONFIGURE_COMPONENT(C "4.5.6")'
f_write_file "A/src/a.cpp" 'int a;'
f_write_file "B/src/b.cpp" 'int b;'
f_write_file "C/src/c.cpp" 'int c;'
f_commit_initial
f_write_file "A/src/new1.cpp" 'int n1;'
f_write_file "A/src/new2.cpp" 'int n2;'
f_write_file "B/src/newb.cpp" 'int nb;'
f_write_file "C/src/c.cpp" 'int c2;'
f_stage "A/src/new1.cpp"
f_stage "A/src/new2.cpp"
f_stage "B/src/newb.cpp"
f_stage "C/src/c.cpp"
f_message "feat: add files"
f_run
f_check_status 0 "$LOCAL_rc" "feature commit succeeds"
f_check_index "A/CMakeLists.txt" '"1.02.0000"' "variable library version gains one minor per added file"
f_check_index "B/CMakeLists.txt" '"2.04.0004"' "literal library version gains one minor per added file"
f_check_index "C/CMakeLists.txt" '"4.06.0006"' "component without added files gains one minor per commit"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.7.23)'
f_write_file "root.txt" 'old'
f_commit_initial
f_write_file "root.txt" 'new'
f_stage "root.txt"
f_message "docs: update root"
f_run
f_check_status 0 "$LOCAL_rc" "docs commit succeeds"
f_check_index "CMakeLists.txt" '1.07.0024' "versions are re-formatted as major.00.0000 and padded"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "root.txt" 'old'
f_commit_initial
f_write_file "root.txt" 'new'
f_stage "root.txt"
f_message "perf: faster root"
f_run
f_check_status 0 "$LOCAL_rc" "perf commit succeeds"
f_check_index "CMakeLists.txt" '1.01.0000' "perf is treated as feature-like"

f_case_begin
f_write_file "CMakeLists.txt" 'project(
    Test
    VERSION 1.0.0
    LANGUAGES C CXX
)'
f_write_file "root.txt" 'old'
f_commit_initial
f_write_file "root.txt" 'new'
f_stage "root.txt"
f_message "fix: multiline project"
f_run
f_check_status 0 "$LOCAL_rc" "multiline project() is parsed"
f_check_index "CMakeLists.txt" 'VERSION 1.00.0001' "multiline project() version is updated"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "Components/Broken/CMakeLists.txt" 'COMPONENTS_CONFIGURE_COMPONENT(Broken)'
f_write_file "Components/Broken/src/a.cpp" 'int a;'
f_commit_initial
f_write_file "Components/Broken/src/a.cpp" 'int a2;'
f_stage "Components/Broken/src/a.cpp"
f_message "fix: broken declaration"
f_run
f_check_status 1 "$LOCAL_rc" "malformed component declaration blocks the commit"
f_check_output "ERROR: cannot update version" "malformed declaration reports ERROR"
f_check_index "Components/Broken/CMakeLists.txt" 'COMPONENTS_CONFIGURE_COMPONENT(Broken)' "malformed file is not changed"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test)'
f_write_file "root.txt" 'old'
f_commit_initial
f_write_file "root.txt" 'new'
f_stage "root.txt"
f_message "fix: no version"
f_run
f_check_status 1 "$LOCAL_rc" "project() without VERSION blocks the commit"
f_check_output "ERROR: cannot update version" "missing version reports ERROR"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "src/CMakeLists.txt" 'add_subdirectory(deep)'
f_write_file "src/deep/CMakeLists.txt" 'add_library(deep a.cpp)'
f_write_file "src/deep/a.cpp" 'int a;'
f_commit_initial
f_write_file "src/deep/a.cpp" 'int a2;'
f_stage "src/deep/a.cpp"
f_message "fix: nested file without its own CMakeLists"
f_run
f_check_status 0 "$LOCAL_rc" "nested file does not hang while walking to the root project"
f_check_index "CMakeLists.txt" '1.00.0001' "nested file falls back to the root project"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "root.txt" 'old'
f_commit_initial
f_write_file "root.txt" 'new'
f_stage "root.txt"
f_message "fix: deep directory"
f_run_subdir
f_check_status 0 "$LOCAL_rc" "script works from a nested directory"
f_check_index "CMakeLists.txt" '1.00.0001' "nested invocation updates the repository root project"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "root.txt" 'old'
f_commit_initial
f_write_file "root.txt" 'new'
f_stage "root.txt"
printf '// unstaged comment\n' >> "${LOCAL_repo}/CMakeLists.txt"
f_message "fix: preserve worktree"
f_run
f_check_status 0 "$LOCAL_rc" "unstaged worktree changes do not block the bump"
f_check_index "CMakeLists.txt" '1.00.0001' "index receives the bumped version"
f_check_worktree "CMakeLists.txt" 'unstaged comment' "unstaged worktree content is preserved"
f_check_output "left unstaged working-tree changes" "a warning explains the split index/worktree state"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.23.4472)'
f_write_file "root.txt" 'old'
f_commit_initial
git -C "$LOCAL_repo" checkout -q -b other
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.17.9814)'
f_stage "CMakeLists.txt"
git -C "$LOCAL_repo" commit -q -m "chore: other branch"
LOCAL_other="$(git -C "$LOCAL_repo" rev-parse HEAD)"
git -C "$LOCAL_repo" checkout -q main
printf '%s\n' "$LOCAL_other" > "${LOCAL_repo}/.git/MERGE_HEAD"
f_message "Merge branch 'other'"
f_run
f_check_status 0 "$LOCAL_rc" "merge commit succeeds"
f_check_index "CMakeLists.txt" '1.23.4473' "merge keeps the newer branch version and bumps its patch"
rm -f -- "${LOCAL_repo}/.git/MERGE_HEAD"

f_case_begin
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.0.0)'
f_write_file "root.txt" 'old'
f_commit_initial
LOCAL_ours="$(git -C "$LOCAL_repo" rev-parse HEAD)"
git -C "$LOCAL_repo" checkout -q -b other
f_write_file "CMakeLists.txt" 'project(Test VERSION 1.17.9814)'
f_stage "CMakeLists.txt"
git -C "$LOCAL_repo" commit -q -m "chore: other branch bumps version"
printf '%s\n' "$LOCAL_ours" > "${LOCAL_repo}/.git/MERGE_HEAD"
f_message "Merge branch 'main'"
f_run
f_check_status 0 "$LOCAL_rc" "merge where only the other branch changed the version succeeds"
f_check_index "CMakeLists.txt" '1.17.9815' "merge bumps the only changed branch version"
rm -f -- "${LOCAL_repo}/.git/MERGE_HEAD"

if (( LOCAL_status == 0 )); then
  f_LOG_ok "all update-cmake-version tests passed"
else
  f_LOG_error "update-cmake-version tests failed"
fi
exit "$LOCAL_status"
