#!/usr/bin/env python3
"""Read blobs that are currently staged in the Git index."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterator


def f_LOG_error(message: str = "") -> None:
    print(message, file=sys.stderr)


class GitError(RuntimeError):
    pass


@dataclass(frozen=True)
class IndexEntry:
    path: str
    mode: str
    oid: str
    stage: int


@dataclass(frozen=True)
class StagedBlob:
    path: str
    mode: str
    oid: str
    data: bytes
    size: int = 0


def _git(repo: Path, *args: str) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(
        ["git", "-C", os.fspath(repo), *args],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )


def repo_root(path: str | os.PathLike[str] = ".") -> Path:
    result = _git(Path(path), "rev-parse", "--show-toplevel")
    if result.returncode != 0:
        message = result.stderr.decode("utf-8", "replace").strip()
        raise GitError(message or "not a git repository")
    return Path(os.fsdecode(result.stdout.strip()))


def _decode_path(raw: bytes) -> str:
    return os.fsdecode(raw)


def index_entries(repo: Path) -> dict[str, IndexEntry]:
    result = _git(repo, "ls-files", "--stage", "-z")
    if result.returncode != 0:
        message = result.stderr.decode("utf-8", "replace").strip()
        raise GitError(message or "git ls-files failed")

    entries: dict[str, IndexEntry] = {}
    for record in result.stdout.split(b"\0"):
        if not record:
            continue
        meta, separator, path_raw = record.partition(b"\t")
        if not separator:
            continue
        fields = meta.split()
        if len(fields) != 3:
            continue
        mode = fields[0].decode("ascii", "replace")
        oid = fields[1].decode("ascii", "replace")
        try:
            stage = int(fields[2])
        except ValueError:
            continue
        if stage != 0:
            continue
        path = _decode_path(path_raw)
        entries[path] = IndexEntry(path, mode, oid, stage)
    return entries


def staged_paths(repo: Path) -> list[str]:
    has_head = _git(repo, "rev-parse", "--verify", "--quiet", "HEAD").returncode == 0
    if has_head:
        args = ["diff", "--cached", "--name-only", "--diff-filter=ACMRT", "-z", "HEAD"]
    else:
        args = ["ls-files", "--cached", "-z"]

    result = _git(repo, *args)
    if result.returncode != 0:
        message = result.stderr.decode("utf-8", "replace").strip()
        raise GitError(message or "git diff --cached failed")
    return [_decode_path(item) for item in result.stdout.split(b"\0") if item]


def _discard(stream, size: int) -> None:
    remaining = size
    while remaining > 0:
        chunk = stream.read(min(remaining, 65536))
        if not chunk:
            raise GitError("unexpected end of git cat-file output")
        remaining -= len(chunk)


def iter_staged_blobs(
    repo: Path,
    max_size: int | None = None,
) -> Iterator[StagedBlob]:
    entries = index_entries(repo)
    selected = [
        entries[path]
        for path in staged_paths(repo)
        if path in entries
        and entries[path].mode in {"100644", "100755"}
        and os.path.basename(path) != ".gitignore"
    ]
    if not selected:
        return

    process = subprocess.Popen(
        ["git", "-C", os.fspath(repo), "cat-file", "--batch"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert process.stdin is not None
    assert process.stdout is not None

    try:
        for entry in selected:
            process.stdin.write(entry.oid.encode("ascii") + b"\n")
            process.stdin.flush()

            header = process.stdout.readline()
            if not header:
                raise GitError("git cat-file --batch stopped unexpectedly")

            header = header.rstrip(b"\n")
            parts = header.split(b" ")
            if len(parts) != 3:
                continue

            _, object_type, size_raw = parts
            try:
                size = int(size_raw)
            except ValueError as exc:
                raise GitError(f"invalid object size for {entry.path}") from exc

            if max_size is not None and size > max_size:
                _discard(process.stdout, size)
                process.stdout.read(1)
                if object_type == b"blob":
                    yield StagedBlob(entry.path, entry.mode, entry.oid, b"", size)
                continue

            data = process.stdout.read(size)
            if len(data) != size:
                raise GitError(f"cannot read staged blob for {entry.path}")
            process.stdout.read(1)

            if object_type != b"blob":
                continue
            yield StagedBlob(entry.path, entry.mode, entry.oid, data, size)
    finally:
        try:
            process.stdin.close()
        except OSError:
            pass
        process.wait()


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="List or dump blobs that are staged in the Git index."
    )
    parser.add_argument("--repo", default=".", help="repository path")
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--list", action="store_true", help="print staged paths")
    mode.add_argument("--json", action="store_true", help="print staged entries as JSON")
    mode.add_argument("--dump", metavar="PATH", help="write the staged blob to stdout")
    args = parser.parse_args(argv)

    try:
        repo = repo_root(args.repo)
        entries = index_entries(repo)
        paths = staged_paths(repo)
    except GitError as exc:
        f_LOG_error(f"git_staged: {exc}")
        return 2

    if args.list:
        for path in paths:
            print(path)
        return 0

    if args.json:
        payload = [
            {"path": path, "mode": entries[path].mode, "oid": entries[path].oid}
            for path in paths
            if path in entries
        ]
        json.dump(payload, sys.stdout, ensure_ascii=False, indent=2)
        sys.stdout.write("\n")
        return 0

    target = args.dump
    assert target is not None
    if target not in entries:
        f_LOG_error(f"git_staged: path is not staged: {target}")
        return 2

    for blob in iter_staged_blobs(repo):
        if blob.path == target:
            sys.stdout.buffer.write(blob.data)
            return 0

    f_LOG_error(f"git_staged: cannot read staged blob: {target}")
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
