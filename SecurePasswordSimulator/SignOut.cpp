#pragma once
#include <iostream>
#include <string>
using namespace std;

class SignOut {
private:
    string loggedInPRN;

public:
    SignOut() : loggedInPRN("") {}

    // Set the currently logged-in user
    void setLoggedInUser(const string& prn) {
        loggedInPRN = prn;
    }

    // Get the currently logged-in user
    string getLoggedInUser() const {
        return loggedInPRN;
    }

    // Check if any user is currently logged in
    bool isLoggedIn() const {
        return !loggedInPRN.empty();
    }

    // Sign out the current user
    void signOutUser() {
        if (!isLoggedIn()) {
            cout << "No user is currently signed in\n";
            return;
        }
        cout << "User " << loggedInPRN << " signed out successfully\n";
        loggedInPRN = "";
    }
};
