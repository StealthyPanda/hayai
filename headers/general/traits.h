#pragma once

#include <iostream>
#include "stringslice.h"

//List of all traits, applies the __traits_op macro to each element in the list.
#define __traits_list \
\
/*general stuff*/\
__trait_op(translationunit) \
__trait_op(unknown) \
__trait_op(newline) \
__trait_op(whitespace) \
__trait_op(blob) \
__trait_op(statement) \
__trait_op(expression) \
__trait_op(datatype) \
__trait_op(identifier) \
__trait_op(comment) \
\
\
/*statement stuff*/\
__trait_op(statementterminator) \
__trait_op(vardeclaration) \
__trait_op(varassign) \
__trait_op(lvalue) \
__trait_op(rvalue) \
\
\
/*expression stuff*/\
__trait_op(numeric) \
__trait_op(isdigits) \
__trait_op(isinteger) \
__trait_op(strmark) \
__trait_op(commasep) \
__trait_op(call) \
__trait_op(param) \
__trait_op(stringliteral) \
\
\
/*operator stuff*/\
__trait_op(op) \
__trait_op(boolop) \
__trait_op(opplus) \
__trait_op(opminus) \
__trait_op(opslash) \
__trait_op(opbackwardslash) \
__trait_op(opstar) \
__trait_op(opampersand) \
__trait_op(opcarat) \
__trait_op(oppercentage) \
__trait_op(opdollar) \
__trait_op(opat) \
__trait_op(opexclamation) \
__trait_op(opgreaterthan) \
__trait_op(oplesserthan) \
__trait_op(opdot) \
__trait_op(opquestionmark) \
__trait_op(arrow) \
__trait_op(tilde) \
__trait_op(pipe) \
__trait_op(greaterthanorequalto) \
__trait_op(lesserthanorequalto) \
__trait_op(equalto) \
__trait_op(plusequalto) \
__trait_op(minusequalto) \
__trait_op(starequalto) \
__trait_op(divequalto) \
__trait_op(unary) \
__trait_op(binary) \
__trait_op(logicaland) \
__trait_op(logicalor) \
__trait_op(notequalto) \
__trait_op(vertline) \
\
\
/*variable stuff*/\
__trait_op(vardeclarator) \
__trait_op(ptrdeclarator) \
__trait_op(ismutable) \
__trait_op(isconst) \
__trait_op(constdeclarator) \
__trait_op(assigner) \
\
\
/*function stuff*/\
__trait_op(left) \
__trait_op(right) \
__trait_op(roundbracket) \
__trait_op(squarebracket) \
__trait_op(brace) \
__trait_op(function) \
__trait_op(functiondeclarator) \
__trait_op(body) \
__trait_op(argument) \
__trait_op(clone) \
__trait_op(inlinedef) \
__trait_op(rkeyword) \
__trait_op(colon) \
__trait_op(inlinedefsymbol) \
__trait_op(clonekeyword) \
__trait_op(fcall) \
__trait_op(functiondeclaration) \
__trait_op(freturntype) \
__trait_op(returner) \
\
\
/*preprocessor directives*/\
__trait_op(hash) \
\
\
/*Control flow*/\
__trait_op(controlflow)\
__trait_op(cflowkeyword)\
__trait_op(ifkeyword)\
__trait_op(elifkeyword)\
__trait_op(elsekeyword)\
__trait_op(whilekeyword)\
__trait_op(forkeyword)\
__trait_op(ifblock)\
__trait_op(elifblock)\
__trait_op(elseblock)\
__trait_op(whileblock)\
__trait_op(forblock)



#include "cvals.h"








//making the struct
#define __trait_op(name) unsigned char name : 1;
typedef struct traits
{
    traits()
    {
        unsigned char *ptr = (unsigned char*) this;
        for (size_t i = 0; i < sizeof(traits); i++) ptr[i] = 0;
    }

    __traits_list
} traits ;


#undef __trait_op


