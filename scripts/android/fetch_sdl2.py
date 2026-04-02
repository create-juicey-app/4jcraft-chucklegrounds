#!/usr/bin/env python3
from __future__ import annotations

from common import fetch_sdl2_if_missing, resolve_android_paths


if __name__ == "__main__":
    sdk_dir, ndk_dir = resolve_android_paths()
    fetch_sdl2_if_missing(sdk_dir, ndk_dir)
