#include <iostream>
#include <nlohmann/json.hpp>

#include "cfg/cfg.h"
#include "lvn/lvn.h"

void localValueNumbering(Block &block);

int main(){
    json j;
    j.parse(std::cin);

    json outputFuncs = json::array();
    for(auto &func: j["functions"]){
        BasicBlocks blocks(func);
        for(auto &block: blocks){
            localValueNumbering(block);
        }
        outputFuncs.push_back(std::move(blocks.dump()));
    }

    std::cout << json{{"functions", outputFuncs}} << std::endl;
    return 0;
}

void localValueNumbering(Block &block){
    /********* pesudo Code
    for each function:
        for each block:
            val2num = a table that map value to number
            num2var = vector of canonical variable name
            var2num
            getCaconical = from any var name, return to its canonical alternative name.
            for instr in that block:
                if instr is assignment:
                    value = (OP, var2num[argu1], var2num[argu2])
                    dest = instr.dest
                    if(dest is used before): //this is a re-assignment
                        dest needed to be given a new name
                        modify getCanonical to get new name from dest.

                    if value in value_table:
                        //find reuse opportunity

                    else:
                        add_new_element(valueTable, value);

    **********/
   ValueTable table;
    for(auto &instr: block.instrs){
        if(instr.contains("dest")){ // assignment, maybe create a new element
            Value value = makeValue(instr["opcode"], instr["args"][0], instr["args"][1], table);
            auto dest = instr["dest"];
            if(table.contains(value)){ // value already exist
                int index = table[value];
                table[dest] = index;
            }
            else{ // found a new value
                table.addElement(value, instr["dest"]);
            }
        }
        else if(instr.contains("args")){ // replace operand with table index
            for(auto &variable: instr["args"]){
                if(!table.contains(variable)){
                    std::cerr << "[ERROR]: arg not found, " << variable << std::endl;
                    continue;
                }

                variable = num2name(table[variable]);
            }
        }
    }
}