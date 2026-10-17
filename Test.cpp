#include <iostream>
#include <sodium.h>
using namespace std;

int main() {
    if (sodium_init() < 0) {
        cout << "Initialization failed\n";
        return 1;
    }

    cout << "libsodium is working!\n";

    return 0;
}
