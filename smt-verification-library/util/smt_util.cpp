#include "smt_util.hpp"

/**
 * @brief Get pretty formatted result of the solver work
 *
 * @details Having two circuits and defined constraint system
 * inside the solver get the pretty formatted output.
 * The whole witness will be saved in c-like array format.
 * Special variables will be printed to stdout. e.g. `a_1, a_2 = val_a_1, val_a_2;`
 *
 * @param special The list of variables that you need to see in stdout
 * @param c1 first circuit
 * @param c2 the copy of the first circuit with changed tag
 * @param s  solver
 * @param fname file to store the resulting witness if succeded
 */
void default_model(const std::vector<std::string>& special,
                   smt_circuit::Circuit& c1,
                   smt_circuit::Circuit& c2,
                   smt_solver::Solver* s,
                   const std::string& fname)
{
    std::vector<cvc5::Term> vterms1;
    std::vector<cvc5::Term> vterms2;
    vterms1.reserve(c1.get_num_vars());
    vterms2.reserve(c1.get_num_vars());

    for (uint32_t i = 0; i < c1.get_num_vars(); i++) {
        vterms1.push_back(c1.symbolic_vars[c1.real_variable_index[i]]);
        vterms2.push_back(c2.symbolic_vars[c2.real_variable_index[i]]);
    }

    std::unordered_map<std::string, std::string> mmap1 = s->model(vterms1);
    std::unordered_map<std::string, std::string> mmap2 = s->model(vterms2);

    std::fstream myfile;
    myfile.open(fname, std::ios::out | std::ios::trunc | std::ios::binary);
    myfile << "w12 = {" << std::endl;
    for (uint32_t i = 0; i < c1.get_num_vars(); i++) {
        std::string vname1 = vterms1[i].toString();
        std::string vname2 = vterms2[i].toString();
        if (c1.real_variable_index[i] == i) {
            myfile << "{" << mmap1[vname1] << ", " << mmap2[vname2] << "}";
            myfile << ",           // " << vname1 << ", " << vname2 << std::endl;
        } else {
            myfile << "{" << mmap1[vname1] << ", " + mmap2[vname2] << "}";
            myfile << ",           // " << vname1 << " ," << vname2 << " -> " << c1.real_variable_index[i] << std::endl;
        }
    }
    myfile << "};";
    myfile.close();

    std::unordered_map<std::string, cvc5::Term> vterms;
    for (const auto& vname : special) {
        vterms.insert({ vname + "_1", c1[vname] });
        vterms.insert({ vname + "_2", c2[vname] });
    }

    std::unordered_map<std::string, std::string> mmap = s->model(vterms);
    for (const auto& vname : special) {
        std::cout << vname << "_1, " << vname<< "_2 = "<< mmap[vname + "_1"]<< ", "<< mmap[vname + "_2"]<<std::endl;
    }
}

/**
 * @brief Get pretty formatted result of the solver work
 *
 * @details Having a circuit and defined constraint system
 * inside the solver get the pretty formatted output.
 * The whole witness will be saved in c-like array format.
 * Special variables will be printed to stdout. e.g. `a = val_a;`
 *
 * @param special The list of variables that you need to see in stdout
 * @param c first circuit
 * @param s  solver
 * @param fname file to store the resulting witness if succeded
 */
void default_model_single(const std::vector<std::string>& special,
                          smt_circuit::Circuit& c,
                          smt_solver::Solver* s,
                          const std::string& fname)
{
    std::vector<cvc5::Term> vterms;
    vterms.reserve(c.get_num_vars());

    for (uint32_t i = 0; i < c.get_num_vars(); i++) {
        vterms.push_back(c.symbolic_vars[c.real_variable_index[i]]);
    }

    std::unordered_map<std::string, std::string> mmap = s->model(vterms);

    std::fstream myfile;
    myfile.open(fname, std::ios::out | std::ios::trunc | std::ios::binary);
    myfile << "w = {" << std::endl;
    for (size_t i = 0; i < c.get_num_vars(); i++) {
        std::string vname = vterms[i].toString();
        if (c.real_variable_index[i] == i) {
            myfile << mmap[vname] << ",              // " << vname << std::endl;
        } else {
            myfile << mmap[vname] << ",              // " << vname << " -> " << c.real_variable_index[i] << std::endl;
        }
    }
    myfile << "};";
    myfile.close();

    std::unordered_map<std::string, cvc5::Term> vterms1;
    for (const auto& vname : special) {
        vterms1.insert({ vname, c[vname] });
    }

    std::unordered_map<std::string, std::string> mmap1 = s->model(vterms1);
    for (const auto& vname : special) {
        std::cout << vname<< " = "<< mmap1[vname] << std::endl;
    }
}

/**
 * @brief Get the solver result and amount of time
 * that it took to solve.
 *
 * @param s
 * @return bool is system satisfiable?
 */
bool smt_timer(smt_solver::Solver* s)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool res = s->check();
    auto stop = std::chrono::high_resolution_clock::now();
    uint32_t duration_secs = static_cast<uint32_t>(duration_cast<std::chrono::seconds>(stop - start).count());
    std::cout << "Time elapsed: " << duration_secs / 60<< " min "<< duration_secs % 60<< " sec" << std::endl;

    std::cout << s->cvc_result << std::endl;
    return res;
}
