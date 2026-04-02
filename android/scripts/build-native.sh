#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
ANDROID_DIR="$ROOT_DIR/android"
BUILD_DIR="$ROOT_DIR/build-android-arm64"
OUT_DIR="$ANDROID_DIR/app/src/main/jniLibs/arm64-v8a"

SDK_DIR=""
NDK_DIR=""

if [ -f "$ANDROID_DIR/local.properties" ]; then
  SDK_DIR="$(grep -m1 '^sdk.dir=' "$ANDROID_DIR/local.properties" | cut -d= -f2-)"
  NDK_DIR="$(grep -m1 '^ndk.dir=' "$ANDROID_DIR/local.properties" | cut -d= -f2-)"
fi

if [ -z "$SDK_DIR" ]; then
  SDK_DIR="${ANDROID_SDK_ROOT:-${ANDROID_HOME:-}}"
fi

if [ -z "$NDK_DIR" ]; then
  NDK_DIR="${ANDROID_NDK_HOME:-}"
fi

if [ -n "$NDK_DIR" ] && [ ! -d "$NDK_DIR" ]; then
  NDK_DIR_LOWER="${NDK_DIR/\/NDK\//\/ndk\/}"
  if [ -d "$NDK_DIR_LOWER" ]; then
    NDK_DIR="$NDK_DIR_LOWER"
  fi
fi

if [ -z "$NDK_DIR" ] && [ -n "$SDK_DIR" ] && [ -d "$SDK_DIR/ndk" ]; then
  NDK_DIR="$(find "$SDK_DIR/ndk" -mindepth 1 -maxdepth 1 -type d | sort | tail -n 1)"
fi

if [ -z "$NDK_DIR" ]; then
  echo "ANDROID_NDK_HOME or ndk.dir is not set, did you read the instructions?"
  exit 1
fi

TOOLCHAIN="$NDK_DIR/toolchains/llvm/prebuilt/linux-x86_64/bin"
CC_BIN="$TOOLCHAIN/aarch64-linux-android21-clang"
CXX_BIN="$TOOLCHAIN/aarch64-linux-android21-clang++"

if [ ! -x "$CC_BIN" ] || [ ! -x "$CXX_BIN" ]; then
  echo "Android NDK toolchain not found in: $TOOLCHAIN"
  echo "Resolved NDK_DIR: $NDK_DIR"
  exit 1
fi

CROSS_TEMPLATE="$ANDROID_DIR/meson-android-arm64.ini"
CROSS_FILE="$BUILD_DIR/meson-cross.ini"
SDL2_ROOT="$ANDROID_DIR/third_party/SDL2"
SDL2_LIB="$SDL2_ROOT/lib/arm64-v8a/libSDL2.so"
SDL2_INC="$SDL2_ROOT/include/SDL.h"
SDL2_INC_ALT="$SDL2_ROOT/include/SDL2/SDL.h"
GLM_HDR="$ANDROID_DIR/third_party/glm/glm/glm.hpp"

has_local_sdl2() {
  [ -f "$SDL2_LIB" ] && ( [ -f "$SDL2_INC" ] || [ -f "$SDL2_INC_ALT" ] )
}

normalize_sdl2_headers() {
  mkdir -p "$SDL2_ROOT/include/SDL2"
  if [ -f "$SDL2_ROOT/include/SDL.h" ] && [ ! -f "$SDL2_ROOT/include/SDL2/SDL.h" ]; then
    cp -f "$SDL2_ROOT/include/"*.h "$SDL2_ROOT/include/SDL2/" || true
  fi
}

import_sdl2_from() {
  SRC="$1"
  if [ -z "$SRC" ] || [ ! -d "$SRC" ]; then
    return 1
  fi

  SRC_INC=""
  if [ -f "$SRC/include/SDL.h" ]; then
    SRC_INC="$SRC/include"
  elif [ -f "$SRC/include/SDL2/SDL.h" ]; then
    SRC_INC="$SRC/include/SDL2"
  fi

  SRC_LIB=""
  if [ -f "$SRC/lib/arm64-v8a/libSDL2.so" ]; then
    SRC_LIB="$SRC/lib/arm64-v8a/libSDL2.so"
  elif [ -f "$SRC/libSDL2.so" ]; then
    SRC_LIB="$SRC/libSDL2.so"
  fi

  if [ -z "$SRC_INC" ] || [ -z "$SRC_LIB" ]; then
    return 1
  fi

  mkdir -p "$SDL2_ROOT/include"
  mkdir -p "$SDL2_ROOT/include/SDL2"
  mkdir -p "$SDL2_ROOT/lib/arm64-v8a"
  cp -f "$SRC_INC"/*.h "$SDL2_ROOT/include/"
  cp -f "$SRC_INC"/*.h "$SDL2_ROOT/include/SDL2/"
  cp -f "$SRC_LIB" "$SDL2_LIB"
  return 0
}

if ! has_local_sdl2; then
  if ! import_sdl2_from "${SDL2_DIR:-}"; then
    import_sdl2_from "${SDL2_ANDROID_ROOT:-}" || true
  fi
fi

if ! has_local_sdl2 && [ "${SDL2_AUTO_FETCH:-1}" != "0" ]; then
  bash "$ANDROID_DIR/scripts/fetch-sdl2.sh" || true
fi

normalize_sdl2_headers

if [ ! -f "$GLM_HDR" ] && [ "${GLM_AUTO_FETCH:-1}" != "0" ]; then
  bash "$ANDROID_DIR/scripts/fetch-glm.sh" || true
fi

if ! has_local_sdl2; then
  echo "SDL2 Android prebuilts not found"
  echo "Make sure internet is available."
  exit 1
fi

if [ ! -f "$GLM_HDR" ]; then
  echo "GLM headers not found"
  echo "Make sure internet is available."
  exit 1
fi

mkdir -p "$BUILD_DIR"
mkdir -p "$OUT_DIR"

# genuinely couldn't find a better way to do this.
sed "s|@NDK_TOOLCHAIN@|$TOOLCHAIN|g" "$CROSS_TEMPLATE" > "$CROSS_FILE"

if [ -n "$SDK_DIR" ]; then
  printf 'sdk.dir=%s\n' "$SDK_DIR" > "$ANDROID_DIR/local.properties"
  printf 'ndk.dir=%s\n' "$NDK_DIR" >> "$ANDROID_DIR/local.properties"
fi

meson setup "$BUILD_DIR" "$ROOT_DIR" --reconfigure --cross-file "$CROSS_FILE" -Drenderer=gles
meson compile -C "$BUILD_DIR" main

LIB_PATH="$(find "$BUILD_DIR" -name 'libmain.so' | head -n 1)"
if [ -z "$LIB_PATH" ]; then
  echo "libmain.so not found"
  exit 1
fi

cp "$LIB_PATH" "$OUT_DIR/libmain.so"
