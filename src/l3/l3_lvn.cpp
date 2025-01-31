#include <iostream>
#include <nlohmann/json.hpp>

#include "cfg/cfg.h"
#include "lvn/lvn.h"

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

int main(){
    json j;
    j.parse(std::cin);

    for(auto &func: j["functions"]){
        BasicBlocks blocks(func);
        //todo: done things in each block
        
    }

    return 0;
}