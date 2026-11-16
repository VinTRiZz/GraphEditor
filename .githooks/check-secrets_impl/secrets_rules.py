#!/usr/bin/env python3
"""Load check-secrets rule files and the allowlist."""

from __future__ import annotations

import configparser
import re
from dataclasses import dataclass, field
from pathlib import Path


class RuleError(RuntimeError):
    pass


_FLAG_MAP = {
    "i": re.IGNORECASE,
    "m": re.MULTILINE,
    "s": re.DOTALL,
    "x": re.VERBOSE,
    "a": re.ASCII,
}


def _parse_flags(raw: str, where: str) -> int:
    flags = 0
    for character in raw.replace(",", "").replace(" ", ""):
        try:
            flags |= _FLAG_MAP[character.lower()]
        except KeyError as exc:
            raise RuleError(f"{where}: unknown regex flag {character!r}") from exc
    return flags


def _parse_bool(raw: str, where: str) -> bool:
    value = raw.strip().lower()
    if value in {"1", "true", "yes", "on"}:
        return True
    if value in {"0", "false", "no", "off"}:
        return False
    raise RuleError(f"{where}: expected a boolean, got {raw!r}")


def _compile_optional(raw: str | None, where: str, flags: int = 0) -> re.Pattern[str] | None:
    if raw is None:
        return None
    value = raw.strip()
    if not value:
        return None
    try:
        return re.compile(value, flags)
    except re.error as exc:
        raise RuleError(f"{where}: invalid regular expression: {exc}") from exc


@dataclass(frozen=True)
class Rule:
    rule_id: str
    kind: str
    pattern: str
    description: str
    source: str
    regex: re.Pattern[str] | None = None
    literal: str | None = None
    scope: str = "both"
    paths: re.Pattern[str] | None = None
    exclude_paths: re.Pattern[str] | None = None
    enabled: bool = True
    min_length: int = 24
    threshold: float = 4.5

    def applies_to_scope(self, scope: str) -> bool:
        return self.scope == "both" or self.scope == scope

    def applies_to_path(self, path: str) -> bool:
        if self.paths is not None and self.paths.search(path) is None:
            return False
        if self.exclude_paths is not None and self.exclude_paths.search(path) is not None:
            return False
        return True


@dataclass
class AllowList:
    paths: list[re.Pattern[str]] = field(default_factory=list)
    lines: list[re.Pattern[str]] = field(default_factory=list)
    values: list[re.Pattern[str]] = field(default_factory=list)
    rules: set[str] = field(default_factory=set)

    def path_allowed(self, path: str) -> bool:
        return any(pattern.search(path) is not None for pattern in self.paths)

    def line_allowed(self, line: str) -> bool:
        return any(pattern.search(line) is not None for pattern in self.lines)

    def value_allowed(self, value: str) -> bool:
        return any(pattern.search(value) is not None for pattern in self.values)


def load_allowlist(path: Path) -> AllowList:
    allowlist = AllowList()
    if not path.is_file():
        return allowlist

    try:
        content = path.read_text(encoding="utf-8")
    except (OSError, UnicodeError) as exc:
        raise RuleError(f"{path}: cannot read allowlist: {exc}") from exc

    for line_number, raw_line in enumerate(content.splitlines(), 1):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue

        kind, separator, value = line.partition(":")
        if not separator:
            raise RuleError(f"{path}:{line_number}: expected '<kind>:<value>'")
        kind = kind.strip().lower()
        value = value.strip()

        if kind == "rule":
            if not value:
                raise RuleError(f"{path}:{line_number}: empty rule id")
            allowlist.rules.add(value)
            continue

        if kind not in {"path", "line", "value"}:
            raise RuleError(f"{path}:{line_number}: unknown allowlist kind {kind!r}")
        if not value:
            raise RuleError(f"{path}:{line_number}: empty allowlist value")

        try:
            compiled = re.compile(value, re.IGNORECASE)
        except re.error as exc:
            raise RuleError(f"{path}:{line_number}: invalid regular expression: {exc}") from exc
        getattr(allowlist, kind + "s").append(compiled)

    return allowlist


