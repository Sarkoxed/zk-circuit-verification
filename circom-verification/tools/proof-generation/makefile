FOO = main

circom = circuits/$(FOO).circom
r1cs = constraints/$(FOO).r1cs
wasm = constraints/$(FOO)_js/$(FOO).wasm
wit_gen = constraints/$(FOO)_js/generate_witness.js
#wit_gen = constraints/$(FOO)_cpp/$(FOO)

compile_outputs = constraints/$(FOO)_js/witness_calculator.js $(r1cs) $(wasm) $(wit_gen)
pk = keys/$(FOO).zkey
vk = keys/$(FOO)_VK.json

ptau = pot/pot13.ptau
keys = keys/$(pk) keys/$(vk)
p_input = input/$(FOO).input.json
wit = witness/$(FOO).wtns
pf = $(FOO).pf.json
inst = $(FOO).pub.json
prove_outputs = $(pf) $(inst)

all: verify

$(compile_outputs): $(circom)
	circom $< --r1cs --wasm --sym --c -o constraints/
	#make -C constraints/$(FOO)_cpp/

.PHONY: keys setup prove verify clean
$(pk): $(r1cs) $(ptau)
	snarkjs plonk setup $(r1cs) $(ptau) $(pk)	

keys: $(pk)
	snarkjs zkey export verificationkey $(pk) $(vk)

$(wit): $(p_input) $(wasm) $(wit_gen)
	node $(wit_gen) $(wasm) $(p_input) $(wit)

#$(wit): $(p_input) $(wit_gen)
#	$(wit_gen) $(p_input) $(wit)

prove: $(wit) $(pk) keys
	snarkjs plonk prove $(pk) $(wit) $(pf) $(inst)

verify: $(pf) $(inst) $(vk)
	snarkjs plonk verify $(vk) $(inst) $(pf)

clean:
	-rm -rf constraints/* 2>/dev/null
	-rm -f keys/* 2>/dev/null
	-rm -f witness/* 2>/dev/null 
	-rm -f *.pf.json 2>/dev/null 
	-rm -f *.pub.json 2>/dev/null 
