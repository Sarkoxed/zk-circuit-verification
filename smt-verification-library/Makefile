.PHONY: default
default: lib

CVC5_INCLUDE = ../site/cvc5/build/include
CVC5_LIB = ../site/cvc5/build/lib
JSON_INCLUDE = ../site/json/single_include
CXX = clang++

.PHONY: lib
lib: obj
	$(CXX) -shared ./obj/*.o -o ./lib/libcircuit.so

.PHONY: obj
obj: circuit/*.cpp terms/*.cpp solver/*.cpp
	$(CXX) -std=c++20 -I./include -I$(CVC5_INCLUDE) -c ./solver/solver.cpp -o ./obj/solver.o  -fPIC
	$(CXX) -std=c++20 -I./include -c ./terms/term.cpp  -o ./obj/term.o -fPIC
	$(CXX) -std=c++20 -I./include -c ./terms/bool.cpp    -o ./obj/bool.o -fPIC
	$(CXX) -std=c++20 -I./include -c -I$(JSON_INCLUDE) ./circuit/circuit_schema.cpp   -o ./obj/circuit_schema.o -fPIC
	$(CXX) -std=c++20 -I./include -c ./util/smt_util.cpp  -o ./obj/smt_util.o -fPIC
	$(CXX) -std=c++20 -I./include -I$(JSON_INCLUDE) -c ./circuit/circuit.cpp -o ./obj/circuit.o -fPIC

.PHONY: clean
clean:
	@echo "Cleaning binaries obj/*.o and lib/*.so..."
	-@rm lib/*.so 2>/dev/null || true
	-@rm obj/*.o 2>/dev/null || true
