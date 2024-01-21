#include "translator.hpp"

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

void write_json(nlohmann::json& circuit, std::string& filename){
    std::ofstream fout;
    fout.open(filename);
    fout << circuit.dump();
    fout.close();
}

int main(int argc, char**argv){
    if (argc < 4){
        std::cerr << "Usage: translator <constraints.json> <map.json> <witness.json> <output.json>" << std::endl;
        return 1;
    }
    auto constraints = open(argv[1]);
    auto map = open(argv[2]);
    auto witness = open(argv[3]);
    
    nlohmann::json circuit;
    circuit["modulus"] = constraints["prime"];

    int pubn = constraints["nPubInputs"];
    std::vector<uint32_t> public_inps;
    for(size_t i = 1; i < pubn + 1; i++){
        public_inps.push_back(i);
    }   
    circuit["public_inps"] = public_inps;
        
    std::unordered_map<uint32_t, std::string> vars_of_interest; // TODO: actually pass them
    for(size_t i = 0; i < static_cast<size_t>(constraints["nVars"]); i++){
        vars_of_interest[i] = map["map"][i];
    }
    circuit["vars_of_interest"] = vars_of_interest;

    std::vector<std::string> variables;
    for(size_t i = 0; i < static_cast<size_t>(constraints["nVars"]); i++){
        variables.push_back(witness[i]);
    }
    circuit["variables"] = variables;
    
    std::vector<std::vector<std::vector<std::string>>> selectors;
    std::vector<std::vector<std::vector<uint32_t>>> wires;
    for(auto constr: constraints["constraints"]){
        std::vector<std::vector<std::string>> tmp_sel(3);
        std::vector<std::vector<uint32_t>> tmp_wir(3);
        
        for(size_t i = 0; i < 3; i++){
            for(auto term: constr[i].items()){
                tmp_sel[i].push_back(term.value());
                tmp_wir[i].push_back(std::stoi(term.key()));
            }
        }
        selectors.push_back(tmp_sel);
        wires.push_back(tmp_wir);
    }

    circuit["selectors"] = selectors;
    circuit["wires"] = wires;
    
    std::string outfile = argv[4];
    write_json(circuit, outfile);
    return 0;
}
