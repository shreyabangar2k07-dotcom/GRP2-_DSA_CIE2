#include <iostream>
#include <sodium.h>
#include <cstring>
#include "FileHandle.cpp"
#include "LoginFailException.cpp"
using namespace std;

class System {
    private:
        User user;
        File file;
        int loginTries;

    public:
        System() {
            loginTries = 0;
        }

        void registerUser() {
            cin.ignore();

            cout << "Enter PRN: ";
            getline(cin, user.prn);
            cout << "Enter password: ";
            getline(cin, user.password);

            char hash[crypto_pwhash_STRBYTES];

            int result = crypto_pwhash_str(
                hash,
                user.password.c_str(),
                user.password.size(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE
            );

            if(result != 0) {
                cout << "Registration failed" << endl;
                return;
            }

            user.password = hash;

            if(!file.insertData(user)) {
                cout << "Registration failed" << endl;
            }

            cout << "Registration successful" << endl;
        }

        void loginUser() {
            if(loginTries == 3) {
                throw LoginFailException();
            }

            cin.ignore();

            cout << "Enter PRN: ";
            getline(cin, user.prn);
            cout << "Enter password: ";
            getline(cin, user.password);

            if(!file.findData(user.prn)) {
                cout << "Login failed" << endl;
                loginTries++;
                return;
            }
            
            string storedPassword = file.getPassword(user.prn);

            if(storedPassword == "ERR") {
                cout << "Login failed" << endl;
                return;
            }

            int result = crypto_pwhash_str_verify(
                storedPassword.c_str(),
                user.password.c_str(),
                user.password.size()
            );

            if(result == 0) {
                user.loginStatus = 1;
                if(!file.updateLoginStatus(user)) {
                    cout << "Login failed" << endl;
                    loginTries++;
                }
                else {
                    cout << "Login successful" << endl;
                    loginTries = 0;
                }
            }
            else
                cout << "Login failed" << endl;
        }

        void changePassword() {
            cin.ignore();

            cout << "Enter PRN: ";
            getline(cin, user.prn);
            cout << "Enter new password: ";
            getline(cin, user.password);

            if(!file.findData(user.prn)) {
                cout << "Password changing procedure failed" << endl;
                return;
            }

            char hash[crypto_pwhash_STRBYTES];

            int result = crypto_pwhash_str(
                hash,
                user.password.c_str(),
                user.password.size(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE
            );

            if(result != 0) {
                cout << "Password changing procedure failed" << endl;
                return;
            }

            user.password = hash;

            if(!file.updatePassword(user)) {
                cout << "Password changing procedure failed" << endl;
                return;
            }

            cout << "Password changed successfully" << endl;
        }
};

int main() {
    if(sodium_init() < 0) {
        cout << "Initialization failed" << endl;
        return 1;
    }

    System system;
    int choice = -1;

    while(choice != 0) {
        cout << endl;
        cout << "1. Register user" << endl;
        cout << "2. Login user" << endl;
        cout << "3. Change password" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 0: break;
            case 1: system.registerUser(); break;
            case 2: {
                try {
                    system.loginUser();
                }
                catch(LoginFailException& e) {
                    cout << e.what() << endl;
                }
            } break;
            case 3: system.changePassword(); break;

            default: cout << "Invalid choice!" << endl;
        }
    }

    return 0;
}
