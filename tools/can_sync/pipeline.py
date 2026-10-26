"""Prepare generated files, then publish one commit and patch tag atomically."""

import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

from excel_to_dbc import convert


ROOT = Path(__file__).resolve().parents[2]
GENERATED = {
    "MAIN_DBC.dbc": "MAIN_DBC.dbc",
    "can_signal_defs.h": "drivers/can/can_signal_defs.h",
    "can_board_defs.h": "drivers/can/can_board_defs.h",
}
PROVENANCE = "can-database-source.json"


def git(root, *args):
    return subprocess.check_output(["git", "-C", str(root), *args], text=True, encoding="utf-8").strip()


def output(key, value):
    print(f"{key}={value}")
    if os.environ.get("GITHUB_OUTPUT"):
        with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as stream:
            stream.write(f"{key}={value}\n")


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def next_tag(tags):
    versions = [tuple(map(int, match.groups())) for tag in tags
                if (match := re.fullmatch(r"v?(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)", tag))]
    if not versions:
        raise ValueError("No stable semantic version tag exists; create an initial vX.Y.Z tag first")
    major, minor, patch = max(versions)
    return f"v{major}.{minor}.{patch + 1}"


def prepare(workbook, artifacts, root=ROOT):
    text = convert(workbook)  # No repository outputs are touched on validation failure.
    artifacts.mkdir(parents=True, exist_ok=True)
    dbc = artifacts / "MAIN_DBC.dbc"
    dbc.write_text(text, encoding="utf-8", newline="\n")
    subprocess.run([sys.executable, str(root / "dbc_to_c_builder.py"), "--dbc", str(dbc),
                    "--signal-header", str(artifacts / "can_signal_defs.h"),
                    "--board-header", str(artifacts / "can_board_defs.h")], check=True)
    scripts = [root / "dbc_to_c_builder.py", *sorted((root / "tools/can_sync").glob("*.py")),
               root / "tools/can_sync/requirements.txt"]
    manifest = {"workbook_sha256": sha256(workbook),
                "generators": {path.relative_to(root).as_posix(): sha256(path) for path in scripts},
                "outputs": {target: sha256(artifacts / name) for name, target in GENERATED.items()}}
    (artifacts / PROVENANCE).write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8", newline="\n")


def stage(artifacts, root=ROOT):
    changed = any(not (root / target).exists() or
                  (root / target).read_bytes() != (artifacts / name).read_bytes()
                  for name, target in GENERATED.items())
    if changed:
        for name, target in {**GENERATED, PROVENANCE: PROVENANCE}.items():
            (root / target).parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(artifacts / name, root / target)
    output("changed", str(changed).lower())
    return changed


def publish(root=ROOT):
    # This command is called only AFTER generator and C tests have succeeded.
    if git(root, "branch", "--show-current") != "main":
        raise ValueError("Publishing requires the main branch")
    if git(root, "diff", "--cached", "--name-only"):
        raise ValueError("Unexpected staged files; refusing to publish unrelated changes")
    allowed = set(GENERATED.values()) | {PROVENANCE}
    modified = set(git(root, "diff", "--name-only", "HEAD").splitlines())
    untracked = set(git(root, "ls-files", "--others", "--exclude-standard").splitlines())
    if (modified | untracked) - allowed:
        raise ValueError("Unexpected repository changes; refusing to publish unrelated files")
    if not (modified | untracked) & set(GENERATED.values()):
        raise ValueError("No generated changes to publish")
    git(root, "fetch", "origin", "main", "--tags")
    if git(root, "rev-parse", "HEAD") != git(root, "rev-parse", "origin/main"):
        raise ValueError("main advanced during generation. Run the workflow again; nothing was pushed")
    tag = next_tag(git(root, "tag", "--list").splitlines())
    git(root, "add", "--", *sorted(allowed))
    git(root, "-c", "user.name=github-actions[bot]", "-c",
        "user.email=41898282+github-actions[bot]@users.noreply.github.com",
        "commit", "-m", f"chore(can): sync SharePoint database ({tag})")
    git(root, "tag", tag)
    # A competing main update/tag or a branch rule rejects BOTH refs; never force.
    git(root, "push", "--atomic", "origin", "HEAD:refs/heads/main", f"refs/tags/{tag}")
    output("tag", tag)


def release_tag(root=ROOT):
    # Recover release dispatch/upload failures without a second commit/tag.
    commit = git(root, "log", "-1", "--format=%H", "--", PROVENANCE)
    tags = git(root, "tag", "--points-at", commit).splitlines() if commit else []
    tags = [tag for tag in tags if re.fullmatch(r"v[0-9]+\.[0-9]+\.[0-9]+", tag)]
    output("tag", max(tags, key=lambda tag: tuple(map(int, tag[1:].split(".")))) if tags else "")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    prepare_parser = sub.add_parser("prepare")
    prepare_parser.add_argument("--workbook", type=Path, required=True)
    prepare_parser.add_argument("--artifacts", type=Path, required=True)
    stage_parser = sub.add_parser("stage")
    stage_parser.add_argument("--artifacts", type=Path, required=True)
    sub.add_parser("publish")
    sub.add_parser("release-tag")
    args = parser.parse_args()
    try:
        if args.command == "prepare":
            prepare(args.workbook.resolve(), args.artifacts.resolve())
        elif args.command == "stage":
            stage(args.artifacts.resolve())
        elif args.command == "publish":
            publish()
        else:
            release_tag()
    except (ValueError, OSError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"CAN sync failed: {error}\n")


if __name__ == "__main__":
    main()
