#!/usr/bin/env python3
from __future__ import annotations

import os
import shutil
import subprocess
from pathlib import Path

from common import (
    ANDROID_DIR,
    ROOT_DIR,
    fetch_glm_if_missing,
    fetch_sdl2_if_missing,
    has_local_sdl2,
    import_sdl2_from,
    normalize_sdl2_headers,
    resolve_android_paths,
    write_local_properties,
)


def run(cmd: list[str], cwd: Path | None = None) -> None:
    subprocess.run(cmd, cwd=str(cwd) if cwd else None, check=True)


def main() -> int:
    sdk_dir, ndk_dir = resolve_android_paths()

    build_dir = ROOT_DIR / "build-android-arm64"
    out_dir = ANDROID_DIR / "app" / "src" / "main" / "jniLibs" / "arm64-v8a"

    if not ndk_dir:
        print("ANDROID_NDK_HOME or ndk.dir is not set")
        return 1

    toolchain = Path(ndk_dir) / "toolchains" / "llvm" / "prebuilt" / "linux-x86_64" / "bin"
    cc_bin = toolchain / "aarch64-linux-android21-clang"
    cxx_bin = toolchain / "aarch64-linux-android21-clang++"

    if not cc_bin.is_file() or not cxx_bin.is_file():
        print(f"Android NDK toolchain not found in: {toolchain}")
        print(f"Resolved NDK_DIR: {ndk_dir}")
        return 1

    glm_hdr = ANDROID_DIR / "third_party" / "glm" / "glm" / "glm.hpp"

    if not has_local_sdl2():
        if not import_sdl2_from(os.environ.get("SDL2_DIR", "")):
            import_sdl2_from(os.environ.get("SDL2_ANDROID_ROOT", ""))

    if not has_local_sdl2() and os.environ.get("SDL2_AUTO_FETCH", "1") != "0":
        try:
            fetch_sdl2_if_missing(sdk_dir, ndk_dir)
        except Exception:
            pass

    normalize_sdl2_headers()

    if not glm_hdr.is_file() and os.environ.get("GLM_AUTO_FETCH", "1") != "0":
        try:
            fetch_glm_if_missing()
        except Exception:
            pass

    if not has_local_sdl2():
        print("SDL2 Android prebuilts not found")
        print("Make sure internet is available.")
        return 1

    if not glm_hdr.is_file():
        print("GLM headers not found")
        print("Make sure internet is available.")
        return 1

    build_dir.mkdir(parents=True, exist_ok=True)
    out_dir.mkdir(parents=True, exist_ok=True)

    # Keep cross-file generation local to this script to avoid extra templates.
    cross_file = build_dir / "meson-cross.ini"
    cross_file.write_text(
        "\n".join(
            [
                "[host_machine]",
                "system = 'android'",
                "cpu_family = 'aarch64'",
                "cpu = 'armv8'",
                "endian = 'little'",
                "",
                "[binaries]",
                f"ar = '{toolchain / 'llvm-ar'}'",
                f"strip = '{toolchain / 'llvm-strip'}'",
                f"c = '{cc_bin}'",
                f"cpp = '{cxx_bin}'",
                "",
                "[properties]",
                "needs_exe_wrapper = true",
                "",
                "[built-in options]",
                "c_args = ['-fPIC']",
                "cpp_args = ['-fPIC']",
                "c_link_args = ['-llog']",
                "cpp_link_args = ['-llog']",

                "",
            ]
        ),
        encoding="utf-8",
    )

    write_local_properties(sdk_dir)

    run(
        [
            "meson",
            "setup",
            str(build_dir),
            str(ROOT_DIR),
            "--reconfigure",
            "--cross-file",
            str(cross_file),
            "-Drenderer=gles",
            "-Dui_backend=java",
        ]
    )
    run(["meson", "compile", "-C", str(build_dir), "main", "--ninja-args=-j2"])

    lib_path = next(iter(build_dir.rglob("libmain.so")), None)
    if lib_path is None:
        print("libmain.so not found")
        return 1

    shutil.copy2(lib_path, out_dir / "libmain.so")

    sdl2_lib_path = ANDROID_DIR / "third_party" / "SDL2" / "lib" / "arm64-v8a" / "libSDL2.so"
    if sdl2_lib_path.is_file():
        shutil.copy2(sdl2_lib_path, out_dir / "libSDL2.so")
    else:
        print("Warning: libSDL2.so was not found; skipping copy")

    libcxx_path = (
        Path(ndk_dir)
        /"toolchains"
        /"llvm"
        /"prebuilt"
        /"linux-x86_64"
        /"sysroot"
        /"usr"
        /"lib"
        /"aarch64-linux-android"
        /"libc++_shared.so"
    )

    if libcxx_path.is_file():
        shutil.copy2(libcxx_path, out_dir / "libc++_shared.so")
    else:
        print("Warning: libc++_shared.so was not found; skipping copy")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
