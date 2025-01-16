#include <iostream>
#include "cfg/cfg.h"
#include <nlohmann/json.hpp>

int main(){
    json j;
    j = j.parse(std::cin);
    for(const auto &func: j["functions"]){
        BasicBlocks blocks(func);
        CFG cfg(blocks);
        cfg.ExtractCFG();
        cfg.Dump();
    }

    return 0;
}