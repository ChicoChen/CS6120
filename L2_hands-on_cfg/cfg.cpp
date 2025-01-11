#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#define TERMNI_SIZE 3

using json = nlohmann::json;

static std::string terminator[3] = {"jmp", "br", "ret"};

struct Block{
    int mID;
    bool fallThrough = true;

    std::unordered_set<int> predcessors;
    std::unordered_set<int> successors;
    std::vector<json> instrs;

    void Init(int newID){
        mID = newID;
        fallThrough = true;

        predcessors.clear();
        successors.clear();
        instrs.clear();
    }

    bool Empty(){
        return instrs.empty();
    }

    void LinkTo(Block &successor){
        this->successors.insert(successor.mID);
        successor.predcessors.insert(mID);
    }
};

void AddInst(std::vector<Block> &blocks, Block &currentBlock, const json &inst,
            std::unordered_map<std::string, int> &label2Block,
            std::unordered_map<std::string, std::vector<int>> &fwdDeclPreds);

void Link2NewLabel(std::vector<Block> &blocks, Block &currentBlock,
                    const std::string label,
                    std::unordered_map<std::string, std::vector<int>> &fwdDeclPreds);

void LinkJumps(std::vector<Block> &blocks, Block &currentBlock, const json &labels,
            std::unordered_map<std::string, int> &label2Block,
            std::unordered_map<std::string, std::vector<int>> &fwdDeclPreds);

int main(){
    json j;
    j = j.parse(std::cin);

    std::vector<Block> blocks;
    std::unordered_map<std::string, int> label2Block;
    std::unordered_map<std::string, std::vector<int>> fwdDeclPreds; //forward declared predcessors

    for(const auto func: j["functions"]){

        Block currentBlock;
        currentBlock.Init(blocks.size());
        for(const auto inst: func["instrs"]){
           AddInst(blocks, currentBlock, inst, label2Block, fwdDeclPreds);
        }
        if(!currentBlock.Empty())blocks.push_back(currentBlock);
    }

    //dump result;
    for(const Block &block: blocks){
        std::cout << "block "<< block.mID << ": " << std::endl;
        for(const auto &inst: block.instrs){
            std::cout << '\t' << inst << std::endl;
        }

        for(const auto &successor: block.successors){
            std::cout << '\t' << block.mID << "--->" << successor << std::endl;
        }
    }

    return 0;
}

void AddInst(std::vector<Block> &blocks, Block &currentBlock, const json &inst,
            std::unordered_map<std::string, int> &label2Block,
            std::unordered_map<std::string, std::vector<int>> &fwdDeclPreds)
{
    auto opcode = inst.find("op");
    int id = currentBlock.mID;
    if(opcode == inst.end()){ //case 1: found "label"
        //end previous block
        if(!currentBlock.Empty()){
            blocks.push_back(currentBlock);
            currentBlock.Init(++id);
        }

        //update label2Block
        std::string label = inst["label"];
        label2Block.emplace(label, id);
        
        //link edges
        if(blocks.back().fallThrough){ blocks.back().LinkTo(currentBlock); }
        Link2NewLabel(blocks, currentBlock, label, fwdDeclPreds);

        currentBlock.instrs.push_back(inst);
        return;
    }
    
    //case 2: normal instructions
    currentBlock.instrs.push_back(inst);

    const auto found = std::find(terminator, terminator + TERMNI_SIZE, inst["op"]);
    if(found != terminator + TERMNI_SIZE){ //case 3: terminator
        //link edges        
        if(inst.size() != 1) {
            LinkJumps(blocks, currentBlock, inst["labels"], label2Block, fwdDeclPreds);   
        }

        //create new block
        currentBlock.fallThrough = false;
        blocks.push_back(currentBlock);
        currentBlock.Init(++id);
    }
}

void Link2NewLabel(std::vector<Block> &blocks, Block &currentBlock,
                    const std::string label,
                    std::unordered_map<std::string, std::vector<int>> &fwdDeclPreds)
{
    if(fwdDeclPreds.count(label) == 0) return;

    for(int predIdx: fwdDeclPreds[label]){
        blocks[predIdx].LinkTo(currentBlock);
    }
    fwdDeclPreds.erase(label);
}


void LinkJumps(std::vector<Block> &blocks, Block &currentBlock, const json &labels,
            std::unordered_map<std::string, int> &label2Block,
            std::unordered_map<std::string, std::vector<int>> &fwdDeclPreds)
{
    int id = currentBlock.mID;
    for(auto label: labels){
        //case 3-1: target block not exist yet
        if(label2Block.count(label) == 0) { fwdDeclPreds[label].push_back(id); }
        else{ //case 3-2: have seen target block
            int targetIdx = label2Block[label];
            currentBlock.LinkTo(blocks[targetIdx]);
        }
    }
}