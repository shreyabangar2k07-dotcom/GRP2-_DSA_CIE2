#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
using namespace std;

class UserRecords{
    public:
    
    bool checkPRN(string prn){
        ifstream file("User.txt");
        string sno, storedPRN, hash;

        if(!file) return false;

        while(file>>sno>>storedPRN>>hash){
            if(storedPRN == prn) {
                return true;
            }
        }

        return false;
    }


    int nextSerialNo(){
        ifstream file("User.txt");
        string line;
        int count = 0;

        while(getline(file, line)){

            if(!line.empty()) count++;
        }
        return count + 1;
    }


    bool saveUser(int sno, string prn, string passwordHash){
        ifstream checkFile("User.txt");
        bool isEmpty = (checkFile.peek() == EOF);
        checkFile.close();
        
        ofstream file("User.txt", ios::app);

        if (!file){
            cout<<"Error opening User.txt\n";
            return false;
        }
        if (isEmpty) {
        file << left
             << setw(10) << "SNO"
             << setw(20) << "PRN"
             << "Hashed Password\n";
        }

        file<<left
            <<setw(10)<<sno
            <<setw(20)<<prn
            <<passwordHash<<endl;

        if(!file.good()){
            cout<<"Error writing user records\n";
            return false;
        }

        file.close();
        return true;
    }


    void displayRecords(){
        ifstream file("User.txt");
        string sno, prn, hash;

        if(!file){
            cout<<"No user record found\n";
            return;
        }

        cout<<"\nSTUDENT RECORDS\n";
        cout<<left
            <<setw(10)<<"S.NO."
            <<setw(20)<<"PRN"
            <<"Password Hash"<<endl;
        
        cout << string(60, '-') << '\n';

        while(file>>sno>>prn>>hash){
            cout<<left
                <<setw(10)<<sno
                <<setw(20)<<prn
                <<hash<<endl;
        }
        file.close();
    }
};
