cd site
make
cd ../smt-verification-library
make
cd ../circom-verification
make

-> circom-verification/circuits/something/main.circom

make translate-circom CIRCUIT=CIRCUITNAME

-> circom-verification/verify/something/main.cpp

make verify-circom CIRCUIT=CIRCUITNAME

# TODO
- plonk switch
- eh
