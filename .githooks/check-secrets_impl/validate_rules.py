#!/usr/bin/env python3
"""Validate check-secrets rule files without scanning a repository."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

import secrets_rules


def f_LOG_info(message: str = "") -> None:
    print(message)


def f_LOG_error(message: str = "") -> None:
    print(message, file=sys.stderr)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Validate check-secrets *.rules files and allowlist.txt."
    )
    parser.add_argument(
        "--rules",
        default=str(Path(__file__).resolve().parent.parent / "secrets-rules"),
        help="directory with *.rules and allowlist.txt",
    )
    args = parser.parse_args(argv)

    rules_dir = Path(args.rules)
    try:
        rules, allowlist = secrets_rules.load_rules(rules_dir)
    except secrets_rules.RuleError as exc:
        f_LOG_error(f"validate_rules: {exc}")
        return 2

    enabled = sum(1 for rule in rules if rule.enabled)
    f_LOG_info(
        f"validate_rules: OK, rules={len(rules)}, enabled={enabled}, "
        f"allowlist paths={len(allowlist.paths)}, "
        f"lines={len(allowlist.lines)}, values={len(allowlist.values)}, "
        f"rule ids={len(allowlist.rules)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
