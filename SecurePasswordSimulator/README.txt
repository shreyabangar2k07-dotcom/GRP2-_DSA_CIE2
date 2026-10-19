# Integrated Secure Password Storage and Login Simulator

## Files
- `main.cpp`: the only entry point and menu.
- `Registration.cpp`: registration using Argon2id hashing.
- `Authentication.cpp`: login and temporary lockout after three incorrect passwords.
- `PasswordChange.cpp`: verifies the current password, then hashes and saves the new password.
- `UserRecords.cpp`: shared `User.txt` account storage.
- `PasswordHasher.cpp`: Libsodium password hashing.
- `LoginException.cpp`: exception for repeated failed attempts.

## Important
This version uses `User.txt` as the single shared account store. It does not use Part 2's separate `sample.txt`, because separate storage files would make registration/login/password changes inconsistent.

The lockout counter and timer are kept in memory and reset when the program exits. Persisting lockouts across restarts would require storing lockout data separately.

## Build on Linux
Install the compiler and libsodium development package if needed:
```bash
sudo apt update
sudo apt install g++ libsodium-dev
```

Compile only `main.cpp`, because the existing project style includes implementation `.cpp` files into the entry-point translation unit:
```bash
g++ -std=c++17 main.cpp -o SecurePasswordSimulator -lsodium
```

Run:
```bash
./SecurePasswordSimulator
```

Do not pass every `.cpp` file to the compiler in the same command with this include-based layout, or duplicate definitions / multiple `main` functions can result.

## Data file
`User.txt` is created on first registration. It contains PRNs and encoded password hashes, not plaintext passwords. Do not share it because it contains sensitive account data.
