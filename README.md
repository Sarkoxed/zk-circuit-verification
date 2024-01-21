cd site
make
cd ../smt-verification-library
make
cd ../circom-verification
make
cd ..

-> circom-verification/circuits/something/main.circom
make translate-circom 
-> circom-verification/verify/something/main.cpp
make verify-circom

# TODO
- plonk switch
- eh
