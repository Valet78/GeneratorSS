#include<iostream>
#include<ctime>
#include<sstream>
#include<cmath>
#include <algorithm>


int main () {
    std::string firstWord = "5999500000";
    int numPoint = 12, valuePoint = 121;
    
    std::string valueFW [] = {"149900", "29998500", "999900000", "5999500000", "5999400000"};
    bool ex = std::count(std::begin(valueFW), std::end(valueFW), firstWord);
    if (!(ex && numPoint == valuePoint)) { // != std::string.npos) {
        
        std::cout << "no! " << std::boolalpha << !ex << std::endl;
    } else std::cout << "yes! " << std::boolalpha << ex << std::endl;


    std::cout << "findPos = " << *std::find(std::begin(valueFW), std::end(valueFW), firstWord) << std::endl;
        
    return 0;
}


