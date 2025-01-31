#pragma once

#include <string>
#include <unordered_map>
struct Value{
    std::string op;
    int arg1; //int: "12" or string name "$12"?
    int arg2;
    Value(std::string opcode, int arg1, int arg2): op(opcode), arg1(arg1), arg2(arg2) {}
};

class ValueTable{
 public:
    
    ValueTable() = default; 
    
    std::string operator[](const Value &value);
    std::string operator[](const std::string var_name);
    bool contains(const Value &value);
    bool contains(const std::string &var_name);

    void addElement(const Value &value, const std::string &var_name);

 private:
    std::unordered_map<Value, int> val2num;
    std::unordered_map<std::string, int> var2num;
    std::string num2name(int num);
};