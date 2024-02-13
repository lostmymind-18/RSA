#ifndef BIGINT_H
#define BIGINT_H
#include<vector>
#include<string>
#include<iostream>
#include<bitset>
#include<random>

const int BITS_IN_ELEMENT = sizeof(int)*8;


/*This class is used to represent big int
For optimization of storage, it's stored by bits, with a vector of integer
An vector's element is like a word in memory, with Big-endian
Big-endian means the most value bytes will be stored in the first element
*/
class BigInt{
private:
    std::vector<int> content;
    //This is a secondary function that support for decimal representation
    std::string add(const std::string& a, const std::string& b){
        std::string result = "";
        int i = a.length() - 1;
        int j = b.length() - 1;
        bool carry = false;
        while(i >= 0 | j >= 0 | carry == true){
            int sum = carry;
            carry = false;
            if(i >= 0) sum += a[i--] - '0';
            if(j >= 0) sum += b[j--] - '0';
            if(sum >= 10){
                result = std::to_string(sum%10) + result;
                carry = true;
            }
            else result = std::to_string(sum) + result;
        }
        return result;
    }
public:
    BigInt(){}
    BigInt(std::string bit_string){
        //Get index of the most value bit
        int most_index = 0;
        while(bit_string[most_index] == '0')
            most_index ++;
        //Cut the string from that index
        bit_string = bit_string.substr(most_index);
        //Count number of elements in a vector needed for representing the string
        int num_elements_needed = (bit_string.size() + BITS_IN_ELEMENT - 1)/BITS_IN_ELEMENT;
        //Buffing the string
        int string_length = bit_string.length();
        for(int i = 0; i < num_elements_needed*BITS_IN_ELEMENT - string_length; i++){
            bit_string = '0' + bit_string;
        }
        //Resize content
        content.resize(num_elements_needed,0);
        //Convert binary to integer
        for(int i = 0; i < content.size(); i++){
            std::bitset<32> bits(bit_string.substr(i*BITS_IN_ELEMENT,BITS_IN_ELEMENT));
            content[i] = bits.to_ulong();
        }
    }
    BigInt(int bits_length, bool random = false){
        this->content.resize((bits_length + BITS_IN_ELEMENT - 1)/BITS_IN_ELEMENT,0);
        if(random == true){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 1);
            for(int i = 0; i < content.size() * BITS_IN_ELEMENT; i++){
                if(dis(gen) == 1)
                    setBit(i);
            }
        }
    }
    void setBit(int pos){
        if(pos >= content.size() * BITS_IN_ELEMENT) throw "Out of range";
        int content_index = pos/BITS_IN_ELEMENT;
        int set_off = pos%BITS_IN_ELEMENT;
        content[content_index] = content[content_index] | (1 << set_off);
    }

    std::string binary()const{
        std::string result = "";
        for(int i = 0; i < this->content.size(); i++){
            std::bitset<sizeof(int)*8> binary(content[i]);
            result = binary.to_string() + result;
        }
        return result;
    }    

    std::string decimal(){
        std::string result = "0";
        //Get string representing binary
        std::string binary_value = binary();
        //The index of previous set bit
        int pre_set_bit = binary_value.length()-1;
        //Value of previous calculated power of two
        std::string pre_pow = "1";
        //Go from the least bit up
        for(int j = binary_value.length() - 1; j >= 0; j--){
            //If the bit is set then calculate 2^n using the last calculated value
            if(binary_value[j] == '1'){
                for(int i = 0; i < pre_set_bit - j; i++){
                    pre_pow = add(pre_pow, pre_pow);
                }
                pre_set_bit = j;
                result = add(pre_pow, result);
            }
        }
        return result;
    }
    BigInt operator+(const BigInt& other){
        std::string result_binary_string = "";
        //Get two binary strings from two BigInts
        std::string binary_1 = this->binary();
        std::string binary_2 = other.binary();
        int i = binary_1.length() - 1;
        int j = binary_2.length() - 1;
        bool carry = false;
        while(i >= 0 | j >= 0 | carry == true){
            int sum = carry;
            carry = false;
            if(i >= 0) sum += binary_1[i--] - '0';
            if(j >= 0) sum += binary_2[j--] - '0';
            if(sum/2 == 0) carry = false;
            else carry = true;
            result_binary_string = std::to_string(sum%2) + result_binary_string;
        }
        return BigInt(result_binary_string);
    }
    bool isOdd(){
        return false;
    }
};
#endif