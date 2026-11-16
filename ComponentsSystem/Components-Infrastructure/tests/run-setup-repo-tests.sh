#!/usr/bin/env bash
# Tests for the repository setup subscript in main_impl/setup-repo.sh.

set -u -o pipefail

f_LOG_error() { printf '[FAIL] %s\n' "$*" >&2; }
f_LOG_ok() { printf '[  OK  ] %s\n' "$*"; }
f_LOG_info() { printf '[ INFO ] %s\n' "$*"; }

LOCAL_testsDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
LOCAL_sourceRoot=$(cd -- "$LOCAL_testsDir/.." && pwd)
LOCAL_workDir=$(mktemp -d)
trap 'rm -rf -- "$LOCAL_workDir"' EXIT

# Isolate tests from the user's Git configuration and parent repositories.
export GIT_CONFIG_NOSYSTEM=1
export GIT_CONFIG_GLOBAL=/dev/null
export GIT_CEILING_DIRECTORIES="$LOCAL_workDir"

LOCAL_status=0
LOCAL_output=""
LOCAL_rc=0
LOCAL_target=""

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

f_check_contains() {
    local LOCAL_needle="$1"
    local LOCAL_name="${2:-output}"
    if [[ "$LOCAL_output" == *"$LOCAL_needle"* ]]; then
        f_LOG_ok "$LOCAL_name"
    else
        f_LOG_error "$LOCAL_name: output does not contain: $LOCAL_needle"
        f_LOG_error "  output: $LOCAL_output"
        LOCAL_status=1
    fi
}

f_check_file() {
    local LOCAL_path="$1"
    local LOCAL_name="$2"
    if [[ -f "$LOCAL_path" ]]; then
        f_LOG_ok "$LOCAL_name"
    else
        f_LOG_error "$LOCAL_name: missing file: $LOCAL_path"
        LOCAL_status=1
    fi
}

f_check_executable() {
    local LOCAL_path="$1"
    local LOCAL_name="$2"
    if [[ -x "$LOCAL_path" ]]; then
        f_LOG_ok "$LOCAL_name"
    else
        f_LOG_error "$LOCAL_name: not executable: $LOCAL_path"
        LOCAL_status=1
    fi
}

f_make_repo() {
    LOCAL_target="$LOCAL_workDir/$1"
    mkdir -p -- "$LOCAL_target"
    git -C "$LOCAL_target" init -q
}

f_run_setup() {
    ( cd -- / && MAIN_PROJECT_ROOTDIR="$1" bash -- "$LOCAL_setup" "${@:2}" ) 2>&1
}

LOCAL_infra="$LOCAL_workDir/infra"
mkdir -p -- "$LOCAL_infra/main_impl" "$LOCAL_infra/data"
cp -a -- "$LOCAL_sourceRoot/main_impl/setup-repo.sh" "$LOCAL_infra/main_impl/"
cp -a -- "$LOCAL_sourceRoot/data/setup-repo" "$LOCAL_infra/data/"
cp -a -- "$LOCAL_sourceRoot/data/gitconfig" "$LOCAL_infra/data/"
cat > "$LOCAL_infra/main.sh" <<'EOF'
#!/bin/bash
printf 'ROOT=%s\n' "$MAIN_PROJECT_ROOTDIR"
printf 'ARGS=%s\n' "$*"
exit 0
EOF
chmod +x -- "$LOCAL_infra/main.sh"

LOCAL_setup="$LOCAL_infra/main_impl/setup-repo.sh"
LOCAL_template="$LOCAL_infra/data/gitconfig/.gitignore"

f_LOG_info "running setup-repo tests"

f_make_repo fresh
LOCAL_output=$(f_run_setup "$LOCAL_target")
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "fresh repository setup succeeds"

for LOCAL_relPath in \
    ".githooks/commit-msg" \
    ".githooks/check-debug-words.sh" \
    ".githooks/check-secrets.sh" \
    ".githooks/update-cmake-version.sh" \
    ".githooks/forbidden-words.txt" \
    ".githooks/check-secrets_impl/scan_secrets.py" \
    ".githooks/secrets-rules/tokens.rules"; do
    f_check_file "$LOCAL_target/$LOCAL_relPath" "installed $LOCAL_relPath"
done

for LOCAL_relPath in \
    ".githooks/commit-msg" \
    ".githooks/check-debug-words.sh" \
    ".githooks/check-secrets.sh" \
    ".githooks/update-cmake-version.sh"; do
    f_check_executable "$LOCAL_target/$LOCAL_relPath" "executable $LOCAL_relPath"
done

LOCAL_hooksPath=$(git -C "$LOCAL_target" config --local --get core.hooksPath)
if [[ "$LOCAL_hooksPath" == ".githooks" ]]; then
    f_LOG_ok "core.hooksPath is .githooks"
else
    f_LOG_error "core.hooksPath is not .githooks: ${LOCAL_hooksPath:-<unset>}"
    LOCAL_status=1
fi

if cmp -s -- "$LOCAL_template" "$LOCAL_target/.gitignore"; then
    f_LOG_ok "fresh .gitignore matches the template"
else
    f_LOG_error "fresh .gitignore does not match the template"
    LOCAL_status=1
