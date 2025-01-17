#include "cfg/cfg.h"

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <nlohmann/json.hpp>

void DeadCodeElimination(BasicBlocks &blocks, bool simple_mode);
void DeleteUnusedCode(BasicBlocks &blocks, bool simple_mode);
void DeleteReAssigned(BasicBlocks &blocks);

int main(int argc, char** argv){
    json j;
    j = j.parse(std::cin);

    //perform dead code elimination, and output modified code
    bool simple_mode = (argc > 1);
    json outputFuncs = json::array();
    for(const auto &func: j["functions"]){
        BasicBlocks blocks(func);
        DeadCodeElimination(blocks, simple_mode);
        outputFuncs.push_back(std::move(blocks.dump()));
    }
    
    std::cout << json{{"functions", outputFuncs}} << std::endl;
    return 0;
}

void DeadCodeElimination(BasicBlocks &blocks, bool simple_mode){
    DeleteUnusedCode(blocks, simple_mode);
    DeleteReAssigned(blocks);
}

void DeleteUnusedCode(BasicBlocks &blocks, bool simple_mode){
    int changed = true;
    while(changed){
        //collect all usage
        std::unordered_set<std::string> used;
        for(const auto &block: blocks){
            for(const auto &inst: block.instrs){
                if(!inst.contains("args")) continue;
                for(std::string operand: inst["args"]){ used.insert(operand); }
            }
        }

        std::vector<std::vector<int>> redundent_lines(blocks.size());
        for(int i = 0; i < blocks.size(); i++){
            auto &instrs = blocks[i].instrs;
            for(int j = 0; j < instrs.size(); j++){
                if(!instrs[j].contains("dest")) continue;
                if(used.count(instrs[j]["dest"]) == 0){
                    redundent_lines[i].push_back(j);
                }
            }
        }

        changed = false;
        for(int i = 0; i < redundent_lines.size(); i++){
            auto &instrs = blocks[i].instrs;
            auto &lines = redundent_lines[i];
            for(auto riter = lines.rbegin(); riter != lines.rend(); riter++){
                instrs.erase(instrs.begin() + *riter);
                if(!simple_mode)changed = true;
            }
        }
    }
}

void DeleteReAssigned(BasicBlocks &blocks){
    for(Block& block: blocks){
        std::unordered_map<std::string, int> unused_var;
        std::vector<int> redundent_lines;

        auto &instrs = block.instrs;
        for(int i = 0; i < instrs.size(); i++){
            auto &inst = instrs[i];
            if(inst.contains("args")){
                for(const auto &arg: inst["args"]){
                    auto iter = unused_var.find(arg);
                    if(iter == unused_var.end()) continue;
                    unused_var.erase(iter);
                }
            }

            if(!inst.contains("dest")) continue;
            auto iter = unused_var.find(inst["dest"]);
            if(iter != unused_var.end()) redundent_lines.push_back(iter->second);
            
            unused_var[inst["dest"]] = i;
        }

        for(auto riter = redundent_lines.rbegin(); riter != redundent_lines.rend(); riter++){
            instrs.erase(instrs.begin() + *riter);
        }
    }
}