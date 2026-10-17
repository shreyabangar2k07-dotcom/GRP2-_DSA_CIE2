#include <iostream>
using namespace std;

class User {
    public:
        string prn;
        string password;
        int loginStatus;

        User() {
            prn = "";
            password = "";
            loginStatus = false;
        }

        User(string prn, string password, int loginStatus) {
            this->prn = prn;
            this->password = password;
            this->loginStatus = loginStatus;
        }
};
