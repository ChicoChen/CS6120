#include "cfg/cfg.h"

#include <iostream>
#include <set>
#include <unordered_map>
#include <nlohmann/json.hpp>

void DeadCodeEliminate(Block &block);

int main(){
    json j;
    j = j.parse(std::cin);

    //for each block, do dead code elimination, and output modified code
    json outputFuncs = json::array();
    for(const auto &func: j["functions"]){
        BasicBlocks blocks(func);
        for(Block &block: blocks){
            size_t codeSize;
            do{
                codeSize = block.instrs.size();
                DeadCodeEliminate(block);
            }while(codeSize != block.instrs.size());
        }
        outputFuncs.push_back(std::move(blocks.dump()));
    }
    
    std::cout << json{{"functions", outputFuncs}} << std::endl;
    return 0;
}

void DeadCodeEliminate(Block &block){
    std::unordered_map<std::string, std::vector<int>> unusedInst;

    for(int i = 0; i < block.instrs.size(); i++){
        const auto &inst = block.instrs[i];
        if(inst.contains("args")){
            for(const auto &arg : inst["args"]){
                if(unusedInst[arg].empty()) continue;
                unusedInst[arg].pop_back();
            }
        }
        
        if(!inst.contains("dest")) continue;
        unusedInst[inst["dest"]].push_back(i);
    }

    std::set<int> unusedLine;
    for(const auto &unused: unusedInst){
        for(const int &instIdx: unused.second){
            unusedLine.insert(instIdx);
        }
    }

    for(auto iter = unusedLine.rbegin(); iter != unusedLine.rend(); iter++){
        block.instrs.erase(block.instrs.begin() + *iter);
    }
}