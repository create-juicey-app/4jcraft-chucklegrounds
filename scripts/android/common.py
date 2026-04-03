from __future__ import annotations

import os
import shutil
import subprocess
import tarfile
import urllib.request
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parents[2]
ANDROID_DIR = ROOT_DIR / "android"


def run(cmd: list[str], cwd: Path | None = None) -> None:
    subprocess.run(cmd, cwd=str(cwd) if cwd else None, check=True)


def read_local_property(key: str, file_path: Path) -> str:
    if not file_path.is_file():
        return ""
    prefix = f"{key}="
    for line in file_path.read_text(encoding="utf-8").splitlines():
        if line.startswith(prefix):
            return line[len(prefix) :]
    return ""


def expand_path(path: str) -> str:
    if not path:
        return ""
    return os.path.expanduser(path)


def normalize_ndk_path_case(ndk_dir: str) -> str:
    if ndk_dir and not Path(ndk_dir).is_dir():
        lowered = ndk_dir.replace("/NDK/", "/ndk/")
        if Path(lowered).is_dir():
            return lowered
    return ndk_dir


def find_latest_ndk_in_sdk(sdk_dir: str) -> str:
    if not sdk_dir:
        return ""
    ndk_root = Path(sdk_dir) / "ndk"
    if not ndk_root.is_dir():
        return ""
    versions = sorted([p for p in ndk_root.iterdir() if p.is_dir()])
    return str(versions[-1]) if versions else ""


def resolve_android_paths() -> tuple[str, str]:
    local_props = ANDROID_DIR / "local.properties"
    sdk_dir = os.environ.get("ANDROID_SDK_ROOT") or os.environ.get("ANDROID_HOME") or ""
    ndk_dir = os.environ.get("ANDROID_NDK_HOME") or ""

    if not sdk_dir:
        sdk_dir = read_local_property("sdk.dir", local_props)
    if not ndk_dir:
        ndk_dir = read_local_property("ndk.dir", local_props)

    sdk_dir = expand_path(sdk_dir)
    ndk_dir = normalize_ndk_path_case(expand_path(ndk_dir))

    if not ndk_dir:
        ndk_dir = find_latest_ndk_in_sdk(sdk_dir)

    return sdk_dir, ndk_dir


def write_local_properties(sdk_dir: str) -> None:
    if not sdk_dir:
        return

    local_props = ANDROID_DIR / "local.properties"
    existing_lines: list[str] = []
    if local_props.is_file():
        existing_lines = local_props.read_text(encoding="utf-8").splitlines()

    filtered = [line for line in existing_lines if not line.startswith("sdk.dir=") and not line.startswith("ndk.dir=")]
    content_lines = [f"sdk.dir={sdk_dir}"] + filtered
    local_props.write_text("\n".join(content_lines) + "\n", encoding="utf-8")


def download_file(url: str, output: Path) -> None:
    if output.is_file():
        return
    output.parent.mkdir(parents=True, exist_ok=True)
    with urllib.request.urlopen(url) as response, output.open("wb") as out_file:
        shutil.copyfileobj(response, out_file)


def has_local_sdl2() -> bool:
    sdl2_root = ANDROID_DIR / "third_party" / "SDL2"
    sdl2_lib = sdl2_root / "lib" / "arm64-v8a" / "libSDL2.so"
    sdl2_inc = sdl2_root / "include" / "SDL.h"
    sdl2_inc_alt = sdl2_root / "include" / "SDL2" / "SDL.h"
    return sdl2_lib.is_file() and (sdl2_inc.is_file() or sdl2_inc_alt.is_file())


def normalize_sdl2_headers() -> None:
    sdl2_root = ANDROID_DIR / "third_party" / "SDL2"
    include_dir = sdl2_root / "include"
    include_sdl2 = include_dir / "SDL2"
    include_sdl2.mkdir(parents=True, exist_ok=True)

    if (include_dir / "SDL.h").is_file() and not (include_sdl2 / "SDL.h").is_file():
        for header in include_dir.glob("*.h"):
            shutil.copy2(header, include_sdl2 / header.name)


def import_sdl2_from(src: str) -> bool:
    if not src:
        return False
    src_path = Path(src)
    if not src_path.is_dir():
        return False

    src_inc = None
    if (src_path / "include" / "SDL.h").is_file():
        src_inc = src_path / "include"
    elif (src_path / "include" / "SDL2" / "SDL.h").is_file():
        src_inc = src_path / "include" / "SDL2"

    src_lib = None
    if (src_path / "lib" / "arm64-v8a" / "libSDL2.so").is_file():
        src_lib = src_path / "lib" / "arm64-v8a" / "libSDL2.so"
    elif (src_path / "libSDL2.so").is_file():
        src_lib = src_path / "libSDL2.so"

    if src_inc is None or src_lib is None:
        return False

    sdl2_root = ANDROID_DIR / "third_party" / "SDL2"
    include_dir = sdl2_root / "include"
    include_sdl2 = include_dir / "SDL2"
    lib_out = sdl2_root / "lib" / "arm64-v8a" / "libSDL2.so"

    include_sdl2.mkdir(parents=True, exist_ok=True)
    lib_out.parent.mkdir(parents=True, exist_ok=True)

    for header in src_inc.glob("*.h"):
        shutil.copy2(header, include_dir / header.name)
        shutil.copy2(header, include_sdl2 / header.name)
    shutil.copy2(src_lib, lib_out)
    return True


