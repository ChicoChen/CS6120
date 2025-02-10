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

void ValueTable::AddLegacyValue(std::string var_name){
    Value value("declared", var_name, "");
    val2name[value] = var_name;
    var2name[var_name] = var_name;
}

Value MakeValue(const json &instr, ValueTable &table){
    //need also to conside case: {"args":["cond_temp"],"dest":"v15","op":"not","type":"bool"}
    std::string opcode = instr.value("op", "");

    if(opcode == "const"){
        int val = instr["value"];
        return Value("const", std::to_string(val), "");
    }
    
    if(instr.contains("args")){
        std::string arg1 = instr["args"][0];
        if(!table.contains(arg1)) table.AddLegacyValue(arg1);
        arg1 = table[arg1];

        if(instr["args"].size() == 2){ //binary operaion
            std::string arg2 = instr["args"][1];
            if(!table.contains(arg2)) table.AddLegacyValue(arg2);
            arg2 = table[arg2];
            return Value(instr["op"], std::min(arg1, arg2), std::max(arg1, arg2));
        }
        else{ //unary operaion
            return Value(instr["op"], arg1, "");
        }
    }
    else{
        std::cerr << "[ERROR]: unrecongnized instruction when makeValue(): " << instr << std::endl;
        return Value("Error", "", "");
    }
}
