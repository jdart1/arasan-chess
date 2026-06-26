#!/bin/bash
#
# update_network.sh - Update the default NNUE network file name in all build files.
#
# Usage:   tools/update_network.sh <new-network-file>
# Example: tools/update_network.sh arasanv8-20260621.nnue
#
# Updates the network file name in:
#   src/Makefile        (NETWORK ?= ...)
#   src/Makefile.win    (NETWORK = ...)
#   src/CMakeLists.txt  (set(NETWORK ... CACHE STRING ...))
#   prj/arasan.props    (<NETWORK>...</NETWORK>)
#   src/release.bat     (7z ... network\...)
#   gui/install/arasan.iss   (Inno Setup Source: line)
#   gui/install/arasan64.iss (Inno Setup Source: line)
#
# The current network file name may differ between files, so each file is
# rewritten by matching the *.nnue token in its known context rather than
# substituting a single old value globally.
#

set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <new-network-file>" >&2
    echo "Example: $0 arasanv8-20260621.nnue" >&2
    exit 1
fi

NEW_NETWORK="$1"

# Validate: a plausible network file name ending in .nnue with no path.
if [[ "$NEW_NETWORK" == */* ]]; then
    echo "Error: network file '$NEW_NETWORK' must be a bare file name, not a path" >&2
    exit 1
fi
if [[ "$NEW_NETWORK" != *.nnue ]]; then
    echo "Error: network file '$NEW_NETWORK' must end in .nnue" >&2
    exit 1
fi

# Locate the repository root (parent of the directory containing this script).
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$SCRIPT_DIR/.." && pwd)

echo "Updating Arasan network file to: $NEW_NETWORK"
echo "  Repository root = $ROOT"
echo

# A *.nnue file-name token (no path separators).
NNUE_RE='[A-Za-z0-9._-]*\.nnue'

# in_place_sed <file> <sed-expression> [<sed-expression> ...]
# Edits the file in place after verifying it exists.
in_place_sed() {
    local file="$1"
    shift
    if [ ! -f "$file" ]; then
        echo "  WARNING: $file not found, skipping" >&2
        return
    fi
    local args=()
    for expr in "$@"; do
        args+=(-e "$expr")
    done
    sed -i.bak "${args[@]}" "$file"
    rm -f "${file}.bak"
    echo "  updated $file"
}

# --- src/Makefile (GNU make, Linux/Mac) -------------------------------------
in_place_sed "$ROOT/src/Makefile" \
    "s|^\(NETWORK[[:space:]]*?=[[:space:]]*\)${NNUE_RE}|\1${NEW_NETWORK}|"

# --- src/Makefile.win (nmake, MSVC) -----------------------------------------
in_place_sed "$ROOT/src/Makefile.win" \
    "s|^\(NETWORK[[:space:]]*=[[:space:]]*\)${NNUE_RE}|\1${NEW_NETWORK}|"

# --- src/CMakeLists.txt -----------------------------------------------------
in_place_sed "$ROOT/src/CMakeLists.txt" \
    "s|\(set(NETWORK[[:space:]]*\)${NNUE_RE}|\1${NEW_NETWORK}|"

# --- Visual Studio property sheet (prj) -------------------------------------
in_place_sed "$ROOT/prj/arasan.props" \
    "s|<NETWORK>${NNUE_RE}</NETWORK>|<NETWORK>${NEW_NETWORK}</NETWORK>|"

# --- Windows release batch script -------------------------------------------
in_place_sed "$ROOT/src/release.bat" \
    "s|\(network\\\\\)${NNUE_RE}|\1${NEW_NETWORK}|"

# --- Inno Setup installer scripts -------------------------------------------
for iss in arasan.iss arasan64.iss; do
    in_place_sed "$ROOT/gui/install/$iss" \
        "s|\(network\\\\\)${NNUE_RE}|\1${NEW_NETWORK}|"
done

echo
echo "Done. Review the changes with 'git diff' before committing."
