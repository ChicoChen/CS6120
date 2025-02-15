#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Value{
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string property = "";
    Value(std::string opcode, std::string arg1, std::string arg2): op(opcode), arg1(arg1), arg2(arg2) {}
    Value(std::string opcode, std::string arg1, std::string arg2, std::string property):
        op(opcode), arg1(arg1), arg2(arg2), property(property) {}

    bool operator==(const Value &other) const {
        return op == other.op && arg1 == other.arg1 && arg2 == other.arg2;
    }

};

namespace std {
    template <>
    struct hash<Value> {
        std::size_t operator()(const Value& v) const {
            return std::hash<std::string>()(v.op) ^ std::hash<std::string>()(v.arg1) ^ std::hash<std::string>()(v.arg2);
        }
    };
}

class ValueTable{
 public:
    
    ValueTable() = default; 
    
    std::string& operator[](const Value &value);
    std::string& operator[](const std::string &var_name);
    bool contains(const Value &value);
    bool contains(const std::string &var_name);

    void AddElement(const Value &value, const std::string &variable, const std::string &name);
    void AddArgs(const json &args);
    void AddLegacyValue(std::string var, std::string name);

 private:
    std::unordered_map<Value, std::string> val2name;
    std::unordered_map<std::string, std::string> var2name;

};

Value MakeValue(const json &instr, ValueTable &table);
// std::string Num2Name(int num);