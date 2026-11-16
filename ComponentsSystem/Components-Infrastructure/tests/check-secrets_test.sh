#!/usr/bin/env bash
set -euo pipefail
export PYTHONDONTWRITEBYTECODE=1

f_LOG_info() { printf '[ INFO ] %s\n' "$*"; }
f_LOG_error() { printf '[ FAIL ] %s\n' "$*" >&2; }

LOCAL_scriptBasedir="$(dirname -- "$(realpath -- "${BASH_SOURCE[0]}")")"
LOCAL_gitconfigDir="$(realpath -- "${LOCAL_scriptBasedir}/../data/gitconfig")"
LOCAL_checker="${LOCAL_gitconfigDir}/check-secrets.sh"
LOCAL_validator="${LOCAL_gitconfigDir}/check-secrets_impl/validate_rules.py"
LOCAL_tempRoot="$(mktemp -d)"
LOCAL_repo="${LOCAL_tempRoot}/repo"
LOCAL_output="${LOCAL_tempRoot}/output.txt"
LOCAL_failures=0

f_cleanup() {
  rm -rf -- "$LOCAL_tempRoot"
}
trap f_cleanup EXIT

LOCAL_fakePassword="$(printf 'hunter%s' 2)"
LOCAL_fakeUser="$(printf 'ro%s' ot)"
LOCAL_fakeGithubToken="ghp_$(printf 'a%.0s' {1..36})"
LOCAL_fakeBearer="$(printf 'abcdefghijklmnopqrstuvwxyz%s' 123456)"
LOCAL_passwordKey="$(printf 'pass%s' word)"
LOCAL_userKey="$(printf 'user%s' name)"
LOCAL_tokenKey="$(printf 'TO%s' KEN)"

f_run() {
  local LOCAL_dir="$1"
  shift
  set +e
  ( cd -- "$LOCAL_dir" && "$@" ) >"$LOCAL_output" 2>&1
  LOCAL_runStatus=$?
  set -e
}

f_assert_status() {
  local LOCAL_expected="$1"
  local LOCAL_actual="$2"
  local LOCAL_name="$3"
  if [[ "$LOCAL_actual" -eq "$LOCAL_expected" ]]; then
    f_LOG_info "PASS: $LOCAL_name"
  else
    f_LOG_error "FAIL: $LOCAL_name (expected $LOCAL_expected, got $LOCAL_actual)"
    LOCAL_failures=$((LOCAL_failures + 1))
  fi
}

f_assert_output() {
  local LOCAL_pattern="$1"
  local LOCAL_name="$2"
  if grep -q -- "$LOCAL_pattern" "$LOCAL_output"; then
    f_LOG_info "PASS: $LOCAL_name"
  else
    f_LOG_error "FAIL: $LOCAL_name (missing '$LOCAL_pattern')"
    LOCAL_failures=$((LOCAL_failures + 1))
  fi
}

mkdir -p "$LOCAL_repo"
cp -a "$LOCAL_gitconfigDir/check-secrets.sh" "$LOCAL_repo/"
cp -a "$LOCAL_gitconfigDir/check-secrets_impl" "$LOCAL_repo/"
cp -a "$LOCAL_gitconfigDir/secrets-rules" "$LOCAL_repo/"
git -C "$LOCAL_repo" init -q
git -C "$LOCAL_repo" config user.email test@example.com
git -C "$LOCAL_repo" config user.name Test

f_run "$LOCAL_repo" "$LOCAL_checker" --help
f_assert_status 0 "$LOCAL_runStatus" "help exits with 0"
f_assert_output "Usage:" "help prints usage"

f_run "$LOCAL_repo" python3 "$LOCAL_validator" --rules "$LOCAL_repo/secrets-rules"
f_assert_status 0 "$LOCAL_runStatus" "rules validate"

printf '%s = "%s"\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/initial.txt"
git -C "$LOCAL_repo" add initial.txt
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 1 "$LOCAL_runStatus" "detects password before first commit"
f_assert_output "password-assignment" "reports password rule"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/initial.txt"

printf 'clean\n' > "$LOCAL_repo/clean.txt"
git -C "$LOCAL_repo" add clean.txt
git -C "$LOCAL_repo" commit -q -m "clean"

printf 'clean\n' > "$LOCAL_repo/staged.txt"
printf '%s = "%s"\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/unstaged.txt"
git -C "$LOCAL_repo" add staged.txt
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 0 "$LOCAL_runStatus" "ignores unstaged file"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/staged.txt" "$LOCAL_repo/unstaged.txt"

printf '%s = "%s"\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/.gitignore"
mkdir -p "$LOCAL_repo/nested"
printf '%s = "%s"\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/nested/.gitignore"
git -C "$LOCAL_repo" add .gitignore nested/.gitignore
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 0 "$LOCAL_runStatus" "ignores .gitignore files"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/.gitignore" "$LOCAL_repo/nested/.gitignore"
rmdir "$LOCAL_repo/nested"

printf '%s = "${%s}"\n' "$LOCAL_passwordKey" DB_PASSWORD > "$LOCAL_repo/placeholder.txt"
git -C "$LOCAL_repo" add placeholder.txt
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 0 "$LOCAL_runStatus" "allows placeholder values"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/placeholder.txt"

