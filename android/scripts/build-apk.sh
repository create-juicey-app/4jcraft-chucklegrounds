#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT_DIR/android"

if [ -x "./gradlew" ]; then
  ./gradlew assembleDebug
else
  gradle assembleDebug
fi
