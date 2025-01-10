#include <iostream>
#include <nlohmann/json.hpp>

#define TERMNI_SIZE 3

using json = nlohmann::json;

static std::string terminator[3] = {"jmp", "br", "ret"};

int main(){
    json j;
    j = j.parse(std::cin);
    for(const auto func: j["functions"]){
        for(const auto inst: func["instrs"]){
            const auto found = std::find(terminator, terminator + TERMNI_SIZE, inst["op"]);
            if(found == terminator + TERMNI_SIZE){
                std::cout << inst.dump() << std::endl;
            }
            else{
                std::cout << "found terminator" << std::endl;
            }
        }
    }

    return 0;
}