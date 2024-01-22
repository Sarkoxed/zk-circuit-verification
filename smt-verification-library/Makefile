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
	$(CXX) -std=c++20 -I./include -I$(CVC5_INCLUDE) -c ./solver/solver.cpp -o ./obj/solver.o 
	$(CXX) -std=c++20 -I./include -c ./terms/ffterm.cpp  -o ./obj/ffterm.o
	$(CXX) -std=c++20 -I./include -c ./terms/ffiterm.cpp -o ./obj/ffiterm.o
	$(CXX) -std=c++20 -I./include -c ./terms/bool.cpp    -o ./obj/bool.o
	$(CXX) -std=c++20 -I./include -I$(JSON_INCLUDE) -c ./circuit/circuit.cpp -o ./obj/circuit.o -fPIC

.PHONY: clean
clean:
	@echo "Cleaning binaries obj/*.o and lib/*.so..."
	-@rm lib/*.so 2>/dev/null || true
	-@rm obj/*.o 2>/dev/null || true
