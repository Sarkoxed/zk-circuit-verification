#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <gmpxx.h>

using uint32_t = unsigned int;
nlohmann::json open(const std::string& filename);
void write_json(nlohmann::json& circuit, std::string& filename);