//defining all the string reprs
#define __trait_op(name) const char* _##name##_str = #name;
__traits_list
#define __str_repr(name) _##name##_str


#undef __trait_op

//defining a print function for traits
#define __trait_op(name) if (t.name) os << __str_repr(name) << ", ";
std::ostream& operator<<(std::ostream& os, traits &t)
{
    __traits_list
    return os;
}


/// @brief finds all the traits that can be automagically inferred from the given token stringslice.
/// @param ss the token to be evaluated.
/// @return a `traits` struct instance with all the inferred traits set to 1.
traits gettraits(stringslice ss)
{
    traits t;

    size_t l = length(ss);

    if (l == 0) return t;
    else if (l == 1)
    {
        t.op = 1;
        if (equal(ss, _exclamationmark))
        {
            t.opexclamation = 1;
            t.unary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _at)) t.opat = 1;
        else if (equal(ss, _dollar)) t.opdollar = 1;
        else if (equal(ss, _percent))
        {
            t.oppercentage = 1;
            t.binary = 1;
        }
        else if (equal(ss, _carat))
        {
            t.opcarat = 1;
            t.binary = 1;
        }
        else if (equal(ss, _ampersand)) t.opampersand = 1;
        else if (equal(ss, _star))
        {
            t.opstar = 1;
            t.binary = 1;
        }
        else if (equal(ss, _minus))
        {
            t.opminus = 1;
            t.binary = 1;
        }
        else if (equal(ss, _plus))
        {
            t.opplus = 1;
            t.binary = 1;
        }
        else if (equal(ss, _lesser))
        {
            t.oplesserthan = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _greater))
        {
            t.opgreaterthan = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _dot)) t.opdot = 1;
        else if (equal(ss, _fslash))
        {
            t.opslash = 1;
            t.binary = 1;
        }
        else if (equal(ss, _bslash))
        {
            t.opbackwardslash = 1;
            t.binary = 1;
        }
        else if (equal(ss, _question)) t.opquestionmark = 1;
        else if (isdigitsonly(ss))
        {
            t.op = 0;
            t.isdigits = 1;
            t.isinteger = 1;
            t.numeric = 1;
        }
        else if (isonlyalpha(ss))
        {
            t.op = 0;
            t.identifier = 1;
        }
        else if (equal(ss, _vertline))
        {
            t.op = 0;
            t.vertline = 1;
        }
        else if (equal(ss, _comma))
        {
            t.op = 0;
            t.commasep = 1;
        }
        else if (equal(ss, _lrb))
        {
            t.op = 0;
            t.roundbracket = 1;
            t.left = 1;
        }
        else if (equal(ss, _rrb))
        {
            t.op = 0;
            t.roundbracket = 1;
            t.right = 1;
        }
        else if (equal(ss, _lsb))
        {
            t.op = 0;
            t.squarebracket = 1;
            t.left = 1;
        }
        else if (equal(ss, _rsb))
        {
            t.op = 0;
            t.squarebracket = 1;
            t.right = 1;
        }
        else if (equal(ss, _lcb))
        {
            t.op = 0;
            t.brace = 1;
            t.left = 1;
        }
        else if (equal(ss, _rcb))
        {
            t.op = 0;
            t.brace = 1;
            t.right = 1;
        }
        else if (equal(ss, _semicolon))
        {
            t.op = 0;
            t.statementterminator = 1;
        }
        else if (equal(ss, _quote))
        {
            t.op = 0;
            t.strmark = 1;
        }
        else if (equal(ss, _colon))
        {
            t.op = 0;
            t.colon = 1;
        }
        else if (equal(ss, _equal))
        {
            t.op = 0;
            t.assigner = 1;
        }
        else if (equal(ss, _tilde))
        {
            t.op = 0;
            t.tilde = 1;
        }
        else if (equal(ss, _newline))
        {
            t.op = 0;
            t.newline = 1;
            t.whitespace = 1;
        }
        else if (equal(ss, _space) || equal(ss, _tab) || iswhitespace(*ss.start))
        {
            t.op = 0;
            t.whitespace = 1;
        }
    }
    else if (l == 2)
    {
        if (equal(ss, _fatarrow)) t.inlinedefsymbol = 1;
        else if (equal(ss, _pipe)) t.pipe = 1;
        else if (isstringliteral(ss)) t.stringliteral = 1;
        else if (equal(ss, _arrow)) t.arrow = 1;
        else if (equal(ss, _comment)) t.comment = 1;
        else if (equal(ss, _gtet))
        {
            t.greaterthanorequalto = 1;
            t.op = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _ltet))
        {
            t.lesserthanorequalto = 1;
            t.op = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _equalto))
        {
            t.equalto = 1;
            t.op = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _pluset))
        {
            t.plusequalto = 1;
            t.op = 1;
            t.assigner = 1;
        }
        else if (equal(ss, _minuset))
        {
            t.minusequalto = 1;
            t.op = 1;
            t.assigner = 1;
        }
        else if (equal(ss, _staret))
        {
            t.starequalto = 1;
            t.op = 1;
            t.assigner = 1;
        }
        else if (equal(ss, _slashet))
        {
            t.divequalto = 1;
            t.op = 1;
            t.assigner = 1;
        }
        else if (equal(ss, _and))
        {
            t.logicaland = 1;
            t.op = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _or))
        {
            t.logicalor = 1;
            t.op = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _net))
        {
            t.notequalto = 1;
            t.op = 1;
            t.binary = 1;
            t.boolop = 1;
        }
        else if (equal(ss, _if))
        {
            t.cflowkeyword = 1;
            t.ifkeyword = 1;
        }
        else if (isdigitsonly(ss))
        {
            t.isdigits = 1;
            t.isinteger = 1;
            t.numeric = 1;
        }
        else if (isint(ss))
        {
            t.isinteger = 1;
            t.numeric = 1;
        }
        else if (isnum(ss)) t.numeric = 1;
        else if (isonlyalpha(ss)) t.identifier = 1;
    }
    else
    {
        if (equal(ss, _fun)) t.functiondeclarator = 1;
        else if (equal(ss, _return)) t.rkeyword = 1;
        else if (equal(ss, _clone)) t.clonekeyword = 1;
        else if (isstringliteral(ss)) t.stringliteral = 1;
        else if (equal(ss, _if))
        {
            t.identifier = 0;
            t.cflowkeyword = 1;
            t.ifkeyword = 1;
        }
        else if (equal(ss, _elif))
        {
            t.cflowkeyword = 1;
            t.elifkeyword = 1;
        }
        else if (equal(ss, _else))
        {
            t.cflowkeyword = 1;
            t.elsekeyword = 1;
        }
        else if (equal(ss, _while))
        {
            t.cflowkeyword = 1;
            t.whilekeyword = 1;
        }
        else if (equal(ss, _for))
        {
            t.cflowkeyword = 1;
            t.forkeyword = 1;
        }
        else if (equal(ss, _let))
        {
            t.vardeclarator = 1;
            t.ismutable = 1;
        }
        else if (equal(ss, _ptr))
        {
            t.ptrdeclarator = 1;
            // t.ismutable = 1;
        }
        else if (equal(ss, _const))
        {
            t.vardeclarator = 1;
            t.constdeclarator = 1;
        }
        else if (isdigitsonly(ss))
        {
            t.isdigits = 1;
            t.numeric = 1;
            t.isinteger = 1;
        }
        else if (isint(ss))
        {
            t.numeric = 1;
            t.isinteger = 1;
        }
        else if (isnum(ss)) t.numeric = 1;
        else if (isonlyalpha(ss)) t.identifier = 1;
        else t.unknown = 1;
    }
    

    return t;
}

#undef __trait_op

#define __trait_op(name) U.name = t1.name || t2.name;

traits unionof(traits &t1, traits &t2)
{
    traits U;
    __traits_list
    return U;
}

#undef __trait_op
