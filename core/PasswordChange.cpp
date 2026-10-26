#pragma once
#include <iostream>
#include <string>
#include <sodium.h>
#include "UserRecords.cpp"
#include "PasswordHasher.cpp"
using namespace std;

class PasswordChange {
private:
    UserRecords records;
    PasswordHasher hasher;

public:
    void changePassword() {
        string prn, oldPassword, newPassword, confirmPassword;
        cout << "\nCHANGE PASSWORD\n";
        cout << "Enter PRN: ";
        cin >> prn;

        string storedHash = records.getPasswordHash(prn);
        if (storedHash.empty()) {
            cout << "Password change failed: PRN not found\n";
            return;
        }

        cout << "Enter current password: ";
        cin >> oldPassword;
        if (crypto_pwhash_str_verify(storedHash.c_str(), oldPassword.c_str(), oldPassword.length()) != 0) {
            cout << "Password change failed: current password is incorrect\n";
            return;
        }

        cout << "Enter new password: ";
        cin >> newPassword;
        cout << "Confirm new password: ";
        cin >> confirmPassword;

        if (newPassword != confirmPassword) {
            cout << "New passwords do not match\n";
            return;
        }
        if (newPassword == oldPassword) {
            cout << "New password must be different from the current password\n";
            return;
        }

        string newHash = hasher.hashPasswords(newPassword);
        if (newHash.empty()) {
            cout << "Password change failed: hashing error\n";
            return;
        }

        if (records.updatePasswordHash(prn, newHash))
            cout << "Password changed successfully\n";
        else
            cout << "Password change failed: could not update user record\n";
    }
};
