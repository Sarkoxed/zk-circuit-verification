#pragma once
#include <fstream>
#include <chrono>

#include "../include/circuit.hpp"

void default_model(const std::vector<std::string>& special,
                   smt_circuit::Circuit& c1,
                   smt_circuit::Circuit& c2,
                   smt_solver::Solver* s,
                   const std::string& fname = "witness.out");
void default_model_single(const std::vector<std::string>& special,
                          smt_circuit::Circuit& c,
                          smt_solver::Solver* s,
                          const std::string& fname = "witness.out");

bool smt_timer(smt_solver::Solver* s);
