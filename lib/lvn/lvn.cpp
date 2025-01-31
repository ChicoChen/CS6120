#include "lvn/lvn.h"
std::string ValueTable::operator[](const Value &value){
    return num2name(val2num[value]); 
}

std::string ValueTable::operator[](const std::string var_name){
    return num2name(var2num[var_name]); 
}

bool ValueTable::contains(const Value &value){
    return val2num.count(value) != 0;
}

bool ValueTable::contains(const std::string &var_name){
    return var2num.count(var_name) != 0;
}  

void ValueTable::addElement(const Value &value, const std::string &var_name){

}

std::string ValueTable::num2name(int num){

}