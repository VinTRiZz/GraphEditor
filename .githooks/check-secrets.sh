#!/usr/bin/env bash
set -euo pipefail

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
LOCAL_python="${SECRETS_PYTHON:-python3}"
LOCAL_scanner="${LOCAL_scriptBasedir}/check-secrets_impl/scan_secrets.py"
LOCAL_rulesDir="${SECRETS_RULES_DIR:-${LOCAL_scriptBasedir}/secrets-rules}"

f_show_help() {
  f_LOG_info "Usage: check-secrets.sh [COMMIT_MSG_FILE] [--show-values] [--no-color]"
  f_LOG_info ""
  f_LOG_info "Scans files staged with git add, and optionally a commit message file."
  f_LOG_info "Rules: ${LOCAL_rulesDir}"
  f_LOG_info "Environment:"
  f_LOG_info "  SECRETS_RULES_DIR      override rules directory"
  f_LOG_info "  SECRETS_PYTHON         python3 interpreter to use"
  f_LOG_info "  SECRETS_MAX_FILE_SIZE  maximum staged file size in bytes"
  f_LOG_info "  SECRETS_SHOW_VALUES    set to 1 to print matched values"
}

for LOCAL_arg in "$@"; do
  if [[ "$LOCAL_arg" == "--help" ]]; then
    f_show_help
    exit 0
  fi
done

if [[ ! -f "$LOCAL_scanner" ]]; then
  f_LOG_error "check-secrets: scanner not found: $LOCAL_scanner"
  exit 2
fi

if ! command -v "$LOCAL_python" >/dev/null 2>&1; then
  f_LOG_error "check-secrets: Python 3 interpreter not found: $LOCAL_python"
  f_LOG_error "check-secrets: install python3 or set SECRETS_PYTHON"
  exit 2
fi

if [[ ! -d "$LOCAL_rulesDir" ]]; then
  f_LOG_error "check-secrets: rules directory not found: $LOCAL_rulesDir"
  exit 2
fi

if ! LOCAL_repoRoot="$(git rev-parse --show-toplevel 2>/dev/null)"; then
  f_LOG_error "check-secrets: not inside a git repository"
  exit 2
fi

LOCAL_args=(--repo "$LOCAL_repoRoot" --rules "$LOCAL_rulesDir")

if [[ -n "${SECRETS_MAX_FILE_SIZE:-}" ]]; then
  LOCAL_args+=(--max-file-size "$SECRETS_MAX_FILE_SIZE")
fi

case "${SECRETS_SHOW_VALUES:-0}" in
  1|true|TRUE|yes|YES|on|ON) LOCAL_args+=(--show-values) ;;
esac

while (( $# > 0 )); do
  case "$1" in
    --help)
      f_show_help
      exit 0
      ;;
    --show-values|--no-color)
      LOCAL_args+=("$1")
      shift
      ;;
    --message-file)
      shift
      if (( $# == 0 )); then
        f_LOG_error "check-secrets: --message-file requires a path"
        exit 2
      fi
      LOCAL_args+=(--message-file "$1")
      shift
      ;;
    --*)
      f_LOG_error "check-secrets: unknown option: $1"
      exit 2
      ;;
    *)
      if [[ -f "$1" ]]; then
        LOCAL_args+=(--message-file "$1")
      fi
      shift
      ;;
  esac
done

export PYTHONDONTWRITEBYTECODE=1
exec "$LOCAL_python" "$LOCAL_scanner" "${LOCAL_args[@]}"
