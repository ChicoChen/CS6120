#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>

#define TERMNI_SIZE 3

using json = nlohmann::json;

static std::string terminator[3] = {"jmp", "br", "ret"};

struct Block{
    std::vector<Block*> predcessors;
    std::vector<Block*> successors;
    std::vector<json> instrs;

    void clear(){
        predcessors.clear();
        successors.clear();
        instrs.clear();
    }

    bool empty(){
        return instrs.empty();
    }
};

void addInst(std::vector<Block> &blocks, Block &currentBlock, const json &inst);

int main(){
    json j;
    j = j.parse(std::cin);

    std::vector<Block> blocks;
    for(const auto func: j["functions"]){
        Block currentBlock;
        for(const auto inst: func["instrs"]){
           addInst(blocks, currentBlock, inst);
        }
        if(!currentBlock.empty())blocks.push_back(currentBlock);
    }

    //dump result;
    for(const Block &block: blocks){
        std::cout << "block: " << std::endl;
        for(const auto &inst: block.instrs){
            std::cout << '\t' << inst << std::endl;
        }
    }

    return 0;
}

void addInst(std::vector<Block> &blocks, Block &currentBlock, const json &inst){
    auto opcode = inst.find("op");
    if(opcode == inst.end()){
        if(!currentBlock.empty()){
            blocks.push_back(currentBlock);
            currentBlock.clear();
        }
        currentBlock.instrs.push_back(inst);
        return;
    }

    currentBlock.instrs.push_back(inst);
    const auto found = std::find(terminator, terminator + TERMNI_SIZE, inst["op"]);
    if(found != terminator + TERMNI_SIZE){ //terminator
        blocks.push_back(currentBlock);
        currentBlock.clear();
    }
}
