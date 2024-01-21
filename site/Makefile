.PHONY: setup
setup: cvc5 circom

.PHONY: cvc5
cvc5:
	cd cvc5
	./configure.sh --auto-download --cocoa --python-bindings -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
	cd build
	make -j4
	cd ..

.PHONY: circom
circom:
	cd circom
	cargo build --release
	cargo install --path circom
	cd ..
