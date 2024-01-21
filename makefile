CIRCUIT = test
INCLUDE  = -I./site/json/single_include
CVC_INCLUDE += -I./site/cvc5/build/include
CVC_LIB = -L./site/cvc5/build/build/lib -Wl,-rpath=./site/cvc5/build/build/lib

#.PHONY: setup
#setup: cvc5 circom
#
#.PHONY: cvc5
#cvc5: site/cvc5
#	cd ./site/cvc5
#	./configure.sh --auto-download --cocoa --python-bindings -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
#	cd build
#	make -j4
#	cd ../../
#
#.PHONY: circom
#circom: site/circom
#	cd ./site/circom
#	cargo build --release
#	cargo install --path circom
#	cd ../../

.PHONY: translator
translator: circom-verification/translator/translator.cpp circom-verification/translator/translator.hpp
	clang++ circom-verification/translator/translator.cpp $(INCLUDE) -o circom-verification/translator.out

.PHONY: translate-circom 
translate-circom: translator
	@cd circom-verification
	@make CIRCUIT=$(CIRCUIT)
	@./circom-verification/translator.out circom-verification/constraints/$(CIRCUIT)/constraints.json circom-verification/constraints/$(CIRCUIT)/map_c.json circom-verification/constraints/$(CIRCUIT)/witness.json circom-verification/constraints/$(CIRCUIT)/$(CIRCUIT).json
	@cd ..

.PHONY: verify-circom
verify-circom: translate-circom compile-smt
	clang++ -std=c++20 circom-verification/verify/$(CIRCUIT)/$(CIRCUIT).cpp -L./$(SMT)/lib -Wl,-rpath=./$(SMT)/lib $(CVC_LIB) -I./$(SMT)/include $(INCLUDE) -lcvc5 -lterms -lcircuit -lsolver

.PHONY: quick-build
quick-build:
	clang++ -std=c++20 circom-verification/verify/test/test.cpp smt-verification-library/circuit/circuit.cpp smt-verification-library/solver/solver.cpp smt-verification-library/terms/bool.cpp smt-verification-library/terms/ffiterm.cpp smt-verification-library/terms/ffterm.cpp -I./smt-verification-library/include -I./site/json/single_include -I./site/cvc5/build/include -L./site/cvc5/build/build/lib -Wl,-rpath=./site/cvc5/build/build/lib -lcvc5
