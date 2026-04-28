#!/usr/bin/env bash
# Compile sans avoir cmake dans le PATH (utilise .tools/ comme setup-macos.sh).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
CMAKE_VER="${CMAKE_VER:-3.31.6}"
PORTABLE="$ROOT/.tools/cmake-${CMAKE_VER}-macos-universal/CMake.app/Contents/bin/cmake"
if [[ -x "$PORTABLE" ]]; then
  CMAKE="$PORTABLE"
elif command -v cmake >/dev/null 2>&1; then
  CMAKE="cmake"
else
  echo "CMake introuvable. Lance une fois : ./scripts/setup-macos.sh"
  exit 1
fi

mkdir -p "$ROOT/build"
cd "$ROOT/build"
if [[ ! -f Makefile && ! -f build.ninja ]]; then
  "$CMAKE" .. -DCMAKE_BUILD_TYPE=Release
fi
"$CMAKE" --build . -j
echo "OK → $ROOT/build/MeshViewer"
