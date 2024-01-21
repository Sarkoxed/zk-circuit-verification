#include <circuit.hpp>
#include <iostream>

using namespace smt_circuit;

int main(int argc, char**argv){
    nlohmann::json circuit_info = open(argv[1]);

    SolverConfiguration config = { true, 0 };
    Solver s(circuit_info["modulus"], config, 10);
    
    auto C12 = unique_witness<FFTerm>(circuit_info, &s, {"something in circuit"});
    std::cout << s.check() << std::endl;

    std::unordered_map<std::string, cvc5::Term> terms = {"smth1", C12.first["smth"], {"smth2", C12.second["smth"]}};
    auto model = s.model(terms);
    
    std::cout << model["smth1"] << std::endl;
    std::cout << model["smth2"] << std::endl;
    return 0;
}
