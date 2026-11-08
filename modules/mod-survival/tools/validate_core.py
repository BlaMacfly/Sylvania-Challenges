"""Compile/check a staged module against the current core, without changing the core or its build."""
import argparse
import json
from pathlib import Path
import shlex
import shutil
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--core", type=Path, required=True)
    parser.add_argument("--module", type=Path, required=True)
    parser.add_argument("--challenge", type=Path, required=True)
    parser.add_argument("--store", type=Path, required=True)
    args = parser.parse_args()
    args.module = args.module.resolve()
    output = args.module.parent / "validation"
    output.mkdir(exist_ok=True)
    entries = json.loads((args.core / "build/compile_commands.json").read_text())
    entry = next(row for row in entries if row["file"].endswith("/mod-challenge-modes/src/ChallengeModes.cpp"))
    original = entry.get("arguments") or shlex.split(entry["command"])
    flags = []
    skip = False
    for value in original:
        if skip:
            skip = False
            continue
        if value == "-o":
            skip = True
            continue
        if value == "-c" or value == entry["file"]:
            continue
        flags.append(value)

    results = []

    def run(name, command, cwd=None):
        result = subprocess.run(command, cwd=cwd, text=True, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, timeout=180)
        (output / (name + ".log")).write_text(result.stdout)
        results.append({"check": name, "exit_code": result.returncode, "output": result.stdout[-6000:]})
        print(json.dumps(results[-1]), flush=True)
        if result.returncode:
            raise SystemExit(result.returncode)

    for path in [args.module / "src/Survival.cpp", args.module / "src/survival_loader.cpp", args.challenge]:
        run("compile_" + path.stem, flags + ["-O0", "-g0", "-I" + str(args.module / "src"),
            "-c", str(path), "-o", str(output / (path.stem + ".o"))], entry["directory"])

    binary = output / "model_tests"
    run("build_model_tests", [original[0], "-std=c++20", "-Wall", "-Wextra", "-Werror", "-g",
        "-fsanitize=address,undefined", "-I" + str(args.module / "src"),
        str(args.module / "tests/model_tests.cpp"), "-o", str(binary)])
    run("model_tests", [str(binary)])
    run("php_syntax", ["php", "-l", str(args.store)])
    run("store_guard", ["php", str(args.module / "tests/store_guard_test.php"), str(args.store)])
    lua = shutil.which("lua5.1") or shutil.which("lua5.2") or shutil.which("lua")
    if lua is None:
        # The R1 core already builds Lua for ALE; use its own CLI if installed there.
        for candidate in args.core.glob("build/_deps/**/lua"):
            if candidate.is_file() and candidate.stat().st_mode & 0o111:
                lua = str(candidate)
                break
    if lua is None:
        source = args.core / "build/_deps/lua52-src/src"
        library = args.core / "build/modules/mod-ale/src/lualib/lua/liblua52.a"
        if not (source / "lua.c").is_file() or not library.is_file():
            raise SystemExit("Lua runtime unavailable: addon validation remains required.")
        lua = str(output / "lua")
        run("build_lua_cli", ["cc", "-I" + str(source), str(source / "lua.c"), str(library), "-ldl", "-lm", "-o", lua])
    run("addon", [lua, str(args.module / "tests/addon_test.lua"),
        str(args.module / "addon/SylvaniaSurvival/SylvaniaSurvival.lua")])
    (output / "results.json").write_text(json.dumps(results, indent=2))
    print("VALIDATION_COMPLETE", output)


if __name__ == "__main__":
    main()