def fetch_glm_if_missing() -> None:
    glm_hdr = ANDROID_DIR / "third_party" / "glm" / "glm" / "glm.hpp"
    if glm_hdr.is_file():
        return

    glm_version = os.environ.get("GLM_VERSION", "0.9.9.8")
    glm_url = os.environ.get(
        "GLM_DOWNLOAD_URL",
        f"https://github.com/g-truc/glm/archive/refs/tags/{glm_version}.tar.gz",
    )

    work_dir = ANDROID_DIR / ".cache" / "glm"
    archive = work_dir / f"glm-{glm_version}.tar.gz"
    src_dir = work_dir / f"glm-{glm_version}"
    out_dir = ANDROID_DIR / "third_party" / "glm"

    work_dir.mkdir(parents=True, exist_ok=True)
    out_dir.mkdir(parents=True, exist_ok=True)
    download_file(glm_url, archive)

    if src_dir.exists():
        shutil.rmtree(src_dir)
    with tarfile.open(archive, "r:gz") as tar:
        tar.extractall(work_dir)

    if not (src_dir / "glm" / "glm.hpp").is_file():
        raise RuntimeError("GLM headers not found in archive")

    target_glm = out_dir / "glm"
    if target_glm.exists():
        shutil.rmtree(target_glm)
    shutil.copytree(src_dir / "glm", target_glm)


def fetch_sdl2_if_missing(sdk_dir: str, ndk_dir: str) -> None:
    if has_local_sdl2():
        return

    if not ndk_dir:
        raise RuntimeError("ANDROID_NDK_HOME or ndk.dir is not set")

    toolchain = Path(ndk_dir) / "toolchains" / "llvm" / "prebuilt" / "linux-x86_64" / "bin"
    cc_bin = toolchain / "aarch64-linux-android21-clang"
    cxx_bin = toolchain / "aarch64-linux-android21-clang++"
    ar_bin = toolchain / "llvm-ar"
    strip_bin = toolchain / "llvm-strip"

    if not cc_bin.is_file() or not cxx_bin.is_file() or not ar_bin.is_file() or not strip_bin.is_file():
        raise RuntimeError(f"Android NDK toolchain not found in: {toolchain}")

    sdl2_version = os.environ.get("SDL2_VERSION", "2.30.7")
    sdl2_url = os.environ.get(
        "SDL2_DOWNLOAD_URL",
        f"https://github.com/libsdl-org/SDL/releases/download/release-{sdl2_version}/SDL2-{sdl2_version}.tar.gz",
    )

    work_dir = ANDROID_DIR / ".cache" / "sdl2"
    src_archive = work_dir / f"SDL2-{sdl2_version}.tar.gz"
    src_dir = work_dir / f"SDL2-{sdl2_version}"
    build_dir = work_dir / "build-arm64"
    cross_file = work_dir / "meson-cross.ini"

    sdl2_root = ANDROID_DIR / "third_party" / "SDL2"
    (sdl2_root / "include" / "SDL2").mkdir(parents=True, exist_ok=True)
    (sdl2_root / "lib" / "arm64-v8a").mkdir(parents=True, exist_ok=True)

    download_file(sdl2_url, src_archive)

    if src_dir.exists():
        shutil.rmtree(src_dir)
    with tarfile.open(src_archive, "r:gz") as tar:
        tar.extractall(work_dir)

    if build_dir.exists():
        shutil.rmtree(build_dir)

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
                f"ar = '{ar_bin}'",
                f"strip = '{strip_bin}'",
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

    run(
        [
            "meson",
            "setup",
            str(build_dir),
            str(src_dir),
            "--cross-file",
            str(cross_file),
            "--buildtype=release",
            "--default-library=shared",
        ]
    )
    run(["meson", "compile", "-C", str(build_dir), "--ninja-args=-j2"])

    candidates = [
        build_dir / "libSDL2.so",
        build_dir / "Release" / "libSDL2.so",
    ] + list(build_dir.rglob("libSDL2.so"))
    built_lib = next((p for p in candidates if p.is_file()), None)
    if built_lib is None:
        raise RuntimeError("Built SDL2 library not found")

    include_dir = sdl2_root / "include"
    include_sdl2 = include_dir / "SDL2"
    for header in (src_dir / "include").glob("*.h"):
        shutil.copy2(header, include_dir / header.name)
        shutil.copy2(header, include_sdl2 / header.name)
    shutil.copy2(built_lib, sdl2_root / "lib" / "arm64-v8a" / "libSDL2.so")

    normalize_sdl2_headers()
