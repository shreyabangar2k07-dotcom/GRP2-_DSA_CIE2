#include <iostream>
#include <exception>
using namespace std;

class LoginFailException: public exception {
    public:
        const char* what() const noexcept override {
            return "Login limit has been reached";
        }
};
