CIRCUIT = test

CXX = clang++
INCLUDE  = -I./site/json/single_include

circom-verification/translator.out: circom-verification/translator/translator.cpp circom-verification/translator/translator.hpp
	$(CXX) $< $(INCLUDE) -o $@

.PHONY: translator
translator: circom-verification/translator.out

.PHONY: translate-circom 
translate-circom: translator circom-verification/constraints/$(CIRCUIT)/constraints.json
	circom-verification/translator.out circom-verification/constraints/$(CIRCUIT)/constraints.json circom-verification/constraints/$(CIRCUIT)/map_c.json circom-verification/constraints/$(CIRCUIT)/witness.json circom-verification/constraints/$(CIRCUIT)/$(CIRCUIT).json

FLAGS = --std=c++20
FLAGS += -I./site/json/single_include
FLAGS += -I./smt-verification-library/include
FLAGS += -L./smt-verification-library/lib -Wl,-rpath=./smt-verification-library/lib
FLAGS += -lcircuit
FLAGS += -I./site/cvc5/build/include 
FLAGS += -L./site/cvc5/build/build/lib -Wl,-rpath=./site/cvc5/build/build/lib
FLAGS += -lcvc5

.PHONY: verify-circom
verify-circom: $(CIRCUIT).out

$(CIRCUIT).out: circom-verification/verify/$(CIRCUIT)/$(CIRCUIT).cpp smt-verification-library/lib
	clang++ circom-verification/verify/$(CIRCUIT)/$(CIRCUIT).cpp $(FLAGS) -o bin/$(CIRCUIT).out -O3
