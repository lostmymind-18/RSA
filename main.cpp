#include<iostream>
#include<vector>
#include<ctime>
#include "BigInt.h"
#include "GenKeys.cpp"
//Implement representation for BigInt
//Generate a random big integer
//Generate a big prime
//Extended Euclid's algorithm for BigInt
//Miller-Rabin

int main(){
    BigInt x(40);
    BigInt y(40);
    x.setBit(0);
    y.setBit(1);
    y.setBit(2);
    y.setBit(3);
    BigInt z = x-y;
    std::cout<<x.decimal()<<" - "<<y.decimal()<<" = "<<z.decimal()<<'\n';
}