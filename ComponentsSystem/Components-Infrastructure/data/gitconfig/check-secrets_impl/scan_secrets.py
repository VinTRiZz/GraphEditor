#!/usr/bin/env python3
"""Scan staged Git blobs and an optional commit message for secrets."""

from __future__ import annotations

import argparse
import codecs
import math
import os
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

import git_staged
import secrets_rules


def f_LOG_info(message: str = "") -> None:
    print(message)


def f_LOG_warn(message: str = "") -> None:
    print(message, file=sys.stderr)


def f_LOG_error(message: str = "") -> None:
    print(message, file=sys.stderr)


@dataclass(frozen=True)
class Finding:
    path: str
    line_number: int
    rule_id: str
    description: str
    value: str


def shannon_entropy(value: str) -> float:
    if not value:
        return 0.0
    length = len(value)
    return -sum(
        (count / length) * math.log2(count / length)
        for count in Counter(value).values()
    )


def decode_text(data: bytes) -> str | None:
    if data.startswith(codecs.BOM_UTF16_LE) or data.startswith(codecs.BOM_UTF16_BE):
        try:
            return data.decode("utf-16")
        except UnicodeError:
            return None
    if data.startswith(codecs.BOM_UTF8):
        data = data[len(codecs.BOM_UTF8) :]
    if b"\0" in data[:8192]:
        return None
    try:
        return data.decode("utf-8")
    except UnicodeDecodeError:
        return data.decode("utf-8", "replace")


def redact(value: str, show_values: bool) -> str:
    if show_values:
        return value
    if len(value) <= 4:
        return "*" * len(value)
    if len(value) <= 12:
        return value[0] + "*" * (len(value) - 2) + value[-1]
    return value[:4] + "…" + value[-4:]


def normalize_value(value: str) -> str:
    value = value.strip()
    while value.endswith((",", ";")):
        value = value[:-1].rstrip()
    if (
        len(value) >= 2
        and value[0] == value[-1]
        and value[0] in {"\"", "'"}
    ):
        value = value[1:-1]
    return value


def match_values(rule: secrets_rules.Rule, line: str):
    if rule.regex is None:
        if rule.literal and rule.literal in line:
            yield normalize_value(rule.literal)
        return

    for match in rule.regex.finditer(line):
        groups = match.groupdict()
        if "value" in groups:
            value = groups["value"]
            if value is None:
                value = match.group(0)
        else:
            value = match.group(0)
        yield normalize_value(value)


def scan_text(
    text: str,
    display_path: str,
    scope: str,
    rules: list[secrets_rules.Rule],
    allowlist: secrets_rules.AllowList,
    max_matches_per_line: int = 5,
) -> list[Finding]:
    findings: list[Finding] = []
    applicable_rules = [
        rule
        for rule in rules
        if rule.enabled
        and rule.applies_to_scope(scope)
        and rule.rule_id not in allowlist.rules
        and rule.applies_to_path(display_path)
    ]

    for line_number, line in enumerate(text.splitlines(), 1):
        if "check-secrets:ignore" in line.lower():
            continue
        if allowlist.line_allowed(line):
            continue

        line_values: set[str] = set()
        for rule in applicable_rules:
            matches = 0
            for value in match_values(rule, line):
                if not value or value in line_values:
                    continue
                if rule.kind == "entropy":
                    if len(value) < rule.min_length:
                        continue
                    if value.isdigit():
                        continue
                    if len(set(value)) < 8:
                        continue
                    if shannon_entropy(value) < rule.threshold:
                        continue
                if allowlist.value_allowed(value):
                    continue

                line_values.add(value)
                findings.append(
                    Finding(
                        path=display_path,
                        line_number=line_number,
                        rule_id=rule.rule_id,
                        description=rule.description,
                        value=value,
                    )
                )
                matches += 1
                if matches >= max_matches_per_line:
                    break

    return findings


