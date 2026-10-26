# Secure Password Simulator — Qt GUI

This folder adds a Qt 6 Widgets GUI **without editing the seven original C++ files** under `core/`.

## Linux dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libsodium-dev pkg-config
```

## Build and run

```bash
cmake -S . -B build
cmake --build build
./build/secure_password_gui
```

Run the executable from the **project root**, as shown, because the unchanged `UserRecords.cpp` opens `User.txt` relative to the process working directory. If your existing `User.txt` is in another folder, copy it into the project root before running, or launch from the directory containing that file.

## What is included

- `core/`: original, unmodified source files (including the original console `main.cpp`)
- `gui/MainWindow.h`, `gui/MainWindow.cpp`: simple tab-like sidebar UI
- `gui/GuiMain.cpp`: Qt entry point and adapter to the existing `cin`/`cout` methods
- `CMakeLists.txt`: builds the GUI, not the original console main

## Important limitations

- The original functions use `std::cin >>` and therefore only accept whitespace-free PRNs/passwords. The adapter rejects whitespace to prevent shifted inputs.
- The original authentication code maintains a 3-attempt / 60-second lockout **in memory**. This GUI reuses one Authentication instance during the session, but restarting the application resets that state, just as restarting the original console program does.
- The original records display includes encoded password hashes. Avoid showing or sharing those records in public demonstrations.
- The GUI does not change authentication logic, data format, or error messages.
- This GUI has not been compiled in this environment unless the required Qt and libsodium packages are installed.

## GUI improvements
- The Login page now displays a live MM:SS countdown when the original authentication code locks an account. The login button is disabled for that PRN until the countdown finishes; switching PRNs updates the display.
- Console-only input prompts are filtered from the result panel, because the GUI already contains labeled input fields.
- Core files in `core/` remain unchanged. The existing three-attempt/60-second policy remains in Authentication.cpp.

## Login session interface
After a successful login, the sidebar displays the authenticated PRN and
shows a **Log out** button. **Change Password** is enabled only after login;
its PRN field is read-only and uses the logged-in account. Logging out hides
these controls and returns to Login. This is a GUI-only session; the original
core logic and its in-memory failed-attempt tracking are unchanged.
