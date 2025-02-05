#pragma once

#include <string>
#include <unordered_map>

struct Value{
    std::string op;
    int arg1; //int: "12" or string name "$12"?
    int arg2;
    Value(std::string opcode, int arg1, int arg2): op(opcode), arg1(arg1), arg2(arg2) {}

    bool operator==(const Value &other) const {
        return op == other.op && arg1 == other.arg1 && arg2 == other.arg2;
    }

};

namespace std {
    template <>
    struct hash<Value> {
        std::size_t operator()(const Value& v) const {
            return std::hash<std::string>()(v.op) ^ std::hash<int>()(v.arg1) ^ std::hash<int>()(v.arg2);
        }
    };
}

Value makeValue(std::string opcode, std::string arg1, std::string arg2,
                ValueTable &table);
std::string num2name(int num);

class ValueTable{
 public:
    
    ValueTable() = default; 
    
    int& operator[](const std::string var_name);
    int& operator[](const Value &value);
    bool contains(const Value &value);
    bool contains(const std::string &var_name);

    void addElement(const Value &value, const std::string &var_name);

 private:
    std::unordered_map<Value, int> val2num;
    std::unordered_map<std::string, int> var2num;
};