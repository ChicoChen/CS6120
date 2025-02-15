#include <iostream>
#include <set>
#include <map>
#include <nlohmann/json.hpp>

#include "cfg/cfg.h"
#include "lvn/lvn.h"

using json = nlohmann::json;

void LocalValueNumbering(BasicBlocks &blocks);
std::unordered_map<std::string, int> trackAssignments(const Block &block, std::unordered_map<std::string, std::string> &legacy_vars);

void NumberingValue(Block &block, ValueTable table,
                    std::unordered_map<std::string, int> &assignments,
                    const std::unordered_map<std::string, std::string> &legacy_vars);

void AddLegacyEntry(ValueTable &table, const std::unordered_map<std::string, std::string> &legacy_vars);
void AddLegacyLines(Block &block, const std::unordered_map<std::string, std::string> &legacy_vars);

void replaceArgs(json &instr, ValueTable &table);

std::string getLegacyName(std::string var);
std::string getNickname();

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
        std::unordered_map<std::string, std::string> legacy_vars;
        std::unordered_map<std::string, int> assignments = std::move(trackAssignments(block, legacy_vars));
        NumberingValue(block, table, assignments, legacy_vars);
    }
}

std::unordered_map<std::string, int> trackAssignments(const Block &block, std::unordered_map<std::string, std::string> &legacy_vars){
    std::unordered_map<std::string, int> assignments;
    for(const auto & instr: block.instrs){
        if(instr.contains("args")){
            for(const std::string &arg: instr["args"]){
                if(assignments.count(arg) != 0) continue;
                std::string type = "int";
                //TODO: type of an legacy variable might not be int.
                legacy_vars[arg] = type;
            }
        }
    
        if(instr.contains("dest")) assignments[instr["dest"]]++;
    }

    for(auto iter = legacy_vars.begin(); iter != legacy_vars.end(); ){
        if(assignments.count(iter->first) == 0){ iter = legacy_vars.erase(iter); }
        else iter++;
    }

    return assignments;
}

void AddLegacyEntry(ValueTable &table, const std::unordered_map<std::string, std::string> &legacy_vars){
    for(const auto &leg: legacy_vars){
        table.AddLegacyValue(leg.first, getLegacyName(leg.first));
    }
}

void AddLegacyLines(Block &block, const std::unordered_map<std::string, std::string> &legacy_vars){
    auto &instrs = block.instrs;
    auto insert_tagrget = instrs.begin();
    if(instrs[0].contains("label")) insert_tagrget++;
    for(const auto &leg: legacy_vars){
        json newline;
        newline["args"] = {leg.first};
        newline["dest"] = getLegacyName(leg.first);
        newline["op"] = "id";
        newline["type"] = leg.second; //TODO: the type might be int or bool, currently default int.
        insert_tagrget = instrs.insert(insert_tagrget, newline);
    }
}

void NumberingValue(Block &block, ValueTable table,
                    std::unordered_map<std::string, int> &assignments,
                    const std::unordered_map<std::string, std::string> &legacy_vars)
{
    AddLegacyEntry(table, legacy_vars);
    for(auto &instr: block.instrs){
        if(instr.contains("dest")){ //assignment, maybe create a new element
            std::string dest = instr["dest"];

            assignments[dest]--;
            if(instr["op"] == "id"){
                if(!table.contains(instr["args"][0])){
                    table.AddLegacyValue(instr["args"][0], instr["args"][0]);
                }
                std::string equivalent = table[instr["args"][0]];
                replaceArgs(instr, table);
                table[dest] = equivalent;
            }
            else{
                Value value = MakeValue(instr, table);
                replaceArgs(instr, table);
                if(table.contains(value)){ //value already exist
                    table[dest] = table[value];
                }
                else{ //found a new value
                    std::string equivalent = (assignments[dest] == 0)? dest: getNickname();
                    table.AddElement(value, dest, equivalent);
                    instr["dest"] = equivalent;
                }
            }
        }
        else if(instr.contains("args")){ //instruct with no dest like return, invoke void function, etc
            replaceArgs(instr, table);
        }
    }
    AddLegacyLines(block, legacy_vars);
}

void replaceArgs(json &instr, ValueTable &table){
    for(auto &arg: instr["args"]){
        if(!table.contains(arg)){
            continue;
        }

        arg = table[arg];
    }
}

std::string getNickname(){
    static int counter = 0; 
    counter++;
    return "$" + std::to_string(counter);
}

std::string getLegacyName(std::string var){
    return "$old_" + var;
}