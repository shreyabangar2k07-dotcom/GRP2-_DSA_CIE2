#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdio>
using namespace std;

class UserRecords {
public:
    bool checkPRN(const string& prn) {
        ifstream file("User.txt");
        if (!file) return false;

        string line;
        getline(file, line); // skip the header
        while (getline(file, line)) {
            stringstream ss(line);
            string sno, storedPRN, hash;
            if (ss >> sno >> storedPRN >> hash) {
                if (storedPRN == prn) return true;
            }
        }
        return false;
    }

    int nextSerialNo() {
        ifstream file("User.txt");
        if (!file) return 1;

        string line;
        int count = 0;
        getline(file, line); // skip header
        while (getline(file, line)) {
            if (!line.empty()) ++count;
        }
        return count + 1;
    }

    bool saveUser(int sno, const string& prn, const string& passwordHash) {
        bool isEmpty = true;
        ifstream checkFile("User.txt");
        if (checkFile && checkFile.peek() != EOF) isEmpty = false;
        checkFile.close();

        ofstream file("User.txt", ios::app);
        if (!file) {
            cerr << "Error opening User.txt\n";
            return false;
        }

        if (isEmpty) {
            file << left << setw(10) << "SNO"
                 << setw(20) << "PRN" << "HASH\n";
        }

        file << left << setw(10) << sno
             << setw(20) << prn << passwordHash << '\n';
        return file.good();
    }

    string getPasswordHash(const string& prn) {
        ifstream file("User.txt");
        if (!file) return "";

        string line;
        getline(file, line); // skip header
        while (getline(file, line)) {
            stringstream ss(line);
            string sno, storedPRN, hash;
            if (ss >> sno >> storedPRN >> hash && storedPRN == prn)
                return hash;
        }
        return "";
    }

    bool updatePasswordHash(const string& prn, const string& newHash) {
        ifstream file("User.txt");
        if (!file) return false;

        ofstream temp("User_temp.txt");
        if (!temp) return false;

        string line;
        if (getline(file, line)) {
            temp << line << '\n'; // preserve the existing header
        } else {
            file.close();
            temp.close();
            remove("User_temp.txt");
            return false;
        }

        bool updated = false;
        while (getline(file, line)) {
            stringstream ss(line);
            string sno, storedPRN, oldHash;
            if (ss >> sno >> storedPRN >> oldHash) {
                if (storedPRN == prn) {
                    temp << left << setw(10) << sno
                         << setw(20) << storedPRN << newHash << '\n';
                    updated = true;
                } else {
                    temp << line << '\n';
                }
            } else {
                temp << line << '\n';
            }
        }

        file.close();
        temp.close();

        if (!updated) {
            remove("User_temp.txt");
            return false;
        }

        if (remove("User.txt") != 0) return false;
        if (rename("User_temp.txt", "User.txt") != 0) return false;
        return true;
    }

    void displayRecords() {
        ifstream file("User.txt");
        if (!file) {
            cout << "No user records found\n";
            return;
        }

        cout << "\nSTUDENT RECORDS\n";
        cout << left << setw(10) << "S.NO."
             << setw(20) << "PRN" << "Password Hash\n";
        cout << string(60, '-') << '\n';

        string line;
        getline(file, line); // skip file header
        while (getline(file, line)) {
            stringstream ss(line);
            string sno, prn, hash;
            if (ss >> sno >> prn >> hash) {
                cout << left << setw(10) << sno
                     << setw(20) << prn << hash << '\n';
            }
        }
    }
};
