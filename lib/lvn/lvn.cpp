#include "lvn/lvn.h"
int& ValueTable::operator[](const Value &value){
    return val2num[value]; 
}

int& ValueTable::operator[](const std::string var_name){
    return var2num[var_name]; 
}

bool ValueTable::contains(const Value &value){
    return val2num.count(value) != 0;
}

bool ValueTable::contains(const std::string &var_name){
    return var2num.count(var_name) != 0;
}  

Value makeValue(std::string opcode, std::string arg1, std::string arg2,
                ValueTable &table){
    int num1 = std::min(table[arg1], table[arg2]);
    int num2 = std::max(table[arg1], table[arg2]);
    return Value(opcode, num1, num2);
}

std::string num2name(int num){
    return "$" + std::to_string(num);
}

void ValueTable::addElement(const Value &value, const std::string &var_name){
    int index = val2num.size();
    val2num[value] = index;
    var2num[var_name] = index;
}
