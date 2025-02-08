#include <iostream>
#include <nlohmann/json.hpp>

#include "cfg/cfg.h"
#include "lvn/lvn.h"

using json = nlohmann::json;

void localValueNumbering(Block &block, ValueTable &table);

int main(){
    json j;
    j = j.parse(std::cin);

    json outputFuncs = json::array();
    for(auto &func: j["functions"]){
        BasicBlocks blocks(func);
        ValueTable table;
        if(func.contains("args")) {
            table.AddArgs(func["args"]);
            for(auto &arg: blocks.getArgs()){
                arg["name"] = Num2Name(table[arg["name"]]);
            }
        }

        for(auto &block: blocks){
            localValueNumbering(block, table);
        }
        outputFuncs.push_back(std::move(blocks.Dump()));
    }

    std::cout << json{{"functions", outputFuncs}} << std::endl;
    return 0;
}

void localValueNumbering(Block &block, ValueTable &table){
    for(auto &instr: block.instrs){
        if(instr.contains("dest")){ //assignment, maybe create a new element
            Value value = MakeValue(instr, table);
            auto dest = instr["dest"];
            if(table.contains(value)){ //value already exist
                int index = table[value];
                table[dest] = index;
                instr["dest"] = Num2Name(index);
            }
            else{ //found a new value
                table.AddElement(value, instr["dest"]);
                instr["dest"] = Num2Name(table[value]);
            }
        }

        if(instr.contains("args")){ //replace operand with table index
            for(auto &variable: instr["args"]){
                if(!table.contains(variable)){
                    std::cerr << "[ERROR]: arg not found, " << variable << std::endl;
                    continue;
                }
                variable = Num2Name(table[variable]);
            }
        }
    }
}