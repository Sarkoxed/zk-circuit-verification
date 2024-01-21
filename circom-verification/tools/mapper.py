# map json values to their symbolic names in circuit
import json
import sys

def create_map(filename_sym, file_json=None):
    fd = open(filename_sym, 'rt')
    lines = fd.read().split('\n')[:-1]

    Idx2Name = [0 for _ in range(len(lines))]    
    Idx2Name[0] = "one"
    
    idxs = []
    for l in lines:
        arr = l.split(",")
        if len(arr) != 4:
            return None
        if Idx2Name[int(arr[1])] != 0:
            Idx2Name[int(arr[1])] += "|" + arr[3]
        else:
            Idx2Name[int(arr[1])] = arr[3]
        idxs.append(arr[1])
    fd.close()
    Idx2Name = [x for x in Idx2Name if x != 0]
    
    if file_json is not None:
        with open(file_json, 'wt') as f:
            json.dump({"map": Idx2Name}, f)
    return Idx2Name

fn_sym = sys.argv[1]
fn_json = sys.argv[2]
create_map(fn_sym, fn_json)
