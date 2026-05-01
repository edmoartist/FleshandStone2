from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

REQUIRED = [
    "src/main.cpp",
    "build_msvc.bat",
    "build_mingw.bat",
    "README.md",
]


def main() -> None:
    missing = [path for path in REQUIRED if not (ROOT / path).exists()]

    if missing:
        print("Missing required files:")
        for path in missing:
            print(f" - {path}")
        raise SystemExit(1)

    main_cpp = (ROOT / "src/main.cpp").read_text(encoding="utf-8")

    checks = {
        "WinMain exists": "WinMain" in main_cpp,
        "window proc exists": "WindowProc" in main_cpp,
        "software backbuffer exists": "Backbuffer" in main_cpp,
        "game update exists": "update_game" in main_cpp,
        "game render exists": "render_game" in main_cpp,
        "portal exists": "enter_next_room" in main_cpp,
    }

    failed = [name for name, passed in checks.items() if not passed]

    if failed:
        print("Failed checks:")
        for name in failed:
            print(f" - {name}")
        raise SystemExit(1)

    print("Smoke test passed.")


if __name__ == "__main__":
    main()
