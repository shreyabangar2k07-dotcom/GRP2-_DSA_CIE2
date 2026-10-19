#pragma once
#include<iostream>
#include<exception>
using namespace std;

class MultipleFailedAttemptsException : public exception{
    public:
    const char* what() const noexcept override{
        return "Too many failed login attempts\n";
    }
};