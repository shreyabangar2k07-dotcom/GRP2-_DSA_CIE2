#pragma once
#include<iostream>
#include<sodium.h>
using namespace std;
class PasswordHasher{
    public:

    string hashPasswords(string password){

        //initialise libsodium
        if(sodium_init() < 0){
            cerr<<"Error initializing password hashing library\n";
            return "";;
        }

        //buffer for encoded Argonn2id hash
        char hash[crypto_pwhash_STRBYTES];

        //Libsodium generates and stores a random salt
        if(crypto_pwhash_str_alg(
            hash,
            password.c_str(),
            password.length(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE,
            crypto_pwhash_ALG_ARGON2ID13

        )!= 0){
            cerr<<"Password hashing failed\n";
            return "";
        }

        //return encoded hash, incluing salt and parameters
        return string(hash);
    }
};