#include<iostream>
#include<string>
#include "PasswordHasher.cpp"
#include "UserRecords.cpp"
using namespace std;

class Regstration{
    private:
    UserRecords records;
    PasswordHasher hasher;

    public:
    void registerUser(){
        string prn, password, confirmPasswword;
        cout<<"\nSTUDENT REGISTRATION\n";
        cout<<"Enter PRN: ";
        cin>>prn;

        if (records.checkPRN (prn)){
            cout<<"This PRN is already registered\n";
            return;
        }

        cout<<"Enter password: ";
        cin>>password;

        cout<<"Confirm Password: ";
        cin>>confirmPasswword;

        if(password != confirmPasswword){
            cout<<"Passwords do not match!\n";
            return;
        }

        //hash password using Argon2id
        string passwordHash = hasher.hashPasswords(password);
        
        if(passwordHash.empty()){
            cout<<"Registration failed: Could not hash password\n";
            return;
        }

        int sno = records.nextSerialNo();

        if(records.saveUser(sno, prn, passwordHash)){
            cout<<"Reegistration successful\n";
        }
        else{
            cout<<"Registration failed: could not save user\n";
        }
    }
};