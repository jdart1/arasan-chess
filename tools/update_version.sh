#!/bin/bash
#
# update_version.sh - Update the Arasan version number in all build files.
#
# Usage:   tools/update_version.sh <new-version>
# Example: tools/update_version.sh 25.4
#          tools/update_version.sh 25.3.1
#
# Updates the version in:
#   src/Makefile, src/Makefile.win, src/Makefile.icl
#   src/CMakeLists.txt
#   prj/arasan.props, gui/arasan.props
#   gui/install/arasan.iss   (Inno Setup installer script)
#   gui/install/arasan64.iss (Inno Setup installer script, 64-bit)
#   gui/gui.rc               (Windows VERSIONINFO resource)
#
# The Win32 VERSIONINFO block in gui.rc requires FILEVERSION and PRODUCTVERSION
# to be expressed as four comma-separated 16-bit integers (e.g. 25,3,1,0). The
# string-form FileVersion / ProductVersion fields use the dotted form (25.3.1).
# LINK_VERSION (passed to link.exe /version:) is limited to MAJOR.MINOR.
#

set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <new-version>" >&2
    echo "Example: $0 25.4" >&2
    echo "         $0 25.3.1" >&2
    exit 1
fi

NEW_VERSION="$1"

# Validate: dotted numeric version with 2, 3 or 4 components.
if ! [[ "$NEW_VERSION" =~ ^[0-9]+\.[0-9]+(\.[0-9]+){0,2}$ ]]; then
    echo "Error: version '$NEW_VERSION' must be of the form MAJOR.MINOR[.PATCH[.BUILD]]" >&2
    exit 1
fi

# Locate the repository root (parent of the directory containing this script).
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$SCRIPT_DIR/.." && pwd)

# Split version into components and derive helpers.
IFS='.' read -r V_MAJOR V_MINOR V_PATCH V_BUILD <<< "$NEW_VERSION"
: "${V_PATCH:=0}"
: "${V_BUILD:=0}"

# LINK_VERSION (link.exe /version:) accepts only MAJOR.MINOR, regardless of
# whether NEW_VERSION has 2, 3 or 4 components.
LINK_VERSION="${V_MAJOR}.${V_MINOR}"

# Comma-separated 4-tuple for VERSIONINFO FILEVERSION / PRODUCTVERSION.
RC_VERSION="${V_MAJOR},${V_MINOR},${V_PATCH},${V_BUILD}"

echo "Updating Arasan version to: $NEW_VERSION"
echo "  LINK_VERSION       = $LINK_VERSION"
echo "  RC FILEVERSION     = $RC_VERSION"
echo "  Repository root    = $ROOT"
echo

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
    "s/^VERSION[[:space:]]*=.*/VERSION = ${NEW_VERSION}/"

# --- src/Makefile.win (nmake, MSVC) and src/Makefile.icl (nmake, Intel C++) -
# Both define VERSION and LINK_VERSION on dedicated lines. Rewrite each
# directly so that LINK_VERSION keeps its MAJOR.MINOR form even when the full
# version has 3 or 4 components.
for mk in Makefile.win Makefile.icl; do
    in_place_sed "$ROOT/src/$mk" \
        "s/^VERSION=.*/VERSION=${NEW_VERSION}/" \
        "s/^LINK_VERSION=.*/LINK_VERSION=${LINK_VERSION}/"
done

# --- src/CMakeLists.txt -----------------------------------------------------
in_place_sed "$ROOT/src/CMakeLists.txt" \
    "s/add_compile_definitions(ARASAN_VERSION=[^)]*)/add_compile_definitions(ARASAN_VERSION=${NEW_VERSION})/"