printf '%s = "%s"\n' "$LOCAL_userKey" "$LOCAL_fakeUser" > "$LOCAL_repo/user.txt"
git -C "$LOCAL_repo" add user.txt
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 1 "$LOCAL_runStatus" "detects username"
f_assert_output "username-assignment" "reports username rule"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/user.txt"

printf '%s=%s\n' "$LOCAL_tokenKey" "$LOCAL_fakeGithubToken" > "$LOCAL_repo/token.txt"
git -C "$LOCAL_repo" add token.txt
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 1 "$LOCAL_runStatus" "detects GitHub token"
f_assert_output "github-token" "reports GitHub rule"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/token.txt"

printf 'Authorization: Bearer %s\n' "$LOCAL_fakeBearer" > "$LOCAL_repo/bearer.txt"
git -C "$LOCAL_repo" add bearer.txt
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 1 "$LOCAL_runStatus" "detects bearer token"
f_assert_output "bearer-token" "reports bearer rule"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/bearer.txt"

printf '%s = "%s" # check-secrets:ignore\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/ignored.txt"
git -C "$LOCAL_repo" add ignored.txt
f_run "$LOCAL_repo" "$LOCAL_checker"
f_assert_status 0 "$LOCAL_runStatus" "honors inline ignore"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/ignored.txt"

printf 'clean\n' > "$LOCAL_repo/clean-msg.txt"
git -C "$LOCAL_repo" add clean-msg.txt
printf 'Add %s\n' "$LOCAL_fakeGithubToken" > "$LOCAL_tempRoot/msg.txt"
f_run "$LOCAL_repo" "$LOCAL_checker" "$LOCAL_tempRoot/msg.txt"
f_assert_status 1 "$LOCAL_runStatus" "detects token in commit message"
f_assert_output "COMMIT_MSG" "reports commit message path"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/clean-msg.txt"

printf '%s = "%s"\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/show.txt"
git -C "$LOCAL_repo" add show.txt
f_run "$LOCAL_repo" env SECRETS_SHOW_VALUES=1 "$LOCAL_checker"
f_assert_status 1 "$LOCAL_runStatus" "SECRETS_SHOW_VALUES keeps failure"
f_assert_output "$LOCAL_fakePassword" "SECRETS_SHOW_VALUES prints value"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/show.txt"

mkdir -p "$LOCAL_repo/sub"
cp -a "$LOCAL_gitconfigDir/secrets-rules" "$LOCAL_tempRoot/custom-rules"
printf '%s = "%s"\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/custom.txt"
git -C "$LOCAL_repo" add custom.txt
f_run "$LOCAL_repo/sub" env SECRETS_RULES_DIR="$LOCAL_tempRoot/custom-rules" "$LOCAL_checker"
f_assert_status 1 "$LOCAL_runStatus" "works from another directory with SECRETS_RULES_DIR"
f_assert_output "password-assignment" "custom rules are used"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/custom.txt"

printf 'xx\n' > "$LOCAL_repo/large.txt"
git -C "$LOCAL_repo" add large.txt
f_run "$LOCAL_repo" env SECRETS_MAX_FILE_SIZE=1 "$LOCAL_checker"
f_assert_status 0 "$LOCAL_runStatus" "SECRETS_MAX_FILE_SIZE skips large file"
f_assert_output "large.txt" "skip warning names the file"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/large.txt"

printf '#!/usr/bin/env bash\nexec ./check-secrets.sh "$1"\n' > "$LOCAL_repo/.git/hooks/commit-msg"
chmod +x "$LOCAL_repo/.git/hooks/commit-msg"

printf 'clean\n' > "$LOCAL_repo/hook-clean.txt"
git -C "$LOCAL_repo" add hook-clean.txt
f_run "$LOCAL_repo" git commit -m "hook clean"
f_assert_status 0 "$LOCAL_runStatus" "commit-msg hook allows clean commit"

printf '%s = "%s"\n' "$LOCAL_passwordKey" "$LOCAL_fakePassword" > "$LOCAL_repo/hook-dirty.txt"
git -C "$LOCAL_repo" add hook-dirty.txt
f_run "$LOCAL_repo" git commit -m "hook dirty"
f_assert_status 1 "$LOCAL_runStatus" "commit-msg hook blocks staged secret"
f_assert_output "password-assignment" "hook reports password rule"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/hook-dirty.txt"

printf 'clean\n' > "$LOCAL_repo/hook-message.txt"
git -C "$LOCAL_repo" add hook-message.txt
f_run "$LOCAL_repo" git commit -m "Add $LOCAL_fakeGithubToken"
f_assert_status 1 "$LOCAL_runStatus" "commit-msg hook blocks message secret"
f_assert_output "COMMIT_MSG" "hook reports commit message"
git -C "$LOCAL_repo" reset -q
rm -f "$LOCAL_repo/hook-message.txt"

if (( LOCAL_failures > 0 )); then
  f_LOG_error "$LOCAL_failures test(s) failed"
  exit 1
fi

f_LOG_info "All check-secrets tests passed"
