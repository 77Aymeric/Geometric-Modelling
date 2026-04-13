#!/bin/zsh
# Enable macOS Game Mode (allocates maximum CPU/GPU resources, silences background tasks)
GAMECTL="/Applications/Xcode.app/Contents/Developer/usr/bin/gamepolicyctl"

if [ -x "$GAMECTL" ]; then
    "$GAMECTL" game-mode set on
    osascript -e 'display notification "Game Mode activé ✅" with title "MeshViewer"'
else
    osascript -e 'display notification "gamepolicyctl introuvable — Game Mode désactivé" with title "MeshViewer"'
fi

# Find the binary next to this script or in the build folder
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BINARY="$SCRIPT_DIR/build/MeshViewer"
[ ! -f "$BINARY" ] && BINARY="$SCRIPT_DIR/MeshViewer"

exec "$BINARY" "$@"
