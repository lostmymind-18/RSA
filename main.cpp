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
    BigInt y(500,true);
    std::cout<<"y decimal: "<<y.decimal()<<'\n';
    std::cout<<"y binary: "<<y.binary()<<'\n';
    BigInt x(200,true);
    std::cout<<"x decimal: "<<x.decimal()<<'\n';
    std::cout<<"x binary: "<<x.binary()<<'\n';
    std::cout<<"z = "<<y.decimal()<<" % "<<x.decimal()<<'\n';
    BigInt z = y%x;
    std::cout<<"z decimal: "<<z.decimal()<<'\n';
    std::cout<<"z binary: "<<z.binary()<<'\n';
}