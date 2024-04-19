#include "circuit_schema.hpp"

namespace smt_circuit_schema {

///**
// * @brief Translates the schema to python format
// * @details Returns the contents of the .py file
// * that can be further imported by python script
// *
// * @example output.py
// * variables = ["zero", "one", "my_input1", "var3", ..., "var_n"] - variable
// * names public = [[0, 0x000...0], [1, 0x000...1], [2, 0x000..abcd]] - (index,
// * value) gates = [
// *  [[0x000...0, 0x000...1, 0x000...0, 0x000...0, 0x000...0], [0, 0, 0]], ...
// * ]
// */
//void print_schema_for_use_in_python(CircuitSchema &cir) {
//  std::cout << "variable_names = [" << std::endl;
//  for (uint32_t i = 0; i < static_cast<uint32_t>(cir.variables.size()); i++) {
//    if (cir.vars_of_interest.contains(i)) {
//      std::cout << '"' << cir.vars_of_interest[i] << "\"," << std::endl;
//    } else {
//      std::cout << "\"v" << i << "\"," << std::endl;
//    }
//  }
//  std::cout << "]" << std::endl;
//  std::cout << "public = [" << std::endl;
//  for (auto i : cir.public_inps) {
//    std::cout << "[" << i << ", " << cir.variables[i] << "]," << std::endl;
//  }
//  std::cout << "]" << std::endl;
//  std::cout << "gates = [" << std::endl;
//
//  for (size_t i = 0; i < cir.selectors.size(); i++) {
//    std::cout << "["
//              << "[" << cir.selectors[i][0] << cir.selectors[i][1] << ", "
//              << cir.selectors[i][2] << ", " << cir.selectors[i][3] << ", "
//              << cir.selectors[i][4] << "], [" << cir.wires[i][0] << ", "
//              << cir.wires[i][1] << ", " << cir.wires[i][2] << "]],"
//              << std::endl;
//  }
//  std::cout << "]" << std::endl;
//}

nlohmann::json open(const std::string& filename){
    std::ifstream fin;
    fin.open(filename);
    if(!fin.is_open()){
        throw std::invalid_argument("no such a file");
    }
    nlohmann::json res;
    fin >> res;
    return res;
}

} // namespace smt_circuit_schema
