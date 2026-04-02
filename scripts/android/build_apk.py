#!/usr/bin/env python3
from __future__ import annotations

import subprocess
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parents[2]
ANDROID_DIR = ROOT_DIR / "android"


def main() -> int:
    gradlew = ANDROID_DIR / "gradlew"
    if gradlew.exists() and gradlew.is_file():
        cmd = ["./gradlew", "assembleDebug"]
    else:
        cmd = ["gradle", "assembleDebug"]

    subprocess.run(cmd, cwd=str(ANDROID_DIR), check=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
