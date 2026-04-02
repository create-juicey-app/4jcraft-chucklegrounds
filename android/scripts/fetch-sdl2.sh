#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
ANDROID_DIR="$ROOT_DIR/android"
SDL2_ROOT="$ANDROID_DIR/third_party/SDL2"
SDL2_VERSION="${SDL2_VERSION:-2.30.7}"
SDL2_URL="${SDL2_DOWNLOAD_URL:-https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-${SDL2_VERSION}.tar.gz}"
NDK_DIR="${ANDROID_NDK_HOME:-}"
SDK_DIR="${ANDROID_SDK_ROOT:-${ANDROID_HOME:-}}"

if [ -f "$ANDROID_DIR/local.properties" ]; then
  if [ -z "$SDK_DIR" ]; then
    SDK_DIR="$(grep -m1 '^sdk.dir=' "$ANDROID_DIR/local.properties" | cut -d= -f2-)"
  fi
  if [ -z "$NDK_DIR" ]; then
    NDK_DIR="$(grep -m1 '^ndk.dir=' "$ANDROID_DIR/local.properties" | cut -d= -f2-)"
  fi
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
  echo "ANDROID_NDK_HOME or ndk.dir is not set, Read instructions first!"
  exit 1
fi

TOOLCHAIN_FILE="$NDK_DIR/build/cmake/android.toolchain.cmake"
if [ ! -f "$TOOLCHAIN_FILE" ]; then
  echo "Android CMake toolchain not found: $TOOLCHAIN_FILE"
  echo "Make sure the NDK is properly installed and the path is correct."
  exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
  echo "cmake not found"
  exit 1
fi

WORK_DIR="$ANDROID_DIR/.cache/sdl2"
SRC_ARCHIVE="$WORK_DIR/SDL2-${SDL2_VERSION}.tar.gz"
SRC_DIR="$WORK_DIR/SDL2-${SDL2_VERSION}"
BUILD_DIR="$WORK_DIR/build-arm64"

mkdir -p "$WORK_DIR"
mkdir -p "$SDL2_ROOT/include"
mkdir -p "$SDL2_ROOT/include/SDL2"
mkdir -p "$SDL2_ROOT/lib/arm64-v8a"

if [ ! -f "$SRC_ARCHIVE" ]; then
  if command -v curl >/dev/null 2>&1; then
    curl -L "$SDL2_URL" -o "$SRC_ARCHIVE"
  elif command -v wget >/dev/null 2>&1; then
    wget "$SDL2_URL" -O "$SRC_ARCHIVE"
  else
    echo "Neither curl nor wget is available, how??"
    exit 1
  fi
fi

rm -rf "$SRC_DIR"
tar -xzf "$SRC_ARCHIVE" -C "$WORK_DIR"

rm -rf "$BUILD_DIR"
cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-21 \
  -DSDL_SHARED=ON \
  -DSDL_STATIC=OFF \
  -DSDL_TEST=OFF

cmake --build "$BUILD_DIR" --config Release -j

SDL_LIB_BUILT=""
if [ -f "$BUILD_DIR/libSDL2.so" ]; then
  SDL_LIB_BUILT="$BUILD_DIR/libSDL2.so"
elif [ -f "$BUILD_DIR/Release/libSDL2.so" ]; then
  SDL_LIB_BUILT="$BUILD_DIR/Release/libSDL2.so"
else
  SDL_LIB_BUILT="$(find "$BUILD_DIR" -name 'libSDL2.so' | head -n 1)"
fi

if [ -z "$SDL_LIB_BUILT" ] || [ ! -f "$SDL_LIB_BUILT" ]; then
  echo "Built SDL2 library not found ):"
  exit 1
fi

cp -f "$SRC_DIR/include/"*.h "$SDL2_ROOT/include/"
cp -f "$SRC_DIR/include/"*.h "$SDL2_ROOT/include/SDL2/"
cp -f "$SDL_LIB_BUILT" "$SDL2_ROOT/lib/arm64-v8a/libSDL2.so"
