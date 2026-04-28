#!/usr/bin/env bash
# Installe CMake portable dans .tools/ (sans Homebrew ni Xcode complet),
# puis compile MeshViewer. Prérequis : Xcode Command Line Tools (clang + SDK OpenGL).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TOOLS="$ROOT/.tools"
CMAKE_VER="${CMAKE_VER:-3.31.6}"
CMAKE_DIR="$TOOLS/cmake-${CMAKE_VER}-macos-universal"
CMAKE_BIN="$CMAKE_DIR/CMake.app/Contents/bin/cmake"

need_clt() {
  if ! xcode-select -p &>/dev/null; then
    echo "Installe les Xcode Command Line Tools :"
    echo "  xcode-select --install"
    exit 1
  fi
}

install_cmake_portable() {
  local tmp url tgz
  tmp="$(mktemp -d)"
  url="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VER}/cmake-${CMAKE_VER}-macos-universal.tar.gz"
  tgz="$tmp/cmake.tar.gz"
  echo "Téléchargement de CMake ${CMAKE_VER}…"
  curl -fsSL -o "$tgz" "$url"
  ( cd "$tmp" && tar xzf "$tgz" )
  mkdir -p "$TOOLS"
  rm -rf "$CMAKE_DIR"
  mv "$tmp/cmake-${CMAKE_VER}-macos-universal" "$CMAKE_DIR"
  rm -rf "$tmp"
}

need_clt

if [[ ! -x "$CMAKE_BIN" ]]; then
  install_cmake_portable
fi

"$CMAKE_BIN" --version
mkdir -p "$ROOT/build"
cd "$ROOT/build"
"$CMAKE_BIN" .. -DCMAKE_BUILD_TYPE=Release
"$CMAKE_BIN" --build . -j

echo
echo "OK — lance depuis la racine du dépôt (les .obj sont des chemins relatifs) :"
echo "  cd \"$ROOT\" && ./build/MeshViewer"
