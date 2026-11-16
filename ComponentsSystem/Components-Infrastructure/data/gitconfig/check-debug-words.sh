#!/usr/bin/env bash
# commit-msg hook helper: block commits that contain forbidden words or phrases
# (up to 3 words) in the staged content or in the commit message.
#
# Install from the repository root:
#   chmod +x data/gitconfig/check-debug-words.sh data/gitconfig/commit-msg
#   git config --local core.hooksPath data/gitconfig
#
# Word list: data/gitconfig/forbidden-words.txt
# Bypass:    git commit --no-verify

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

LOCAL_matchWholeWord="true"
LOCAL_maxMatchesPerPhrase=20
LOCAL_maxLineLength=200

LOCAL_scriptPath="$(realpath -- "${BASH_SOURCE[0]}")"
LOCAL_scriptBasedir="$(dirname -- "$LOCAL_scriptPath")"

LOCAL_messageFile=""
if (( $# > 0 )); then
  case "$1" in
    -h|--help)
      f_LOG_info "Usage: check-debug-words.sh [COMMIT_MSG_FILE]"
      f_LOG_info "Scan staged files and an optional commit message for forbidden words."
      f_LOG_info "Word list: ${LOCAL_scriptBasedir}/forbidden-words.txt"
      exit 0
      ;;
    *)
      if [[ -f "$1" && -r "$1" ]]; then
        LOCAL_messageFile="$(realpath -- "$1")"
      else
        f_LOG_warn "check-debug-words: ignoring unreadable message file: $1"
      fi
      ;;
  esac
fi

# Resolve a relative override before changing the working directory.
LOCAL_envWords="${GIT_HOOK_FORBIDDEN_WORDS:-}"
if [[ -n "$LOCAL_envWords" && -f "$LOCAL_envWords" ]]; then
  LOCAL_envWords="$(realpath -- "$LOCAL_envWords")"
fi

if [[ -z "${LC_ALL:-}" ]]; then
  export LC_ALL="C.UTF-8"
fi

LOCAL_repoRoot="$(git rev-parse --show-toplevel 2>/dev/null)" || exit 0
[[ -n "$LOCAL_repoRoot" ]] || exit 0
cd -- "$LOCAL_repoRoot" || exit 0

LOCAL_hooksDir="$(git rev-parse --git-path hooks 2>/dev/null || true)"
LOCAL_wordsFile=""
for LOCAL_candidate in "$LOCAL_envWords" "${LOCAL_scriptBasedir}/forbidden-words.txt" "${LOCAL_hooksDir}/forbidden-words.txt"; do
  if [[ -n "$LOCAL_candidate" && -f "$LOCAL_candidate" && -r "$LOCAL_candidate" ]]; then
    LOCAL_wordsFile="$LOCAL_candidate"
    break
  fi
done

if [[ -z "$LOCAL_wordsFile" ]]; then
  f_LOG_warn "check-debug-words: forbidden word list not found; check skipped."
  f_LOG_warn "                  expected: ${LOCAL_scriptBasedir}/forbidden-words.txt"
  exit 0
fi

LOCAL_wordsRelPath="$(realpath --relative-to="$LOCAL_repoRoot" -- "$LOCAL_wordsFile" 2>/dev/null || true)"
case "$LOCAL_wordsRelPath" in
  ""|.|/*|../*|..) LOCAL_wordsRelPath="" ;;
esac

# Never scan the hook implementation itself; unlike projects, its files
# intentionally mention the forbidden terms.
LOCAL_scriptRelDir="$(realpath --relative-to="$LOCAL_repoRoot" -- "$LOCAL_scriptBasedir" 2>/dev/null || true)"
LOCAL_skipPrefix=""
case "$LOCAL_scriptRelDir" in
  ""|.|/*|../*|..) : ;;
  *) LOCAL_skipPrefix="${LOCAL_scriptRelDir}/" ;;
esac

declare -a LOCAL_stagedFiles=()
if git rev-parse --verify --quiet HEAD >/dev/null 2>&1; then
  LOCAL_listFilesCmd=(git diff --cached --name-only --diff-filter=ACMR -z)
else
  LOCAL_listFilesCmd=(git ls-files --cached -z)
fi

while IFS= read -r -d '' LOCAL_stagedFile; do
  [[ ${LOCAL_stagedFile##*/} == ".gitignore" ]] && continue
  [[ -n "$LOCAL_wordsRelPath" && "$LOCAL_stagedFile" == "$LOCAL_wordsRelPath" ]] && continue
  [[ -n "$LOCAL_skipPrefix" && "$LOCAL_stagedFile" == "$LOCAL_skipPrefix"* ]] && continue
  LOCAL_stagedFiles+=("$LOCAL_stagedFile")
