#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#define TERMNI_SIZE 3

using json = nlohmann::json;

static std::string terminator[3] = {"jmp", "br", "ret"};

struct Block{
   int id;
   bool fall_through;

   std::unordered_set<int> predcessors;
   std::unordered_set<int> successors;
   std::vector<json> instrs;

   Block(int new_id): id(new_id), fall_through(true){}

   //Getters
   size_t size(){ return instrs.size(); }
   bool empty(){ return instrs.empty(); }

   //Setters
   void clear();
   void setID(int new_id) { id = new_id; }

   //Member functions
   void LinkTo(Block &successor);
};

class BasicBlocks{
   using iterator = std::vector<Block>::iterator;
   using const_iter = std::vector<Block>::const_iterator;

 public:
   std::string function_name;
   std::vector<Block> blocks;
   BasicBlocks(const json &input);

   iterator begin(){ return blocks.begin(); }
   iterator end(){ return blocks.end(); }
   const_iter begin() const{ return blocks.begin(); }
   const_iter end() const { return blocks.end(); }
   size_t size() {return blocks.size(); }
   Block& operator[](unsigned int i){ return blocks[i]; }

   json &getArgs();

   json Dump();

 private:
   std::string return_type;
   json args;

   void AddInst(std::vector<Block> &blocks, Block &current_block, const json &inst);
};

class CFG{
 public:
    std::unordered_map<std::string, int> label_2_block;
    BasicBlocks blocks;

    CFG(const BasicBlocks &blocks): blocks(blocks){}

    void ExtractCFG();
    void Dump();

 private:
    std::unordered_map<std::string, std::vector<int>> fwd_decl_preds; //forward declared predcessors

    void AddInst(std::vector<Block> &blocks, Block &current_block, const json &inst,
                std::unordered_map<std::string, int> &label_2_block,
                std::unordered_map<std::string, std::vector<int>> &fwd_decl_preds);
    void Link2NewLabel(BasicBlocks &blocks, Block &current_block, const std::string label,
                        std::unordered_map<std::string, std::vector<int>> &fwd_decl_preds);
    void LinkJumps(BasicBlocks &blocks, Block &current_block, const json &labels,
                std::unordered_map<std::string, int> &label_2_block,
                std::unordered_map<std::string, std::vector<int>> &fwd_decl_preds);
};