def _load_rule_file(path: Path) -> list[Rule]:
    parser = configparser.RawConfigParser(
        interpolation=None,
        inline_comment_prefixes=None,
        delimiters=("=",),
        strict=True,
    )
    try:
        parser.read(path, encoding="utf-8")
    except (configparser.Error, OSError, UnicodeError) as exc:
        raise RuleError(f"{path}: cannot parse rule file: {exc}") from exc

    rules: list[Rule] = []
    for section in parser.sections():
        rule_id = section.strip()
        where = f"{path.name}:[{rule_id}]"
        values = parser[section]

        kind = values.get("type", "").strip().lower()
        if kind not in {"regex", "literal", "entropy"}:
            raise RuleError(f"{where}: type must be regex, literal or entropy")

        description = values.get("description", "").strip() or rule_id
        scope = values.get("scope", "both").strip().lower() or "both"
        if scope not in {"staged", "message", "both"}:
            raise RuleError(f"{where}: scope must be staged, message or both")

        enabled = _parse_bool(values.get("enabled", "true"), where)
        flags = _parse_flags(values.get("flags", ""), where)
        paths = _compile_optional(values.get("paths"), f"{where}.paths")
        exclude_paths = _compile_optional(
            values.get("exclude_paths"), f"{where}.exclude_paths"
        )

        pattern = values.get("pattern", "")
        regex: re.Pattern[str] | None = None
        literal: str | None = None
        min_length = 24
        threshold = 4.5

        if kind == "regex":
            if not pattern.strip():
                raise RuleError(f"{where}: regex rule requires a pattern")
            try:
                regex = re.compile(pattern, flags)
            except re.error as exc:
                raise RuleError(f"{where}: invalid pattern: {exc}") from exc
        elif kind == "literal":
            if not pattern:
                raise RuleError(f"{where}: literal rule requires a pattern")
            literal = pattern
        else:
            raw_min_length = values.get("min_length", "24").strip()
            raw_threshold = values.get("threshold", "4.5").strip()
            try:
                min_length = int(raw_min_length)
            except ValueError as exc:
                raise RuleError(f"{where}: min_length must be an integer") from exc
            try:
                threshold = float(raw_threshold)
            except ValueError as exc:
                raise RuleError(f"{where}: threshold must be a number") from exc
            if min_length < 8:
                raise RuleError(f"{where}: min_length must be at least 8")
            if threshold <= 0:
                raise RuleError(f"{where}: threshold must be positive")
            if not pattern.strip():
                pattern = (
                    rf"(?<![A-Za-z0-9+/=_-])[A-Za-z0-9+/_-]{{{min_length},}}"
                    rf"={{0,2}}(?![A-Za-z0-9+/=_-])"
                )
            try:
                regex = re.compile(pattern, flags)
            except re.error as exc:
                raise RuleError(f"{where}: invalid pattern: {exc}") from exc

        rules.append(
            Rule(
                rule_id=rule_id,
                kind=kind,
                pattern=pattern,
                description=description,
                source=path.name,
                regex=regex,
                literal=literal,
                scope=scope,
                paths=paths,
                exclude_paths=exclude_paths,
                enabled=enabled,
                min_length=min_length,
                threshold=threshold,
            )
        )
    return rules


def load_rules(rules_dir: Path) -> tuple[list[Rule], AllowList]:
    if not rules_dir.is_dir():
        raise RuleError(f"rules directory not found: {rules_dir}")

    rule_files = sorted(rules_dir.glob("*.rules"))
    if not rule_files:
        raise RuleError(f"no *.rules files found in {rules_dir}")

    rules: list[Rule] = []
    seen: dict[str, str] = {}
    for path in rule_files:
        for rule in _load_rule_file(path):
            if rule.rule_id in seen:
                raise RuleError(
                    f"{path.name}:[{rule.rule_id}]: duplicate rule id, "
                    f"already defined in {seen[rule.rule_id]}"
                )
            seen[rule.rule_id] = path.name
            rules.append(rule)

    allowlist = load_allowlist(rules_dir / "allowlist.txt")
    return rules, allowlist
