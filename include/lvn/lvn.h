#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Value{
    std::string op;
    std::vector<std::string> args;
    std::string property;
    Value(std::string opcode, std::string arg, std::string property = "");
    Value(std::string opcode, json args_json, std::string property = "");

    bool operator==(const Value &other) const;

};

namespace std {
    template <>
    struct hash<Value> {
        std::size_t operator()(const Value& v) const {
            auto hash = std::hash<std::string>()(v.op) ^ std::hash<std::string>()(v.property);
            for(const auto &arg: v.args){
                hash = hash ^ std::hash<std::string>()(arg);
            }
            return hash;
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