"""Build a deterministic full module pack and the standalone WotLK addon ZIP."""
from pathlib import Path
import hashlib
import re
import zipfile

ROOT = Path(__file__).resolve().parents[1]
ADDON = ROOT / "modules/mod-survival/addon/SylvaniaSurvival"
ROOT_FILES = ["README.md", "LICENSE", "LICENSE-AGPL3", "NOTICE.md", "AUTHORS.md", "CHANGELOG.md", "VERSION"]


def add(archive, source, name):
    info = zipfile.ZipInfo(name, date_time=(1980, 1, 1, 0, 0, 0))
    info.create_system = 3
    info.compress_type = zipfile.ZIP_DEFLATED
    info.external_attr = 0o100644 << 16
    archive.writestr(info, source.read_bytes())


def files(directory):
    return sorted(path for path in directory.rglob("*") if path.is_file()
                  and "__pycache__" not in path.parts and path.suffix != ".pyc")


def main():
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    if not re.fullmatch(r"\d+\.\d+\.\d+(?:-[A-Za-z0-9.]+)?", version):
        raise SystemExit("Invalid release version")
    output = ROOT / "dist"
    output.mkdir(exist_ok=True)
    full = output / ("Sylvania-Challenges-v" + version + ".zip")
    standalone = output / "SylvaniaSurvival-0.2.0-WotLK-3.3.5a.zip"
    prefix = "Sylvania-Challenges-v" + version + "/"
    with zipfile.ZipFile(full, "w") as archive:
        for name in ROOT_FILES:
            add(archive, ROOT / name, prefix + name)
        for directory in ["modules", "docs", "profiles", "artwork", "tools"]:
            for path in files(ROOT / directory):
                add(archive, path, prefix + path.relative_to(ROOT).as_posix())
        for path in files(ADDON):
            add(archive, path, prefix + "Interface/AddOns/SylvaniaSurvival/" + path.relative_to(ADDON).as_posix())
    with zipfile.ZipFile(standalone, "w") as archive:
        for path in files(ADDON):
            add(archive, path, "SylvaniaSurvival/" + path.relative_to(ADDON).as_posix())
    for path in [full, standalone]:
        with zipfile.ZipFile(path) as archive:
            assert archive.testzip() is None
            assert len(archive.namelist()) == len(set(archive.namelist()))
            assert not any(name.startswith("/") or ".." in name.split("/") for name in archive.namelist())
    checksums = "".join(hashlib.sha256(path.read_bytes()).hexdigest() + "  " + path.name + "\n" for path in [full, standalone])
    (output / "SHA256SUMS.txt").write_text(checksums, encoding="ascii", newline="\n")
    print(checksums, end="")


if __name__ == "__main__":
    main()
