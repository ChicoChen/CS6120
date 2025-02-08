#include "lvn/lvn.h"
int& ValueTable::operator[](const Value &value){
    return val2num[value]; 
}

int& ValueTable::operator[](const std::string &var_name){
    return var2num[var_name]; 
}

bool ValueTable::contains(const Value &value){
    return val2num.count(value) != 0;
}

bool ValueTable::contains(const std::string &var_name){
    return var2num.count(var_name) != 0;
}  

void ValueTable::AddArgs(const json &args){
    for(int i = 0; i < args.size(); i++){
        std::string arg_name = args[i]["name"];
        Value value(arg_name, -1, i);

        int index = val2num.size();
        val2num[value] = index;
        var2num[arg_name] = index;
    }
}

void ValueTable::AddElement(const Value &value, const std::string &var_name){
    int index = val2num.size();
    val2num[value] = index;
    var2num[var_name] = index;
}

Value MakeValue(const json &instr, ValueTable &table){
    if(instr["op"] == "const"){
        return Value("const", instr["value"], -1);
    }
    else if(instr.contains("args") && instr["args"].size() == 2){
        std::string arg1 = instr["args"][0];
        std::string arg2 = instr["args"][1];
        int num1 = table[arg1], num2 = table[arg2];
        return Value(instr["op"], std::min(num1, num2), std::max(num1, num2));
    }
    else{
        std::cerr << "[ERROR]: unrecongnized instruction when makeValue(): " << instr << std::endl;
        return Value("Error", -1, -1);
    }
}

std::string Num2Name(int num){
    return "$" + std::to_string(num);
}
