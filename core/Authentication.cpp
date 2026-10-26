#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <cmath>
#include <sodium.h>
#include "LoginException.cpp"
#include "UserRecords.cpp"
using namespace std;

class Authentication {
private:
    unordered_map<string, int> failedAttempts;
    unordered_map<string, chrono::steady_clock::time_point> lockoutUntil;
    const int maxAttempts = 3;
    const int lockoutTime = 60;
    UserRecords records;

public:
    void authenticateUser() {
        string prn, password;
        cout << "\nSTUDENT LOGIN\nEnter PRN: ";
        cin >> prn;

        auto now = chrono::steady_clock::now();
        if (lockoutUntil.count(prn) && now < lockoutUntil[prn]) {
            int remaining = (int)ceil(chrono::duration<double>(lockoutUntil[prn] - now).count());
            cout << "Account temporarily locked\nTry again in " << remaining << " seconds\n";
            return;
        }

        if (lockoutUntil.count(prn)) {
            lockoutUntil.erase(prn);
            failedAttempts[prn] = 0;
        }

        string storedHash = records.getPasswordHash(prn);
        if (storedHash.empty()) {
            cout << "Invalid PRN or password\n";
            return;
        }

        cout << "Enter Password: ";
        cin >> password;

        if (sodium_init() < 0) {
            cerr << "Error initializing libsodium\n";
            return;
        }

        if (crypto_pwhash_str_verify(storedHash.c_str(), password.c_str(), password.length()) == 0) {
            cout << "Login successful\n";
            failedAttempts[prn] = 0;
            return;
        }

        ++failedAttempts[prn];
        cout << "Incorrect password\nFailed Attempts: "
             << failedAttempts[prn] << "/" << maxAttempts << '\n';

        if (failedAttempts[prn] >= maxAttempts) {
            try {
                throw MultipleFailedAttemptsException();
            } catch (const MultipleFailedAttemptsException& e) {
                cout << e.what() << '\n';
                lockoutUntil[prn] = chrono::steady_clock::now() + chrono::seconds(lockoutTime);
                cout << "Login locked for " << lockoutTime << " seconds\n";
                failedAttempts[prn] = 0;
            }
        }
    }
};
