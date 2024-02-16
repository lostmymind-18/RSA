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
Each number have a multiplication of 32 bits
For optimization of storage, if a number have a leading 1, it means the number is a negative number
    then, we will cut the number to its first 0 occurrence, and then buffer with 1s
    If a number have a leading 0, it means the number is a positive number
    then, we will cut the number to its first 1 occurrence, and then buffer it with 0s
*/
class BigInt{
private:
    std::vector<int> content;
    /*This is a secondary function that add two string numbers in 10-base form*/
    std::string add(const std::string& a, const std::string& b){
        std::string result = "";
        int i = a.length() - 1;
        int j = b.length() - 1;
        bool carry = false;
        while(i >= 0 | j >= 0 | carry){
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
    /*This is a secondary function used to compact the representation of the number
    It will cut leading ones of negative number and leading zeros of positive ones
    */    
    void compact(){
        //TODO
    }
    /*This is a secondary function used to return two's complement binary
    for this number*/
    std::string binary_complement()const{
        //get binary representation
        std::string result = binary();
        //inverse every bit
        for(int i = 0; i < result.length(); i++)
            result[i] = (result[i] == '0')? '1':'0';
        //add one
        bool carry = true; //Carry is true because we're adding one
        for(int i = result.length() - 1; i >= 0 && carry == true; i --){
            int sum = carry;
            sum += result[i] - '0';
            result[i] = (sum%2 == 0)? '0':'1';
            carry = sum/2;
        }
        return result;
    }
public:
    BigInt(){}
    BigInt(std::string bit_string){
        //Determine the leading bit
        char leading_bit = bit_string[0];
        //Get index of the most value bit (first bit that is not the same as the leading bit)
        int most_index = 0;
        while(bit_string[most_index] == leading_bit)
            most_index ++;
        //Cut the string from that index
        bit_string = bit_string.substr(most_index);
        //Add 1 leading bit at the beginning of the string
        bit_string = leading_bit + bit_string;
        //Count number of elements in a vector needed for representing the string
        int num_elements_needed = (bit_string.size() + BITS_IN_ELEMENT - 1)/BITS_IN_ELEMENT;
        //Buffing the string
        int string_length = bit_string.length();
        for(int i = 0; i < num_elements_needed*BITS_IN_ELEMENT - string_length; i++){
            bit_string = leading_bit + bit_string;
        }
        //Resize content
        content.resize(num_elements_needed,0);
        //Convert binary to integer for each element
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
    /*This function used to set a bit at a given position
    If pos == 0 means the most value bit will be set (0000 --> 1000)
    So, the index counting start at the most value bit of a value*/
    void setBit(int pos){
        if(pos >= content.size() * BITS_IN_ELEMENT) throw "Out of range";
        int content_index = pos/BITS_IN_ELEMENT;
        int set_off = BITS_IN_ELEMENT - pos%BITS_IN_ELEMENT - 1;
        content[content_index] = content[content_index] | (1 << set_off);
    }
    std::string binary()const{
        std::string result = "";
        for(int i = 0; i < this->content.size(); i++){
            std::bitset<sizeof(int)*8> binary(content[i]);
            result = result + binary.to_string();
        }
        return result;
    }    
    std::string decimal(){
        bool isNeg = isNegative();
        std::string result = "0";
        //If it's a negative number then we have to converse it to the positive form
        std::string binary_value = (isNeg == true)? binary_complement() : binary();
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
        return (isNeg == true)? '-' + result : result;
    }
    /*This is a secondary add two binary string, there are some rules:
    1. In any case, the carry flag will be ignored after the procedure finished.
    2. When two number that has the same sign (both positive or negative) is added,
    there is a chance that we need to expand the number of bits to fit for the result.
    3. For the remaining cases, the procedure is just adding bit by bit.
    4. Before the procedure starts, make sure the two number have the same number
    of bits. We do that by adding a sequence of leading at the start of the
    number that have smaller number of bits.
    5. Before the procedure starts, make sure the binary representations of the two
    number have the number of bits that is multiple of BITS_IN_ELEMENT, so that the
    result will be easier to handle.
    */
    BigInt operator+(const BigInt& other){
        std::string result_binary_string = "";
        //Get two binary strings from two BigInts
        std::string binary_1 = this->binary();
        std::string binary_2 = other.binary();
        /*Check if the two have the same sign, if it's true,
        set the leading bit*/
        char leading_same = ' ';
        char leading_bit_1 = binary_1[0];
        char leading_bit_2 = binary_2[0];
        if(leading_bit_1 == leading_bit_2)
            leading_same = leading_bit_1;
        /*Perform buffing if two number don't have the same length
        which mean, add leading bits at the beginning of the smaller in length ones
        */
        if(binary_1.length() != binary_2.length()){
            std::string buffed = (binary_1.length() < binary_2.length())? binary_1:binary_2;
            char leading_buffed = buffed[0];
            int num_leading_buffed = binary_1.length() - binary_2.length();
            if(num_leading_buffed < 0)
                num_leading_buffed -= num_leading_buffed;
            for(int i = 0; i < num_leading_buffed; i++)
                buffed = leading_buffed + buffed; 
            if(binary_1.length() < binary_2.length())
                binary_1 = buffed;
            else binary_2 = buffed;
        }
        //Perform adding two binary string has the same length
        bool carry = false;
        for(int i = binary_1.length() - 1; i >= 0; i--){
            int sum = carry;
            carry = false;
            sum += binary_1[i] - '0';
            sum += binary_2[i] - '0';
            if(sum/2 == 0) carry = false;
            else carry = true;
            result_binary_string = std::to_string(sum%2) + result_binary_string;
        }
        /*If the leading_same is set, and the leading bit of the result is not the same
            then perform bit expanding*/
        if(leading_same != ' ' && result_binary_string[0] != leading_same){
            /*Perform bit expanding by appending BITS_IN_ELEMENT of leading_same
                at the beginning of the result*/
            for(int i = 0; i < BITS_IN_ELEMENT; i++)
                result_binary_string = leading_same + result_binary_string;
        }
        return BigInt(result_binary_string);
    }
    BigInt operator-(const BigInt& other){
        return operator+(BigInt(other.binary_complement()));
    }
    /*Ta sẽ thực hiện hàm so sánh bằng như thế nào?
    * Với điều kiện là một BigInt luôn được biểu diễn dưới dạng gọn gàng nhất, thì 2 BigInt bằng nhau
    * chỉ khi tất cả các bit của chúng giống hệt nhau
    * => Ta phải thực hiện hàm compact, giúp cho việc biểu diễn các số được gọn gàng nhất.
    */
    bool operator==(const BigInt& other){

    }
    /*Ta sẽ thực hiện phép nhân như thế nào?
        * Bản chất của phép nhân là thực hiện hàng loạt phép cộng, vậy nên ta sẽ
        thực hiện dựa trên phép cộng
        * Nếu nhân với số có dấu âm thì sao?
            * Thì ta sẽ chuyển dấu của số đó sang cho số đầu tiên
            * Và thực hiện phép nhân với số dương
        * Thực hiện phép nhân với số dương như thế nào?
        *   Ta sẽ thực hiện hàng loạt phép cộng
        *   Điều đó có nghĩa cần phải có một vòng lặp
        *   Để vòng lặp dừng, thì phải có một hàm so sánh 2 BigInt
        *   => Ta phải thực hiện thêm một hàm so sánh bằng
    */
    BigInt operator*(const BigInt& other){

    }
    // BigInt operator/(const BigInt& other){

    // }
    // BigInt mod(const BigInt& other){

    // }
    bool isOdd(){
        return false;
    }
    //Check if the number is negative or positive
    bool isNegative(){
        return (content[0] < 0)? true:false;
    }
};
#endif