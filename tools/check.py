"""Run the portable Survival checks and verify the installed-source snapshot."""
from pathlib import Path
import hashlib
import json
import os
import shutil
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
MODULE = ROOT / "modules/mod-survival"


def verify_snapshot():
    manifest = json.loads((ROOT / "docs/SERVER_SNAPSHOT.json").read_text(encoding="utf-8"))
    for name, expected in manifest["installed_file_sha256"].items():
        path = ROOT / name
        if not path.resolve().is_relative_to(ROOT):
            raise RuntimeError("Snapshot path escapes the repository")
        actual = hashlib.sha256(path.read_bytes()).hexdigest()
        if actual != expected:
            raise RuntimeError("Installed-source snapshot differs: " + name)
    print("Installed-source snapshot: all file hashes match", flush=True)


def verify_addon_assets():
    addon = MODULE / "addon/SylvaniaSurvival"
    toc = (addon / "SylvaniaSurvival.toc").read_text(encoding="utf-8")
    assert "## Interface: 30300" in toc and "## Version: 0.2.0" in toc
    for name in ["Food", "Water", "Cold"]:
        data = (addon / "Textures" / (name + ".tga")).read_bytes()
        header = struct.unpack_from("<BBBHHBHHHHBB", data)
        assert header[1:3] == (0, 2), "Texture must use uncompressed true-color TGA"
        assert header[8:11] == (128, 128, 24), "Texture must be 128x128 RGB"
        assert len(data) >= 18 + 128 * 128 * 3
    print("WotLK addon manifest and three RGB TGA textures: valid", flush=True)


def main():
    verify_snapshot()
    verify_addon_assets()
    compiler = os.environ.get("CXX") or shutil.which("g++") or shutil.which("clang++") or shutil.which("c++")
    lua = os.environ.get("LUA") or shutil.which("lua5.1") or shutil.which("lua")
    if not compiler or not lua:
        raise SystemExit("Install a C++20 compiler and Lua 5.1, or set CXX and LUA to their executables.")
    with tempfile.TemporaryDirectory(prefix="sylvania-check-") as directory:
        binary = str(Path(directory) / "model_tests")
        subprocess.run([compiler, "-std=c++20", "-Wall", "-Wextra", "-Werror", "-g", "-O1",
                        "-fsanitize=address,undefined", "-I" + str(MODULE / "src"),
                        str(MODULE / "tests/model_tests.cpp"), "-o", binary], check=True)
        subprocess.run([binary], check=True)
    subprocess.run([lua, str(MODULE / "tests/addon_test.lua"),
                    str(MODULE / "addon/SylvaniaSurvival/SylvaniaSurvival.lua")], check=True)
    print("55 portable behavior checks passed", flush=True)


if __name__ == "__main__":
    main()
