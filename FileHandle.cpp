#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "User.cpp"
using namespace std;

class File {
    private:
        fstream file;
        int noOfEntries;

    public:
        File() {
            noOfEntries = 0;
            newFile();
        }

        void newFile() {
            file.open("sample.txt", ios::out);
			
			file << left << setw(9) << "S.No." << setw(12) << "PRN" << setw(18) << "Login Status" << "Password" << endl;
			
			file.close();
        }

        bool insertData(User user) {
            file.open("sample.txt", ios::app);

            if(!file.is_open()) {
				return false;
			}

            file << left << setw(9) << ++noOfEntries << setw(12) << user.prn << setw(18) << user.loginStatus << user.password << endl;

            file.close();

            return true;
        }

        bool findData(string prn) {
            file.open("sample.txt", ios::in);

            if(!file.is_open()) {
				return false;
			}

            string line;

            while(getline(file, line)) {
                string lineArr[3];
                
                stringstream input(line);
                int i = 0;

                while(input.good() && i < 3) {
                    input >> lineArr[i];
                    i++;
                }

                if(lineArr[1] == prn) {
                    file.close();
                    return true;
                }
            }

            file.close();

            return false;
        }

        string getPassword(string prn) {
            file.open("sample.txt", ios::in);

            if(!file.is_open()) {
				return "ERR";
			}

            string line;

            while(getline(file, line)) {
                string lineArr[4];
                
                stringstream input(line);
                int i = 0;

                while(input.good() && i < 4) {
                    input >> lineArr[i];
                    i++;
                }

                if(lineArr[1] == prn) {
                    file.close();
                    return lineArr[3];
                }
            }

            file.close();

            return "ERR";
        }

        bool updateLoginStatus(User user) {
            file.open("sample.txt", ios::in);
            fstream temp("temp.txt", ios::out);
            bool updated = false;

            if(!file.is_open() || !temp.is_open()) {
                return false;
            }

            string line;

            while(getline(file, line)) {
                string lineArr[4];

                stringstream input(line);
                int i = 0;

                while(input.good() && i < 4) {
                    input >> lineArr[i];
                    i++;
                }

                if(lineArr[1] == user.prn) {
                    temp << left << setw(9) << lineArr[0] << setw(12) << lineArr[1] << setw(18) << user.loginStatus << lineArr[3] << endl;
                    updated = true;
                }
                else {
                    temp << line << endl;
                }
            }

            file.close();
			temp.close();
			remove("sample.txt");
			rename("temp.txt", "sample.txt");

            return updated;
        }

        bool updatePassword(User user) {
            file.open("sample.txt", ios::in);
            fstream temp("temp.txt", ios::out);
            bool updated = false;

            if(!file.is_open() || !temp.is_open()) {
                return false;
            }

            string line;

            while(getline(file, line)) {
                string lineArr[4];

                stringstream input(line);
                int i = 0;

                while(input.good() && i < 4) {
                    input >> lineArr[i];
                    i++;
                }

                if(lineArr[1] == user.prn) {
                    temp << left << setw(9) << lineArr[0] << setw(12) << lineArr[1] << setw(18) << lineArr[2] << user.password << endl;
                    updated = true;
                }
                else {
                    temp << line << endl;
                }
            }

            file.close();
			temp.close();
			remove("sample.txt");
			rename("temp.txt", "sample.txt");

            return updated;
        }
};
