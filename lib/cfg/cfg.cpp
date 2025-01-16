#include "cfg/cfg.h"

void Block::LinkTo(Block &successor){
    this->successors.insert(successor.id);
    successor.predcessors.insert(id);
}

void Block::clear(){
    fall_through = true;

    predcessors.clear();
    successors.clear();
    instrs.clear();
}

BasicBlocks::BasicBlocks(const json &input): functionName(input["name"]){
    Block current_block(blocks.size());
    for(const auto inst: input["instrs"]){
        AddInst(blocks, current_block, inst);
    }

    if(!current_block.empty()){
        current_block.fall_through = false;
        blocks.push_back(current_block);
    }
}

json BasicBlocks::dump(){
    json instrs = json::array();
    for(const auto &block: blocks){
        for(const auto &inst: block.instrs){
            instrs.push_back(inst);
        }
    }

    return json{{"name", functionName}, {"instrs", instrs}};
}


void BasicBlocks::AddInst(std::vector<Block> &blocks, Block &current_block, const json &inst){
    int id = current_block.id;
    if(!inst.contains("op")){ //case 1: found "label"
        //end previous block
        if(!current_block.empty()){
            blocks.push_back(current_block);
            current_block.clear();
            current_block.set_id(++id);
        }

        current_block.instrs.push_back(inst);
        return;
    }
    
    //case 2: normal instructions
    current_block.instrs.push_back(inst);

    //case 3: terminator
    const auto found = std::find(terminator, terminator + TERMNI_SIZE, inst["op"]);
    if(found != terminator + TERMNI_SIZE){ 
        //create new block
        current_block.fall_through = false;
        blocks.push_back(current_block);
        current_block.clear();
        current_block.set_id(++id);
    }
}

void CFG::ExtractCFG(){
    for(int i = 0; i < blocks.size(); i++){
        Block &current_block = blocks[i];
        if(current_block.instrs[0].contains("label")){
            std::string label = current_block.instrs[0]["label"];

            label_2_block.emplace(label, i);
            Link2NewLabel(blocks, current_block, label, fwd_decl_preds);
        }

        const auto &last_inst = current_block.instrs.back();
        if(last_inst.contains("label")) return;

        const auto found = std::find(terminator, terminator + TERMNI_SIZE, last_inst["op"]);
        if(found != terminator + TERMNI_SIZE){
            //link branches (but `ret` don't need to link)
            if(last_inst["op"] != "ret"){
                LinkJumps(blocks, current_block, last_inst["labels"], label_2_block, fwd_decl_preds);
            }
        }

        if(current_block.fall_through) current_block.LinkTo(blocks[i + 1]);
    }
}

void CFG::Dump(){
    for(const Block &block: blocks){
        std::cout << "block "<< block.id << ": " << std::endl;
        for(const auto &inst: block.instrs){
            std::cout << '\t' << inst << std::endl;
        }

        for(const auto &successor: block.successors){
            std::cout << '\t' << block.id << "--->" << successor << std::endl;
        }
    }
}

void CFG::Link2NewLabel(BasicBlocks &blocks, Block &current_block, const std::string label,
                        std::unordered_map<std::string, std::vector<int>> &fwd_decl_preds)
{
    if(fwd_decl_preds.count(label) == 0) return;

    for(int predIdx: fwd_decl_preds[label]){
        blocks[predIdx].LinkTo(current_block);
    }
    fwd_decl_preds.erase(label);
}

void CFG::LinkJumps(BasicBlocks &blocks, Block &current_block, const json &labels,
            std::unordered_map<std::string, int> &label_2_block,
            std::unordered_map<std::string, std::vector<int>> &fwd_decl_preds)
{
    int id = current_block.id;
    for(auto label: labels){
        //case 3-1: target block not exist yet
        if(label_2_block.count(label) == 0) { fwd_decl_preds[label].push_back(id); }
        else{ //case 3-2: have seen target block
            int targetIdx = label_2_block[label];
            current_block.LinkTo(blocks[targetIdx]);
        }
    }
}