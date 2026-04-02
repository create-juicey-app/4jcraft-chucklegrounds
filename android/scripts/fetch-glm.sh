#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
ANDROID_DIR="$ROOT_DIR/android"
GLM_VERSION="${GLM_VERSION:-0.9.9.8}"
GLM_URL="${GLM_DOWNLOAD_URL:-https://github.com/g-truc/glm/archive/refs/tags/${GLM_VERSION}.tar.gz}"
WORK_DIR="$ANDROID_DIR/.cache/glm"
ARCHIVE="$WORK_DIR/glm-${GLM_VERSION}.tar.gz"
SRC_DIR="$WORK_DIR/glm-${GLM_VERSION}"
OUT_DIR="$ANDROID_DIR/third_party/glm"

mkdir -p "$WORK_DIR"
mkdir -p "$OUT_DIR"

if [ ! -f "$ARCHIVE" ]; then
  if command -v curl >/dev/null 2>&1; then
    curl -L "$GLM_URL" -o "$ARCHIVE"
  elif command -v wget >/dev/null 2>&1; then
    wget "$GLM_URL" -O "$ARCHIVE"
  else
    echo "Neither curl nor wget is available, how??"
    exit 1
  fi
fi

rm -rf "$SRC_DIR"
tar -xzf "$ARCHIVE" -C "$WORK_DIR"

if [ ! -f "$SRC_DIR/glm/glm.hpp" ]; then
  echo "GLM headers not found in archive ):"
  exit 1
fi

rm -rf "$OUT_DIR/glm"
cp -R "$SRC_DIR/glm" "$OUT_DIR/glm"
