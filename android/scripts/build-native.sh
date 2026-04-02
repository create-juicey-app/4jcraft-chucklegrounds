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

if [ -z "$NDK_DIR" ]; then
  echo "ANDROID_NDK_HOME or ndk.dir is not set"
  exit 1
fi

TOOLCHAIN="$NDK_DIR/toolchains/llvm/prebuilt/linux-x86_64/bin"
CROSS_TEMPLATE="$ANDROID_DIR/meson-android-arm64.ini"
CROSS_FILE="$BUILD_DIR/meson-cross.ini"

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