fi

f_check_executable "$LOCAL_target/main.sh" "main.sh launcher is executable"
LOCAL_output=$( ( cd -- "$LOCAL_target" && ./main.sh alpha "beta gamma" ) 2>&1 )
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "main.sh launcher runs the infrastructure main"
f_check_contains "ROOT=$(realpath -- "$LOCAL_target")" "launcher passes the repository root"
f_check_contains "ARGS=alpha beta gamma" "launcher forwards arguments"

LOCAL_ignoreBefore=$(<"$LOCAL_target/.gitignore")
LOCAL_output=$(f_run_setup "$LOCAL_target")
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "repeat setup succeeds"
LOCAL_ignoreAfter=$(<"$LOCAL_target/.gitignore")
if [[ "$LOCAL_ignoreBefore" == "$LOCAL_ignoreAfter" ]]; then
    f_LOG_ok "repeat setup does not duplicate .gitignore entries"
else
    f_LOG_error "repeat setup changed .gitignore"
    LOCAL_status=1
fi
LOCAL_mainLines=$(grep -Fxc -- "/main.sh" "$LOCAL_target/.gitignore")
if [[ "$LOCAL_mainLines" == "1" ]]; then
    f_LOG_ok "main.sh is ignored once"
else
    f_LOG_error "main.sh ignore count is $LOCAL_mainLines"
    LOCAL_status=1
fi

f_make_repo existing
printf 'custom/\nbuild/\n' > "$LOCAL_target/.gitignore"
LOCAL_output=$(f_run_setup "$LOCAL_target")
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "setup with existing .gitignore succeeds"
for LOCAL_pattern in "custom/" "build/" "/main.sh" "__pycache__/"; do
    if grep -Fqx -- "$LOCAL_pattern" "$LOCAL_target/.gitignore"; then
        f_LOG_ok "existing .gitignore contains $LOCAL_pattern"
    else
        f_LOG_error "existing .gitignore lacks $LOCAL_pattern"
        LOCAL_status=1
    fi
done
LOCAL_buildLines=$(grep -Fxc -- "build/" "$LOCAL_target/.gitignore")
if [[ "$LOCAL_buildLines" == "1" ]]; then
    f_LOG_ok "existing entries are not duplicated"
else
    f_LOG_error "build/ count is $LOCAL_buildLines"
    LOCAL_status=1
fi

f_make_repo custom_starter
printf '#!/bin/bash\necho keep\n' > "$LOCAL_target/main.sh"
chmod +x -- "$LOCAL_target/main.sh"
LOCAL_output=$(f_run_setup "$LOCAL_target")
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "setup with an existing main.sh succeeds"
f_check_contains "exists and differs" "existing main.sh is reported"
if grep -Fqx -- "echo keep" "$LOCAL_target/main.sh"; then
    f_LOG_ok "existing main.sh is preserved without --force"
else
    f_LOG_error "existing main.sh was overwritten without --force"
    LOCAL_status=1
fi
LOCAL_output=$(f_run_setup "$LOCAL_target" --force)
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "setup --force succeeds"
if grep -Fqx -- "echo keep" "$LOCAL_target/main.sh"; then
    f_LOG_error "--force did not replace the existing main.sh"
    LOCAL_status=1
else
    f_LOG_ok "--force replaces the existing main.sh"
fi

f_make_repo hookrun
LOCAL_output=$(f_run_setup "$LOCAL_target")
printf 'TODO\n' > "$LOCAL_target/leak.txt"
git -C "$LOCAL_target" add -- leak.txt
LOCAL_message="$LOCAL_workDir/message.txt"
printf 'clean message\n' > "$LOCAL_message"
LOCAL_output=$( ( cd -- "$LOCAL_target" && bash -- .githooks/check-debug-words.sh "$LOCAL_message" ) 2>&1 )
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "installed hooks block forbidden words"
f_check_contains "TODO" "installed hooks report the forbidden word"
f_check_contains "leak.txt" "installed hooks report the file"

LOCAL_target="$LOCAL_workDir/not_a_repo"
mkdir -p -- "$LOCAL_target"
LOCAL_output=$(f_run_setup "$LOCAL_target")
LOCAL_rc=$?
f_check_status 0 "$LOCAL_rc" "setup initializes a missing repository"
if [[ -d "$LOCAL_target/.git" ]]; then
    f_LOG_ok "missing repository was initialized"
else
    f_LOG_error "missing repository was not initialized"
    LOCAL_status=1
fi

f_make_repo parent
mkdir -p -- "$LOCAL_target/subdir"
LOCAL_output=$(f_run_setup "$LOCAL_target/subdir")
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "setup rejects a non-root project directory"
f_check_contains "not the Git repository root" "non-root rejection is reported"

LOCAL_output=$(f_run_setup "$LOCAL_infra")
LOCAL_rc=$?
f_check_status 1 "$LOCAL_rc" "setup rejects the infrastructure repository itself"
f_check_contains "Components-Infrastructure repository itself" "infrastructure rejection is reported"

if (( LOCAL_status == 0 )); then
    f_LOG_ok "all setup-repo tests passed"
else
    f_LOG_error "setup-repo tests failed"
fi
exit "$LOCAL_status"