done < <("${LOCAL_listFilesCmd[@]}" 2>/dev/null)

if (( ${#LOCAL_stagedFiles[@]} == 0 )) && [[ -z "$LOCAL_messageFile" ]]; then
  exit 0
fi

declare -a LOCAL_patternList=()
declare -a LOCAL_labelList=()
declare -a LOCAL_configErrors=()
LOCAL_lineNumber=0

while IFS= read -r LOCAL_rawLine || [[ -n "$LOCAL_rawLine" ]]; do
  LOCAL_lineNumber=$(( LOCAL_lineNumber + 1 ))
  LOCAL_line="${LOCAL_rawLine%$'\r'}"
  LOCAL_line="${LOCAL_line#"${LOCAL_line%%[![:space:]]*}"}"
  LOCAL_line="${LOCAL_line%"${LOCAL_line##*[![:space:]]}"}"
  [[ -z "$LOCAL_line" || "$LOCAL_line" == '#'* ]] && continue

  read -r -a LOCAL_words <<< "$LOCAL_line"
  if (( ${#LOCAL_words[@]} > 3 )); then
    LOCAL_configErrors+=("line ${LOCAL_lineNumber}: «${LOCAL_line}» has ${#LOCAL_words[@]} words, maximum is 3")
    continue
  fi

  # Escape ERE metacharacters and join words with a whitespace separator.
  LOCAL_pattern=""
  for LOCAL_word in "${LOCAL_words[@]}"; do
    LOCAL_escapedWord=""
    LOCAL_charIndex=0
    while (( LOCAL_charIndex < ${#LOCAL_word} )); do
      LOCAL_char="${LOCAL_word:LOCAL_charIndex:1}"
      case "$LOCAL_char" in
        '['|']'|'\'|'.'|'^'|'$'|'*'|'+'|'?'|'('|')'|'{'|'}'|'|')
          LOCAL_escapedWord+="\\${LOCAL_char}" ;;
        *)
          LOCAL_escapedWord+="${LOCAL_char}" ;;
      esac
      LOCAL_charIndex=$(( LOCAL_charIndex + 1 ))
    done
    [[ -n "$LOCAL_pattern" ]] && LOCAL_pattern+="[[:space:]]+"
    LOCAL_pattern+="$LOCAL_escapedWord"
  done

  if [[ "$LOCAL_matchWholeWord" == "true" && -n "$LOCAL_pattern" ]]; then
    LOCAL_firstWord="${LOCAL_words[0]}"
    LOCAL_lastWord="${LOCAL_words[${#LOCAL_words[@]} - 1]}"
    [[ "$LOCAL_firstWord" == [[:alnum:]_]* ]] && LOCAL_pattern="\\<${LOCAL_pattern}"
    [[ "$LOCAL_lastWord" == *[[:alnum:]_] ]] && LOCAL_pattern="${LOCAL_pattern}\\>"
  fi

  LOCAL_patternList+=("$LOCAL_pattern")
  LOCAL_labelList+=("$LOCAL_line")
done < "$LOCAL_wordsFile"

if (( ${#LOCAL_configErrors[@]} > 0 )); then
  f_LOG_error ""
  f_LOG_error "✖ check-debug-words: invalid word list: ${LOCAL_wordsFile}"
  f_LOG_error ""
  for LOCAL_configError in "${LOCAL_configErrors[@]}"; do
    f_LOG_error "  • ${LOCAL_configError}"
  done
  f_LOG_error ""
  f_LOG_error "  An entry can contain at most 3 words."
  f_LOG_error ""
  exit 1
fi

if (( ${#LOCAL_patternList[@]} == 0 )); then
  exit 0
fi

LOCAL_colorRed=""
LOCAL_colorReset=""
if [[ -t 2 ]]; then
  LOCAL_colorRed=$'\033[1;31m'
  LOCAL_colorReset=$'\033[0m'
fi

LOCAL_totalMatches=0
LOCAL_hitPhrases=0
LOCAL_report=""

# The verbose diff after the scissors line is not part of the commit message.
LOCAL_messageLimit=""
if [[ -n "$LOCAL_messageFile" ]]; then
  LOCAL_scissorsLine="$(grep -n -m1 -E '^#[[:space:]]+-+[[:space:]]+>8' -- "$LOCAL_messageFile" 2>/dev/null | cut -d: -f1)"
  if [[ -n "$LOCAL_scissorsLine" ]]; then
    LOCAL_messageLimit=$(( LOCAL_scissorsLine - 1 ))
  fi
fi

for LOCAL_index in "${!LOCAL_patternList[@]}"; do
  LOCAL_pattern="${LOCAL_patternList[$LOCAL_index]}"
  LOCAL_label="${LOCAL_labelList[$LOCAL_index]}"
  LOCAL_matchLines=()

  if (( ${#LOCAL_stagedFiles[@]} > 0 )); then
    LOCAL_stagedMatches="$(git grep --cached -n -I -i -E -e "$LOCAL_pattern" -- "${LOCAL_stagedFiles[@]}" 2>/dev/null)"
    LOCAL_grepStatus=$?
    if (( LOCAL_grepStatus > 1 )); then
      f_LOG_error ""
      f_LOG_error "✖ check-debug-words: git grep failed (status ${LOCAL_grepStatus}) for pattern: ${LOCAL_pattern}"
      f_LOG_error "  Commit blocked to avoid missing forbidden words."
      f_LOG_error ""
      exit 1
    fi
    if [[ -n "$LOCAL_stagedMatches" ]]; then
      mapfile -t LOCAL_stagedLines <<< "$LOCAL_stagedMatches"
      LOCAL_matchLines+=("${LOCAL_stagedLines[@]}")
    fi
  fi

  if [[ -n "$LOCAL_messageFile" ]]; then
    if [[ -n "$LOCAL_messageLimit" ]]; then
      LOCAL_messageMatches="$(head -n "$LOCAL_messageLimit" -- "$LOCAL_messageFile" | grep -Hn -I -i -E --label=COMMIT_MSG -e "$LOCAL_pattern" - 2>/dev/null)"
    else
      LOCAL_messageMatches="$(grep -Hn -I -i -E --label=COMMIT_MSG -e "$LOCAL_pattern" - < "$LOCAL_messageFile" 2>/dev/null)"
    fi
    LOCAL_grepStatus=$?
    if (( LOCAL_grepStatus > 1 )); then
      f_LOG_error ""
      f_LOG_error "✖ check-debug-words: cannot scan the commit message: ${LOCAL_messageFile}"
      f_LOG_error ""
      exit 1
    fi
    if [[ -n "$LOCAL_messageMatches" ]]; then
      while IFS= read -r LOCAL_messageHit; do
        LOCAL_messageRest="${LOCAL_messageHit#*:}"
        LOCAL_messageText="${LOCAL_messageRest#*:}"
        # Skip regular Git comment lines.
        [[ "$LOCAL_messageText" =~ ^[[:space:]]*# ]] && continue
        LOCAL_matchLines+=("${LOCAL_messageHit}")
      done <<< "$LOCAL_messageMatches"
    fi
  fi

  (( ${#LOCAL_matchLines[@]} == 0 )) && continue

  LOCAL_hitPhrases=$(( LOCAL_hitPhrases + 1 ))
  LOCAL_totalMatches=$(( LOCAL_totalMatches + ${#LOCAL_matchLines[@]} ))
  LOCAL_report+="  ▸ «${LOCAL_label}» — found: ${#LOCAL_matchLines[@]}"$'\n'

  LOCAL_shown=0
  for LOCAL_matchLine in "${LOCAL_matchLines[@]}"; do
    if (( LOCAL_shown >= LOCAL_maxMatchesPerPhrase )); then
      LOCAL_report+="      … and $(( ${#LOCAL_matchLines[@]} - LOCAL_shown )) more"$'\n'
      break
    fi
    LOCAL_shown=$(( LOCAL_shown + 1 ))
    if (( ${#LOCAL_matchLine} > LOCAL_maxLineLength )); then
      LOCAL_report+="      ${LOCAL_matchLine:0:LOCAL_maxLineLength}…"$'\n'
    else
      LOCAL_report+="      ${LOCAL_matchLine}"$'\n'
    fi
  done

  LOCAL_report+=$'\n'
done

if (( LOCAL_totalMatches == 0 )); then
  exit 0
fi

f_LOG_error ""
f_LOG_error "----------------------------------------------------------------------"
f_LOG_error "${LOCAL_colorRed} ✖  COMMIT BLOCKED${LOCAL_colorReset}"
f_LOG_error "   Forbidden words or phrases were found."
f_LOG_error "----------------------------------------------------------------------"
f_LOG_error ""
f_LOG_error "$LOCAL_report"
f_LOG_error "  Total: matches — ${LOCAL_totalMatches}, entries — ${LOCAL_hitPhrases}"
f_LOG_error "  Word list: ${LOCAL_wordsFile}"
f_LOG_error ""
f_LOG_error "  Fix the files (or update the word list) and commit again."
f_LOG_error "  Intentional bypass: git commit --no-verify"
f_LOG_error ""

exit 1
