CIRCUIT=test
WD = constraints/$(CIRCUIT)
SD = circuits/$(CIRCUIT)

.PHONY: default
default: init constraints map witness

.PHONY: init
init:
	@mkdir $(WD) || true
	@mkdir verify/$(CIRCUIT) || true

.PHONY: constraints
constraints: $(SD)/*.circom $(WD)
	@circom $(SD)/main.circom --r1cs --wasm -c --sym -o $(WD)
	@snarkjs rp $(WD)/main_c.r1cs $(WD)/main_c.sym > $(WD)/constraints.hr
	@snarkjs rej $(WD)/main_c.r1cs $(WD)/constraints.json

.PHONY: map
map: constraints
	@python tools/mapper.py $(WD)/main_c.sym $(WD)/map_c.json

.PHONY: witness
witness: $(SD)/input.json constraints
	@snarkjs wc $(WD)/main_c_js/main_c.wasm $(SD)/input.json $(WD)/witness.wtns 
	@snarkjs wej $(WD)/witness.wtns $(WD)/witness.json

.PHONY: clean
clean: 
	@echo "Cleaning constraints, witness, twitness and calculations directories"
	@rm -rf $(WD) 2>/dev/null || true
