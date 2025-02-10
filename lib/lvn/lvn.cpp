#include "lvn/lvn.h"
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

std::string ValueTable::getNickname(){
    return "$" + std::to_string(val2name.size());
}

Value MakeValue(const json &instr, ValueTable &table){
    if(instr["op"] == "const"){
        int val = instr["value"];
        return Value("const", std::to_string(val), "");
    }
    else if(instr.contains("args") && instr["args"].size() == 2){
        std::string arg1 = instr["args"][0];
        std::string arg2 = instr["args"][1];

        arg1 = table[arg1];
        arg2 = table[arg2];
        return Value(instr["op"], std::min(arg1, arg2), std::max(arg1, arg2));
    }
    else{
        std::cerr << "[ERROR]: unrecongnized instruction when makeValue(): " << instr << std::endl;
        return Value("Error", "", "");
    }
}
