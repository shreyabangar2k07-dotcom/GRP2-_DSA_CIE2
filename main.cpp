#include<iostream>
#include "Registration.cpp"
#include "Authentication.cpp"
using namespace std;
int main(){
    Regstration r;
    UserRecords records;
    Authentication auth;
    int choice;

    do{
        cout<<"\nSTUDENT LOGIN SYSTEM\n";
        cout<<"1. Register new student\n";
        cout<<"2. display stored records\n";
        cout << "3. Login\n";
        cout << "4. Exit\n";
        cout<<"Enter choice: ";
        cin>>choice;

        switch(choice){
            case 1:
                r.registerUser();
                break;
            case 2:
                records.displayRecords();
                break;
            case 3:
                auth.authenticateUser();
                break;
            case 4:
                cout<<"Exiting\n";
                break;
            default:
                cout<<"Invalid input\n";
        }

    }while(choice != 4);

    return 0;
}