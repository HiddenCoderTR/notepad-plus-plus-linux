#!/usr/bin/env python3
"""Generate NppLibsVersion.h from Scintilla/Lexilla/Boost version macros."""
import re
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[4] if "Platform" in Path(__file__).parts else Path(__file__).resolve().parents[3]
# tools/ is Platform/unix/tools -> repo root is 5 levels up from file? 
# PowerEditor/src/Platform/unix/tools/generate_npp_libs_version.py
# parents[0]=tools, [1]=unix, [2]=Platform, [3]=src, [4]=PowerEditor, [5]=repo
root = Path(__file__).resolve().parents[5]

def extract(path: Path, macro: str) -> str:
    if not path.exists():
        return '"N/A"'
    text = path.read_text(errors="replace")
    m = re.search(rf'#define\s+{re.escape(macro)}\s+("?[^"\n]+"?)', text)
    return m.group(1).strip() if m else '"N/A"'

sci = extract(root / "scintilla/win32/ScintRes.rc", "VERSION_SCINTILLA")
lex = extract(root / "lexilla/src/LexillaVersion.rc", "VERSION_LEXILLA")
boost = extract(root / "boostregex/boost/version.hpp", "BOOST_LIB_VERSION")

out = root / "PowerEditor/src/NppLibsVersion.h"
out.write_text(
    "// NppLibsVersion.h\n"
    "// - maintained by generate_npp_libs_version.py\n"
    f"#define NPP_SCINTILLA_VERSION {sci}\n"
    f"#define NPP_LEXILLA_VERSION {lex}\n"
    f"#define NPP_BOOST_REGEX_VERSION {boost}\n"
)
print(f"Wrote {out}")
print(f"  Scintilla {sci} Lexilla {lex} Boost {boost}")
