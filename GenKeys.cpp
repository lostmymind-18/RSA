#include"BigInt.h"

const int NUM_MILLER_RABIN = 10;

bool Miller_Rabin(){
    return false;
}

//This function generate a big prime
BigInt prime(int bits_length){
    while(true){
        //Get an odd number
        BigInt candidate(bits_length,true);
        while(candidate.isOdd() == false)
            candidate = BigInt(bits_length, true);
        //Select a number less then the odd number
        BigInt less_than_number(bits_length, true);
        //Check with Miller-Rabin
        for(int i = 0; i < NUM_MILLER_RABIN; i++){
            if(Miller_Rabin() == false)
                break;
        }
        return candidate;
    }
}