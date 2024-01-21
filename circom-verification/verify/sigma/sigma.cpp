#include <circuit.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace smt_circuit;

int main(int argc, char**argv){
    nlohmann::json circuit_info = open(argv[1]);

    SolverConfiguration config = { true, 0 };
    Solver s(circuit_info["modulus"], config, 10);
    
    
    std::vector<std::string> equal;
    for(size_t i = 0; i < 32; i++){
        equal.push_back(std::string(std::string("main.out[") + std::to_string(i) + std::string("]")));
        
    }
    auto C12 = unique_witness<FFTerm>(circuit_info, &s, equal);
    bool check = s.check();
    
    if (!check){
        std::cout << "witness is unique" << std::endl; 
    }else{
        std::unordered_map<std::string, cvc5::Term> terms;
        for(size_t i = 0; i < 32; i++){
            std::string varname = std::string("main.in[") + std::to_string(i) + std::string("]");
            terms[std::string("in1_") + std::to_string(i)] = C12.first[varname];
            terms[std::string("in2_") + std::to_string(i)] = C12.second[varname];
        }
        auto model = s.model(terms);
        
        for(size_t i = 0; i < 32; i++){
            std::cout << model[std::string("in1_") + std::to_string(i)] << ", ";
        }
        std::cout << std::endl;
        for(size_t i = 0; i < 32; i++){
            std::cout << model[std::string("in2_") + std::to_string(i)] << ", ";
        }
        std::cout << std::endl;
 
    }
    return 0;
}
