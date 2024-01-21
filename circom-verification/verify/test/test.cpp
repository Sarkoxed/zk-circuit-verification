#include <circuit.hpp>
#include <iostream>

using namespace smt_circuit;

int main(int argc, char**argv){
    nlohmann::json circuit_info = open(argv[1]);

    SolverConfiguration config = { true, 0 };
    Solver s(circuit_info["modulus"], config, 10);
    
    auto C12 = unique_witness<FFTerm>(circuit_info, &s, {"main.c"});
    C12.first["main.c"] == "1000";
    for(auto assertion: s.s.getAssertions()){
        std::cout << assertion << std::endl;
    }

    std::cout << s.check() << std::endl;

    std::unordered_map<std::string, cvc5::Term> terms = {{"a_1", C12.first["main.a"]}, {"a_2", C12.second["main.a"]}, {"b_1", C12.first["main.b"]}, {"b_2", C12.second["main.b"]}, {"c1", C12.first["main.c"] }, {"c2", C12.second["main.c"]}};
    auto model = s.model(terms);
    
    std::cout << "a1 = " << model["a_1"] << std::endl;
    std::cout << "b1 = " << model["b_1"] << std::endl;
    std::cout << "c1 = " << model["c1"] << std::endl;

    std::cout << "a2 = " << model["a_2"] << std::endl;
    std::cout << "b2 = " << model["b_2"] << std::endl;
    std::cout << "c2 = " << model["c2"] << std::endl;
    return 0;
}
