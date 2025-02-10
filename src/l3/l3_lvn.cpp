#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

#include "cfg/cfg.h"
#include "lvn/lvn.h"

using json = nlohmann::json;

void LocalValueNumbering(BasicBlocks &blocks);
std::unordered_map<std::string, int> trackAssignments(const Block &block);
void NumberingValue(Block &block, ValueTable table,
                    std::unordered_map<std::string, int> &assignments);
void replaceArgs(json &instr, ValueTable &table);

int main(){
    json j;
    j = j.parse(std::cin);

    json outputFuncs = json::array();
    for(auto &func: j["functions"]){
        BasicBlocks blocks(func);
        LocalValueNumbering(blocks);
        outputFuncs.push_back(std::move(blocks.Dump()));
    }

    std::cout << json{{"functions", outputFuncs}} << std::endl;
    return 0;
}

void LocalValueNumbering(BasicBlocks &blocks){
    ValueTable table;

    if(blocks.hasArgs()){
        auto &args = blocks.getArgs();
        table.AddArgs(args);
    }

    for(auto &block: blocks){
        std::unordered_map<std::string, int> assignments = std::move(trackAssignments(block));
        NumberingValue(block, table, assignments);
    }
}

std::unordered_map<std::string, int> trackAssignments(const Block &block){
    std::unordered_map<std::string, int> assignments;
    std::unordered_set<std::string> legacies;
    for(const auto & instr: block.instrs){
        //TODO: handle legacy variables
        // if(instr.contains("args")){
        //     for(const std::string &arg: instr["args"]){
        //         if(assignments.count(arg) == 0){
        //             legacies.insert(arg);
        //         }
        //     }
        // }
        if(instr.contains("dest")) assignments[instr["dest"]]++;
    }

    return assignments;
}

void NumberingValue(Block &block, ValueTable table,
                    std::unordered_map<std::string, int> &assignments)
{
    for(auto &instr: block.instrs){
        if(instr.contains("dest")){ //assignment, maybe create a new element
            std::string dest = instr["dest"];

            assignments[dest]--;
            if(instr["op"] == "id"){
                if(!table.contains(instr["args"][0])){
                    table.AddLegacyValue(instr["args"][0]);
                }
                std::string equivalent = table[instr["args"][0]];
                replaceArgs(instr, table);
                table[dest] = equivalent;
            }
            else{
                Value value = MakeValue(instr, table);
                replaceArgs(instr, table);
                if(table.contains(value)){ //value already exist
                    std::string equivalent = table[value];
                    table[dest] = equivalent;
                }
                else{ //found a new value
                    std::string equivalent = (assignments[dest] == 0)? dest: table.getNickname();
                    table.AddElement(value, dest, equivalent);
                    instr["dest"] = equivalent;
                }
            }
        }
        else if(instr.contains("args")){ //instruct with no dest like return, invoke void function, etc
            replaceArgs(instr, table);
        }
    }
}

void replaceArgs(json &instr, ValueTable &table){
    for(auto &arg: instr["args"]){
        if(!table.contains(arg)){
            std::cerr << "[ERROR]: can't find key " << arg.get<std::string>()
                        << " in ValueTable" << std::endl;
            continue;
        }

        arg = table[arg];
    }
}