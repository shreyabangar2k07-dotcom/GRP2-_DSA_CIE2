#include <iostream>
#include <limits>
#include <sodium.h>
#include "Registration.cpp"
#include "Authentication.cpp"
#include "PasswordChange.cpp"
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
    LoginHistory loginHistory;
    SignOut signOut;

    // Wire shared objects into authentication
    authentication.setLoginHistory(&loginHistory);
    authentication.setSignOut(&signOut);

    int choice;
    do {
        cout << "\nSECURE PASSWORD STORAGE AND LOGIN SIMULATOR\n";
        cout << "1. Register new user\n";
        cout << "2. Display stored records\n";
        cout << "3. Login\n";
        cout << "4. Change password\n";
        cout << "5. Sign out\n";
        cout << "6. Login history\n";
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
            case 5: signOut.signOutUser(); break;
            case 6: loginHistory.displayHistory(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice\n";
        }
    } while (choice != 0);

    return 0;
}

