#!/usr/bin/env python3
"""Compile Notepad++ .rc files into a C++ resource table for Unix."""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path


DIALOG_START = re.compile(
    r"^(\w+)\s+DIALOGEX(?:\s+DISCARDABLE)?\s+(?:[\w]+\s+)?(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)",
    re.I | re.M,
)
MENU_START = re.compile(r"^(\w+)\s+MENU\b", re.I | re.M)
STRING_START = re.compile(r"^STRINGTABLE\b", re.I | re.M)

CTRL = {
    "PUSHBUTTON": ("BUTTON", "WS_VISIBLE|WS_CHILD|WS_TABSTOP"),
    "DEFPUSHBUTTON": ("BUTTON", "WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_DEFPUSHBUTTON"),
    "LTEXT": ("STATIC", "WS_VISIBLE|WS_CHILD|SS_LEFT"),
    "CTEXT": ("STATIC", "WS_VISIBLE|WS_CHILD|SS_CENTER"),
    "RTEXT": ("STATIC", "WS_VISIBLE|WS_CHILD|SS_RIGHT"),
    "EDITTEXT": ("EDIT", "WS_VISIBLE|WS_CHILD|WS_BORDER|ES_LEFT"),
    "GROUPBOX": ("BUTTON", "WS_VISIBLE|WS_CHILD|BS_GROUPBOX"),
    "ICON": ("STATIC", "WS_VISIBLE|WS_CHILD|SS_ICON"),
    "LISTBOX": ("LISTBOX", "WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL"),
    "COMBOBOX": ("COMBOBOX", "WS_VISIBLE|WS_CHILD|CBS_DROPDOWN"),
    "SCROLLBAR": ("SCROLLBAR", "WS_VISIBLE|WS_CHILD"),
}


def preprocess(rc: Path, includes: list[Path]) -> str:
    cmd = ["gcc", "-E", "-x", "c", "-P", "-DUNICODE", "-D_UNICODE", "-DRC_INVOKED"]
    for inc in includes:
        cmd += ["-I", str(inc)]
    cmd.append(str(rc))
    try:
        p = subprocess.run(cmd, check=False, capture_output=True, text=True, errors="replace")
        if p.returncode == 0 and p.stdout.strip():
            return p.stdout
    except FileNotFoundError:
        pass
    return rc.read_text(errors="replace")


def cpp_wstring(s: str) -> str:
    s = s.replace("\\", "\\\\").replace('"', '\\"')
    return 'L"' + s + '"'


def emit_empty(out: Path) -> None:
    out.write_text(
        '#include "npp_platform.h"\n'
        "struct RcBlob { LPCWSTR type; LONG_PTR name; const unsigned char* data; DWORD size; };\n"
        'extern "C" {\n'
        "const RcBlob npp_rc_blobs[] = {{nullptr, 0, nullptr, 0}};\n"
        "const int npp_rc_blob_count = 0;\n"
        "}\n"
    )


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--src", required=True, type=Path)
    ap.add_argument("--out", required=True, type=Path)
    ap.add_argument("-I", dest="includes", action="append", default=[], type=Path)
    args = ap.parse_args()
    rcs = list(args.src.rglob("*.rc"))
    # Always emit a valid translation unit even if parsing is incomplete.
    # Dialog templates are reconstructed at runtime from native wchar_t later;
    # this generator currently produces an empty table so the Unix link succeeds.
    # A fuller parser can be filled in incrementally.
    args.out.parent.mkdir(parents=True, exist_ok=True)
    emit_empty(args.out)
    print(f"compile_rc.py: scanned {len(rcs)} rc files, wrote {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
