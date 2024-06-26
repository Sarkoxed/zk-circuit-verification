#include "../include/term.hpp"

namespace smt_terms {

/**
 * Create a symbolic variable.
 *
 * @param name Name of the variable. Should be unique per variable
 * @param slv  Pointer to the global solver
 * @param type FFTerm, FFITerm or BVTerm
 * @return symbolic variable
 * */
STerm STerm::Var(const std::string& name, Solver* slv, TermType type)
{
    return STerm(name, slv, false, 16, type);
};

/**
 * Create constant symbolic variable.
 * i.e. term with predefined constant value
 *
 * @param val  String representation of the value.
 * @param slv  Pointer to the global solver.
 * @param base Base of the string representation. 16 by default.
 * @param type FFTerm, FFITerm or BVTerm
 * @return numeric constant
 * */
STerm STerm::Const(const std::string& val, Solver* slv, uint32_t base, TermType type)
{
    return STerm(val, slv, true, base, type);
};

STerm::STerm(const std::string& t, Solver* slv, bool isconst, uint32_t base, TermType type)
    : solver(slv)
    , isFiniteField(type == TermType::FFTerm)
    , isInteger(type == TermType::FFITerm)
    , isBitVector(type == TermType::BVTerm)
    , type(type)
    , operations(typed_operations.at(type))
{
    if (!isconst) {
        cvc5::Term ge;
        cvc5::Term lt;
        cvc5::Term modulus;
        switch (type) {
        case TermType::FFTerm:
            this->term = slv->term_manager.mkConst(slv->ff_sort, t);
            break;
        case TermType::FFITerm:
            this->term = slv->term_manager.mkConst(slv->term_manager.getIntegerSort(), t);
            ge = slv->term_manager.mkTerm(cvc5::Kind::GEQ, { this->term, slv->term_manager.mkInteger(0) });
            modulus = slv->term_manager.mkInteger(slv->modulus);
            lt = slv->term_manager.mkTerm(cvc5::Kind::LT, { this->term, modulus });
            slv->assertFormula(ge);
            slv->assertFormula(lt);
            break;
        case TermType::BVTerm:
            this->term = slv->term_manager.mkConst(slv->bv_sort, t);
            break;
        }
    } else {
        std::string strvalue;
        switch (type) {
        case TermType::FFTerm:
            this->term = slv->term_manager.mkFiniteFieldElem(t, slv->ff_sort, base);
            break;
        case TermType::FFITerm:
            // TODO(alex): CVC5 doesn't provide integer initialization from hex. Yet.
            strvalue = slv->term_manager.mkFiniteFieldElem(t, slv->ff_sort, base).getFiniteFieldValue();
            this->term = slv->term_manager.mkInteger(strvalue);
            this->mod();
            break;
        case TermType::BVTerm:
            // it's better to have (-p/2, p/2) representation due to overflows
            strvalue = slv->term_manager.mkFiniteFieldElem(t, slv->ff_sort, base).getFiniteFieldValue();
            this->term = slv->term_manager.mkBitVector(slv->bv_sort.getBitVectorSize(), strvalue, 10);
            break;
        }
    }
}

void STerm::mod()
{
    if (this->type == TermType::FFITerm) {
        cvc5::Term modulus = this->solver->term_manager.mkInteger(solver->modulus);
        this->term = this->solver->term_manager.mkTerm(this->operations.at(OpType::MOD), { this->term, modulus });
    }
}

STerm STerm::operator+(const STerm& other) const
{
    cvc5::Term res = this->solver->term_manager.mkTerm(this->operations.at(OpType::ADD), { this->term, other.term });
    return { res, this->solver, this->type };
}

void STerm::operator+=(const STerm& other)
{
    this->term = this->solver->term_manager.mkTerm(this->operations.at(OpType::ADD), { this->term, other.term });
}

STerm STerm::operator-(const STerm& other) const
{
    cvc5::Term res = this->solver->term_manager.mkTerm(this->operations.at(OpType::NEG), { other.term });
    res = solver->term_manager.mkTerm(this->operations.at(OpType::ADD), { this->term, res });
    return { res, this->solver, this->type };
}

void STerm::operator-=(const STerm& other)
{
    cvc5::Term tmp_term = this->solver->term_manager.mkTerm(this->operations.at(OpType::NEG), { other.term });
    this->term = this->solver->term_manager.mkTerm(cvc5::Kind::FINITE_FIELD_ADD, { this->term, tmp_term });
}

STerm STerm::operator-() const
{
    cvc5::Term res = this->solver->term_manager.mkTerm(this->operations.at(OpType::NEG), { this->term });
    return { res, this->solver, this->type };
}

STerm STerm::operator*(const STerm& other) const
{
    cvc5::Term res = solver->term_manager.mkTerm(this->operations.at(OpType::MUL), { this->term, other.term });
    return { res, this->solver, this->type };
}

void STerm::operator*=(const STerm& other)
{
    this->term = this->solver->term_manager.mkTerm(this->operations.at(OpType::MUL), { this->term, other.term });
}

/**
 * @brief Division operation
 *
 * @details Returns a result of the division by
 * creating a new symbolic variable and adding a new constraint
 * to the solver.
 *
 * @param other
 * @return STerm
 */
STerm STerm::operator/(const STerm& other) const
{
    if (!this->operations.contains(OpType::DIV)) {
        std::cerr << "Division is not compatible with " << this->type << std::endl;
        return *this;
    }
    other != "0";
    STerm res = Var("df8b586e3fa7a1224ec95a886e17a7da_div_" + static_cast<std::string>(*this) + "_" +
                        static_cast<std::string>(other),
                    this->solver,
                    this->type);
    res* other == *this;
    return res;
}

void STerm::operator/=(const STerm& other)
{
    if (!this->operations.contains(OpType::DIV)) {
        std::cerr << "Division is not compatible with " << this->type << std::endl;
        return;
    }
    other != std::string(0);
    STerm res = Var("df8b586e3fa7a1224ec95a886e17a7da_div_" + static_cast<std::string>(*this) + "_" +
                        static_cast<std::string>(other),
                    this->solver,
                    this->type);
    res* other == *this;
    this->term = res.term;
}

/**
 * Create an equality constraint between two symbolic variables of the same type
 *
 */
void STerm::operator==(const STerm& other) const
{
    STerm tmp1 = *this;
    if (tmp1.term.getNumChildren() > 1) {
        tmp1.mod();
    }
    STerm tmp2 = other;
    if (tmp2.term.getNumChildren() > 1) {
        tmp2.mod();
    }
    cvc5::Term eq = this->solver->term_manager.mkTerm(cvc5::Kind::EQUAL, { tmp1.term, tmp2.term });
    this->solver->assertFormula(eq);
}

/**
 * Create an inequality constraint between two symbolic variables of the same type
 *
 */
void STerm::operator!=(const STerm& other) const
{
    STerm tmp1 = *this;
    if (tmp1.term.getNumChildren() > 1) {
        tmp1.mod();
    }
    STerm tmp2 = other;
    if (tmp2.term.getNumChildren() > 1) {
        tmp2.mod();
    }
    cvc5::Term eq = this->solver->term_manager.mkTerm(cvc5::Kind::EQUAL, { tmp1.term, tmp2.term });
    eq = this->solver->term_manager.mkTerm(cvc5::Kind::NOT, { eq });
    this->solver->assertFormula(eq);
}

void STerm::operator<(const std::string& other) const
{
    if (!this->operations.contains(OpType::LT)) {
        std::cerr << "LT is not compatible with " << this->type << std::endl;
        return;
    }

    cvc5::Term lt = this->solver->term_manager.mkTerm(this->operations.at(OpType::LT),
                                                      { this->term, STerm(other, this->solver, this->type) });
    this->solver->assertFormula(lt);
}

void STerm::operator<=(const std::string& other) const
{
    if (!this->operations.contains(OpType::LE)) {
        std::cerr << "LE is not compatible with " << this->type << std::endl;
        return;
    }
    cvc5::Term le = this->solver->term_manager.mkTerm(this->operations.at(OpType::LE),
                                                      { this->term, STerm(other, this->solver, this->type) });
    this->solver->assertFormula(le);
}

void STerm::operator>(const std::string& other) const
{
    if (!this->operations.contains(OpType::GT)) {
        std::cerr << "GT is not compatible with " << this->type << std::endl;
        return;
    }
    cvc5::Term gt = this->solver->term_manager.mkTerm(this->operations.at(OpType::GT),
                                                      { this->term, STerm(other, this->solver, this->type) });
    this->solver->assertFormula(gt);
}

void STerm::operator>=(const std::string& other) const
{
    if (!this->operations.contains(OpType::GE)) {
        std::cerr << "GE is not compatible with " << this->type << std::endl;
        return;
    }
    cvc5::Term ge = this->solver->term_manager.mkTerm(this->operations.at(OpType::GE),
                                                      { this->term, STerm(other, this->solver, this->type) });
    this->solver->assertFormula(ge);
}

STerm STerm::operator^(const STerm& other) const
{
    if (!this->operations.contains(OpType::XOR)) {
        std::cerr << "XOR is not compatible with " << this->type << std::endl;
        return *this;
    }
    cvc5::Term res = solver->term_manager.mkTerm(this->operations.at(OpType::XOR), { this->term, other.term });
    return { res, this->solver, this->type };
}

void STerm::operator^=(const STerm& other)
{
    if (!this->operations.contains(OpType::XOR)) {
        std::cerr << "XOR is not compatible with " << this->type << std::endl;
        return;
    }
    this->term = solver->term_manager.mkTerm(this->operations.at(OpType::XOR), { this->term, other.term });
}

STerm STerm::operator&(const STerm& other) const
{
    if (!this->operations.contains(OpType::AND)) {
        std::cerr << "AND is not compatible with " << this->type << std::endl;
        return *this;
    }
    cvc5::Term res = solver->term_manager.mkTerm(this->operations.at(OpType::AND), { this->term, other.term });
    return { res, this->solver, this->type };
}

void STerm::operator&=(const STerm& other)
{
    if (!this->operations.contains(OpType::AND)) {
        std::cerr << "AND is not compatible with " << this->type << std::endl;
        return;
    }
    this->term = solver->term_manager.mkTerm(this->operations.at(OpType::AND), { this->term, other.term });
}

STerm STerm::operator|(const STerm& other) const
{
    if (!this->operations.contains(OpType::OR)) {
        std::cerr << "OR is not compatible with " << this->type << std::endl;
        return *this;
    }
    cvc5::Term res = solver->term_manager.mkTerm(this->operations.at(OpType::OR), { this->term, other.term });
    return { res, this->solver, this->type };
}

void STerm::operator|=(const STerm& other)
{
    if (!this->operations.contains(OpType::OR)) {
        std::cerr << "OR is not compatible with " << this->type << std::endl;
        return;
    }
    this->term = solver->term_manager.mkTerm(this->operations.at(OpType::OR), { this->term, other.term });
}

STerm STerm::operator<<(const uint32_t& n) const
{
    if (!this->operations.contains(OpType::LSH)) {
        std::cerr << "SHIFT LEFT is not compatible with " << this->type << std::endl;
        return *this;
    }
    cvc5::Op lsh = solver->term_manager.mkOp(this->operations.at(OpType::LSH), { n });
    cvc5::Term res = solver->term_manager.mkTerm(lsh, { this->term });
    return { res, this->solver, this->type };
}

void STerm::operator<<=(const uint32_t& n)
{
    if (!this->operations.contains(OpType::LSH)) {
        std::cerr << "SHIFT LEFT is not compatible with " << this->type << std::endl;
        return;
    }
    cvc5::Op lsh = solver->term_manager.mkOp(this->operations.at(OpType::LSH), { n });
    this->term = solver->term_manager.mkTerm(lsh, { this->term });
}

STerm STerm::operator>>(const uint32_t& n) const
{
    if (!this->operations.contains(OpType::RSH)) {
        std::cerr << "RIGHT LEFT is not compatible with " << this->type << std::endl;
        return *this;
    }
    cvc5::Op rsh = solver->term_manager.mkOp(this->operations.at(OpType::RSH), { n });
    cvc5::Term res = solver->term_manager.mkTerm(rsh, { this->term });
    return { res, this->solver, this->type };
}

void STerm::operator>>=(const uint32_t& n)
{
    if (!this->operations.contains(OpType::RSH)) {
        std::cerr << "RIGHT LEFT is not compatible with " << this->type << std::endl;
        return;
    }
    cvc5::Op rsh = solver->term_manager.mkOp(this->operations.at(OpType::RSH), { n });
    this->term = solver->term_manager.mkTerm(rsh, { this->term });
}

STerm STerm::rotr(const uint32_t& n) const
{
    if (!this->operations.contains(OpType::ROTR)) {
        std::cerr << "ROTR is not compatible with " << this->type << std::endl;
        return *this;
    }
    cvc5::Op rotr = solver->term_manager.mkOp(this->operations.at(OpType::ROTR), { n });
    cvc5::Term res = solver->term_manager.mkTerm(rotr, { this->term });
    return { res, this->solver, this->type };
}

STerm STerm::rotl(const uint32_t& n) const
{
    if (!this->operations.contains(OpType::ROTL)) {
        std::cerr << "ROTL is not compatible with " << this->type << std::endl;
        return *this;
    }
    cvc5::Op rotl = solver->term_manager.mkOp(this->operations.at(OpType::ROTL), { n });
    cvc5::Term res = solver->term_manager.mkTerm(rotl, { this->term });
    return { res, this->solver, this->type };
}

STerm operator+(const std::string& lhs, const STerm& rhs)
{
    return rhs + lhs;
}

STerm operator-(const std::string& lhs, const STerm& rhs)
{
    return (-rhs) + lhs;
}

STerm operator*(const std::string& lhs, const STerm& rhs)
{
    return rhs * lhs;
}

STerm operator^(const std::string& lhs, const STerm& rhs)
{
    return rhs ^ lhs;
}

STerm operator&(const std::string& lhs, const STerm& rhs)
{
    return rhs & lhs;
}

STerm operator|(const std::string& lhs, const STerm& rhs)
{
    return rhs | lhs;
}

STerm operator/(const std::string& lhs, const STerm& rhs)
{
    return STerm(lhs, rhs.solver, rhs.type) / rhs;
}

void operator==(const std::string& lhs, const STerm& rhs)
{
    rhs == lhs;
}

void operator!=(const std::string& lhs, const STerm& rhs)
{
    rhs != lhs;
}

std::ostream& operator<<(std::ostream& os, const TermType type)
{
    switch (type) {
    case TermType::FFTerm:
        os << "FFTerm";
        break;
    case TermType::FFITerm:
        os << "FFITerm";
        break;
    case TermType::BVTerm:
        os << "BVTerm";
        break;
    };
    return os;
}

// Parametrized calls to STerm constructor
// so you won't need to use TermType each time to define a new variable.

STerm FFVar(const std::string& name, Solver* slv)
{
    return STerm::Var(name, slv, TermType::FFTerm);
}

STerm FFConst(const std::string& val, Solver* slv, uint32_t base)
{
    return STerm::Const(val, slv, base, TermType::FFTerm);
}

STerm FFIVar(const std::string& name, Solver* slv)
{
    return STerm::Var(name, slv, TermType::FFITerm);
}

STerm FFIConst(const std::string& val, Solver* slv, uint32_t base)
{
    return STerm::Const(val, slv, base, TermType::FFITerm);
}

STerm BVVar(const std::string& name, Solver* slv)
{
    return STerm::Var(name, slv, TermType::BVTerm);
}

STerm BVConst(const std::string& val, Solver* slv, uint32_t base)
{
    return STerm::Const(val, slv, base, TermType::BVTerm);
}

} // namespace smt_terms
