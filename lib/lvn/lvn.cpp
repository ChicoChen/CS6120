#include "lvn/lvn.h"
Value::Value(std::string opcode, std::string arg, std::string property)
            :op(opcode), property(property)
{
    args.push_back(arg);
}

Value::Value(std::string opcode, json args_json, std::string property)
            :op(opcode), property(property)
{
    for(std::string arg: args_json){
        args.push_back(arg);
    }
}

bool Value::operator==(const Value &other) const {
    if(op != other.op || property != other.property) return false;
    else if(args.size() != other.args.size()) return false;

    for(int i = 0; i < args.size(); i++){
        if(args[i] != other.args[i]) return false;
    }
    return true;
}

std::string& ValueTable::operator[](const Value &value){
    return val2name[value]; 
}

std::string& ValueTable::operator[](const std::string &var_name){
    return var2name[var_name]; 
}

bool ValueTable::contains(const Value &value){
    return val2name.count(value) != 0;
}

bool ValueTable::contains(const std::string &var_name){
    return var2name.count(var_name) != 0;
}  

void ValueTable::AddElement(const Value &value, const std::string &variable, const std::string &name){
    val2name[value] = name;
    var2name[variable] = name;
}

void ValueTable::AddArgs(const json &args){
    for(int i = 0; i < args.size(); i++){
        std::string arg_name = args[i]["name"];
        Value value("arg", arg_name, std::to_string(i));

        val2name[value] = arg_name;
        var2name[arg_name] = arg_name;
    }
}

void ValueTable::AddLegacyValue(std::string var, std::string name){
    Value value("declared", var);
    val2name[value] = name;
    var2name[var] = name;
}

Value MakeValue(const json &instr, ValueTable &table){
    std::string opcode = instr.value("op", "");

    if(opcode == "const"){
        int val = instr["value"];
        return Value("const", std::to_string(val), instr["type"]);
    }
    else if(opcode == "id"){
        return Value("id", instr["args"][0]);
    }

    
    if(instr.contains("args")){
        json args = instr["args"];
        std::string property = "";
        for(auto &arg: args){
            if(!table.contains(arg)) table.AddLegacyValue(arg, arg);
            arg = table[arg];
        }

        if(opcode == "add" || opcode == "mul") std::sort(args.begin(), args.end());
        else if(opcode == "call") property = instr["funcs"][0];

        return Value(instr["op"], args, property);
    }
    else{
        std::cerr << "[ERROR]: unrecongnized instruction when makeValue(): " << instr << std::endl;
        return Value("Error", static_cast<std::string>(""));
    }
}