# --- Visual Studio property sheets ------------------------------------------
# Both engine (prj) and GUI (gui) use property sheets to set ARASAN_VERSION.
in_place_sed "$ROOT/prj/arasan.props" \
    "s|<ARASAN_VERSION>[^<]*</ARASAN_VERSION>|<ARASAN_VERSION>${NEW_VERSION}</ARASAN_VERSION>|" \
    "s|<ARASAN_LINK_VERSION>[^<]*</ARASAN_LINK_VERSION>|<ARASAN_LINK_VERSION>${LINK_VERSION}</ARASAN_LINK_VERSION>|"

in_place_sed "$ROOT/gui/arasan.props" \
    "s|<ARASAN_VERSION>[^<]*</ARASAN_VERSION>|<ARASAN_VERSION>${NEW_VERSION}</ARASAN_VERSION>|"

# --- Inno Setup installer script --------------------------------------------
# Every occurrence of the version in arasan.iss is the same dotted form
# (AppVerName, DefaultDirName, DefaultGroupName, OutputBaseFilename), so find
# the current version once via AppVerName and globally substitute.
ISS="$ROOT/gui/install/arasan.iss"
if [ -f "$ISS" ]; then
    OLD_ISS_VERSION=$(sed -n 's|^AppVerName=Arasan \([0-9][0-9.]*\).*|\1|p' "$ISS" | head -n1)
    if [ -z "$OLD_ISS_VERSION" ]; then
        echo "  WARNING: could not find current version in $ISS" >&2
    else
        OLD_ISS_RE=$(printf '%s' "$OLD_ISS_VERSION" | sed 's/\./\\./g')
        in_place_sed "$ISS" "s|${OLD_ISS_RE}|${NEW_VERSION}|g"
    fi
else
    echo "  WARNING: $ISS not found, skipping" >&2
fi

ISS64="$ROOT/gui/install/arasan64.iss"
if [ -f "$ISS64" ]; then
    OLD_ISS64_VERSION=$(sed -n 's|^AppVerName=Arasan \([0-9][0-9.]*\).*|\1|p' "$ISS64" | head -n1)
    if [ -z "$OLD_ISS64_VERSION" ]; then
        echo "  WARNING: could not find current version in $ISS64" >&2
    else
        OLD_ISS64_RE=$(printf '%s' "$OLD_ISS64_VERSION" | sed 's/\./\\./g')
        in_place_sed "$ISS64" "s|${OLD_ISS64_RE}|${NEW_VERSION}|g"
    fi
else
    echo "  WARNING: $ISS64 not found, skipping" >&2
fi

# --- GUI Win32 resource script (VERSIONINFO block + product strings) -------
# FILEVERSION / PRODUCTVERSION use a comma-separated 4-tuple; the FileVersion
# / ProductVersion VALUE strings and the product name strings elsewhere in
# the file (IDD_ABOUT_DIALOG caption, IDR_MAINFRAME) use the dotted form.
# Extract the current values from the file and globally substitute both.
RC="$ROOT/gui/gui.rc"
if [ -f "$RC" ]; then
    OLD_RC_TUPLE=$(sed -n 's|^[[:space:]]*FILEVERSION[[:space:]]*\([0-9,]*\).*|\1|p' "$RC" | head -n1)
    OLD_RC_DOTTED=$(sed -n 's|.*VALUE "FileVersion", "\([^"]*\)".*|\1|p' "$RC" | head -n1)
    if [ -z "$OLD_RC_TUPLE" ] || [ -z "$OLD_RC_DOTTED" ]; then
        echo "  WARNING: could not find current version in $RC" >&2
    else
        OLD_RC_DOTTED_RE=$(printf '%s' "$OLD_RC_DOTTED" | sed 's/\./\\./g')
        in_place_sed "$RC" \
            "s|${OLD_RC_TUPLE}|${RC_VERSION}|g" \
            "s|${OLD_RC_DOTTED_RE}|${NEW_VERSION}|g"
    fi
else
    echo "  WARNING: $RC not found, skipping" >&2
fi

echo
echo "Done. Review the changes with 'git diff' before committing."