def scan_staged(
    repo: Path,
    rules: list[secrets_rules.Rule],
    allowlist: secrets_rules.AllowList,
    max_file_size: int,
) -> tuple[list[Finding], list[str]]:
    findings: list[Finding] = []
    warnings: list[str] = []

    for blob in git_staged.iter_staged_blobs(repo, max_file_size):
        path = blob.path.replace("\\", "/")
        if allowlist.path_allowed(path):
            continue
        if blob.size > max_file_size:
            warnings.append(
                f"check-secrets: пропущен файл {path} "
                f"({blob.size} байт > {max_file_size})"
            )
            continue

        text = decode_text(blob.data)
        if text is None:
            continue

        findings.extend(
            scan_text(text, path, "staged", rules, allowlist)
        )

    return findings, warnings


def scan_message_file(
    path: Path,
    rules: list[secrets_rules.Rule],
    allowlist: secrets_rules.AllowList,
) -> list[Finding]:
    data = path.read_bytes()
    text = decode_text(data)
    if text is None:
        text = data.decode("utf-8", "replace")
    return scan_text(text, "COMMIT_MSG", "message", rules, allowlist)


def render_report(
    findings: list[Finding],
    rules_dir: Path,
    show_values: bool,
    no_color: bool,
) -> None:
    use_color = (
        not no_color
        and sys.stderr.isatty()
        and "NO_COLOR" not in os.environ
    )
    red = "\033[1;31m" if use_color else ""
    dim = "\033[2m" if use_color else ""
    reset = "\033[0m" if use_color else ""

    files = {finding.path for finding in findings}
    f_LOG_error()
    f_LOG_error(
        f"{red}✖ check-secrets: найдены потенциальные секреты "
        f"({len(findings)}){reset}"
    )
    f_LOG_error()

    for finding in findings:
        f_LOG_error(
            f"  {finding.path}:{finding.line_number}  "
            f"[{finding.rule_id}] {finding.description}"
        )
        f_LOG_error(
            f"      {dim}{redact(finding.value, show_values)}{reset}"
        )

    f_LOG_error()
    f_LOG_error(
        f"  Итого: совпадений — {len(findings)}, файлов — {len(files)}"
    )
    f_LOG_error(f"  Правила: {rules_dir}")
    f_LOG_error("  Осознанный обход проверки: git commit --no-verify")
    f_LOG_error()


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Find secrets in files staged with git add."
    )
    parser.add_argument("--repo", default=".", help="repository path")
    parser.add_argument("--rules", help="directory with *.rules and allowlist.txt")
    parser.add_argument(
        "--message-file",
        help="optional commit message file to scan as COMMIT_MSG",
    )
    parser.add_argument(
        "--max-file-size",
        type=int,
        default=5 * 1024 * 1024,
        help="skip staged files larger than this many bytes",
    )
    parser.add_argument(
        "--show-values",
        action="store_true",
        help="print matched values without redaction",
    )
    parser.add_argument(
        "--no-color",
        action="store_true",
        help="disable colored output",
    )
    args = parser.parse_args(argv)

    rules_dir = (
        Path(args.rules)
        if args.rules
        else Path(__file__).resolve().parent.parent / "secrets-rules"
    )

    try:
        repo = git_staged.repo_root(args.repo)
        rules, allowlist = secrets_rules.load_rules(rules_dir)
    except (git_staged.GitError, secrets_rules.RuleError) as exc:
        f_LOG_error(f"check-secrets: {exc}")
        return 2

    if not any(rule.enabled for rule in rules):
        f_LOG_error("check-secrets: нет включённых правил")
        return 2

    findings: list[Finding] = []
    try:
        staged_findings, warnings = scan_staged(
            repo, rules, allowlist, args.max_file_size
        )
    except git_staged.GitError as exc:
        f_LOG_error(f"check-secrets: {exc}")
        return 2
    findings.extend(staged_findings)

    if args.message_file:
        message_path = Path(args.message_file)
        if not message_path.is_absolute() and not message_path.exists():
            message_path = repo / message_path
        try:
            findings.extend(scan_message_file(message_path, rules, allowlist))
        except OSError as exc:
            f_LOG_error(
                f"check-secrets: cannot read commit message "
                f"{message_path}: {exc}"
            )
            return 2

    for warning in warnings:
        f_LOG_warn(warning)

    findings.sort(key=lambda item: (item.path, item.line_number, item.rule_id))
    if findings:
        render_report(findings, rules_dir, args.show_values, args.no_color)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
