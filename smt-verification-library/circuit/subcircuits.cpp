#include "subcircuits.hpp"

CircuitSchema get_range_constrant_circuit(uint32_t n){
    StandardCircuitBuilder builder = StandardCircuitBuilder();
    field_t a(witness_t(&builder, fr::random_element()));
    a.create_range_constraint(n);
    msgpack::sbuffer buf = builder.export_circuit();
    CircuitSchema circuit_info = unpack_from_buffer(buf);
    return circuit_info;
}

CircuitSchema get_xor_circuit(){
    return CircuitSchema({});
}
