#include <circuit.hpp>
#include <iostream>

using namespace smt_circuit;

int main(int argc, char**argv){
    nlohmann::json circuit_info = open(argv[1]);

    SolverConfiguration config = { true, 0 };
    Solver s(circuit_info["modulus"], config, 10);
    Circuit<FFTerm> circuit(circuit_info, &s);
    
    auto C12 = unique_witness<FFTerm>(circuit_info, &s, {"main.c"});
    std::cout << s.check() << std::endl;
    return 0;
}
