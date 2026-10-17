#include<iostream>
#include<fstream>
#include<sstream>
#include<unordered_map>
#include<chrono> //for timer
#include<cmath>
#include<sodium.h>

#include "LoginException.cpp"
using namespace std;

class Authentication{
    private:
    unordered_map<string, int> failedAttempts;

    //Store the lockout expiry time for each PRN
    unordered_map<string, chrono::steady_clock::time_point> lockoutUntil;

    const int maxAttempts = 3;
    const int lockoutTime = 60; //in seconds

    string getPasswordHash(string prn){
        ifstream file("User.txt");
        if(!file){
            return "File not found\n";
        }

        string line;

        while(getline(file, line)){
            stringstream ss(line);

            string sno, storedPRN, hash;
            ss>>sno>>storedPRN>>hash;

            //skip header
            if(sno == "SNO") continue;

            if(storedPRN == prn) return hash;
        }

        return "";
    }
    public:
    void authenticateUser(){
        string prn, password;

        cout<<"\nSTUDENT LOGIN\n";
        cout<<"Enter PRN: ";
        cin>>prn;

        //check whether this prn is currently locked
        auto now = chrono::steady_clock::now();

        if(lockoutUntil.count(prn) && 
            now < lockoutUntil[prn]){

                int remaining = (int)ceil(
                    chrono::duration<double>(
                    lockoutUntil[prn] - now).count()
                    );
            

            cout<<"Account temporarily locked\n";
            cout<<"Try again in "<<remaining<<" seconds\n";
            return;
        }

        //remove an expired lockout
        if(lockoutUntil.count(prn)){

            lockoutUntil.erase(prn);
            failedAttempts[prn] = 0;
        }

        //retrieve the stored password hash
        string storedHash = getPasswordHash(prn);
        if(storedHash.empty()){
            cout<<"Invalid PRN or password\n";
            return;
        }

        cout<<"Enter Password: ";
        cin>>password;

        //initialise libsodium
        if(sodium_init() < 0){
            cerr<<"Error initalizing libsodium\n";
            return;
        }

        //verify the entered password against the saved hash

        if(crypto_pwhash_str_verify(
            storedHash.c_str(),
            password.c_str(),
            password.length()
        )==0){
            cout<<"Login successfull\n";
            failedAttempts[prn] = 0;
            return;
        }

        //password was incorrect
        failedAttempts[prn]++;

        cout<<"Incorrect password\n";
        cout<<"Failed Attempts: "
            <<failedAttempts[prn]
            <<"/"<<maxAttempts<<endl;

        //lock the account after reaching the limits
        if(failedAttempts[prn] >= maxAttempts){
            try{
                throw MultipleFailedAttemptsException();
            }
            catch(const MultipleFailedAttemptsException &e){
                cout<<e.what()<<endl;

                lockoutUntil[prn] = chrono::steady_clock::now()
                    + chrono::seconds(lockoutTime);

                cout<<"Login locked for 60 seconds\n";
                failedAttempts[prn] = 0;
            }
        }
    }
};