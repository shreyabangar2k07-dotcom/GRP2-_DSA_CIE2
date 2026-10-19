#pragma once
#include <iostream>
#include <string>
#include "PasswordHasher.cpp"
#include "UserRecords.cpp"
using namespace std;

class Regstration {
private:
    UserRecords records;
    PasswordHasher hasher;

public:
    void registerUser() {
        string prn, password, confirmPassword;
        cout << "\nSTUDENT REGISTRATION\n";
        cout << "Enter PRN: ";
        cin >> prn;

        if (records.checkPRN(prn)) {
            cout << "This PRN is already registered\n";
            return;
        }

        cout << "Enter password: ";
        cin >> password;
        cout << "Confirm Password: ";
        cin >> confirmPassword;

        if (password != confirmPassword) {
            cout << "Passwords do not match!\n";
            return;
        }

        string passwordHash = hasher.hashPasswords(password);
        if (passwordHash.empty()) {
            cout << "Registration failed: could not hash password\n";
            return;
        }

        int sno = records.nextSerialNo();
        if (records.saveUser(sno, prn, passwordHash))
            cout << "Registration successful\n";
        else
            cout << "Registration failed: could not save user\n";
    }
};
