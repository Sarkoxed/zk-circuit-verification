from z3 import BitVec, Or, And, sat, unsat, Solver
import json
from sys import argv

from python_function import f, params


def find_all_solutions(s, vars, bound):
    w = []
    i = 0
    print("_______________________________________________")
    print("Найденные Входные сигналы:")
    if argv[1] == "--1":
        if s.check() == sat:
            m = s.model()
            for x in vars:
                w.append(str(m[x]))
        else:
            print("unsat")

    elif argv[1] == "--all":
        while s.check() == sat:
            m = s.model()
            for x in vars:
                w.append(str(m[x]))

            print("".join(w))

            new = []
            for x in vars:
                new.append(x != m[x])
            s.add(Or(new))
            w = []
            i += 1
            if i > bound:
                print("too much")
                exit()
    else:
        print("unknown option")


def verify(bound):
    constrs = json.load(open("constraints/constr.json"))
    witness = json.load(open("witness/witness.json"))
    # map = json.load(open("data/map.json"))["map"]

    nvars = constrs["nVars"]
    nout = constrs["nOutputs"]
    nPubInputs = constrs["nPubInputs"]
    nPrvInputs = constrs["nPrvInputs"]

    out_exp = [int(x) for x in witness[1 : 1 + nout]]

    inp = [BitVec(f"python_var_{i}", 32) for i in range(nPrvInputs)]
    out = f(inp, *params)
    
    print(out)
    s = Solver()
    for i in range(nout):
        s.add(out_exp[i] == out[i])

    find_all_solutions(s, inp, bound)


def check_witness():
    constrs = json.load(open("constraints/constr.json"))
    witness = json.load(open("witness/witness.json"))
    # map = json.load(open("data/map.json"))["map"]

    nvars = constrs["nVars"]
    nout = constrs["nOutputs"]
    nPubInputs = constrs["nPubInputs"]
    nPrvInputs = constrs["nPrvInputs"]

    out_exp = [int(x) for x in witness[1 : 1 + nout]]

    inp = [int(x) for x in witness[1 + nout : 1 + nout + nPrvInputs]]
    out = f(inp, *params)

    assert out == out_exp


if __name__ == "__main__":
    check_witness()
    verify(20)
