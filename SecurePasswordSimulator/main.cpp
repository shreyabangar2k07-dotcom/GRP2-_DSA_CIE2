#include <iostream>
#include <limits>
#include <sodium.h>
#include "Registration.cpp"
#include "Authentication.cpp"
#include "PasswordChange.cpp"
#include "UserRecords.cpp"
using namespace std;

int main() {
    if (sodium_init() < 0) {
        cerr << "Failed to initialize libsodium\n";
        return 1;
    }

    Regstration registration;
    Authentication authentication;
    PasswordChange passwordChange;
    UserRecords records;

    int choice;
    do {
        cout << "\nSECURE PASSWORD STORAGE AND LOGIN SIMULATOR\n";
        cout << "1. Register new user\n";
        cout << "2. Display stored records\n";
        cout << "3. Login\n";
        cout << "4. Change password\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input\n";
            continue;
        }

        switch (choice) {
            case 1: registration.registerUser(); break;
            case 2: records.displayRecords(); break;
            case 3: authentication.authenticateUser(); break;
            case 4: passwordChange.changePassword(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice\n";
        }
    } while (choice != 0);

    return 0;
}
