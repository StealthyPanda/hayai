#pragma once

#include "./../general/ll.h"
#include "./../general/traits.h"
#include "./../general/strings.h"
#include "newtok.h"

/// @brief node that stores a token in the AST.
class astnode
{
public:
    astnode();
    astnode(token *tk);
    ~astnode();

    traits tokentraits;
    ll<astnode> children;
    token *tok;
    astnode *parent;

    void print();
};

astnode::astnode()
{
    this->parent = NULL;
}

astnode::~astnode()
{
}


/// @brief creates a new node with the given token, and sets its traits automatically.
/// @param tk a stringslice reference to the token.
astnode::astnode(token *tk)
{
    this->tok = tk;
    this->tokentraits = tk->props;
    this->parent = NULL;
}

/// @brief prints the given AST node to standard output in the format [*token string*](*token traits*)
void astnode::print()
{
    this->tok->print();
    std::cout << "(" << this->children.length <<  ")";
}

result<astnode> processstatement(tokenizer &tk);
result<astnode> processexpression(tokenizer &tk);
result<astnode> processfcall(tokenizer &tk, astnode *idtok);
result<astnode> processfuncdef(tokenizer &tk);
result<astnode> processscope(tokenizer &tk);
result<astnode> processcflow(tokenizer &tk);



/// @brief Makes and stores and AST.
class ast
{
public:
    ast(char *filepath);
    ~ast();

    char *filepath;

    tokenizer *tk = NULL;

    astnode *root;

    void printbf();

    result<void> parse();
};


/// @brief makes an ast out of the given file
/// @param filepath 
ast::ast(char *filepath)
{
    this->filepath = filepath;

    this->root = new astnode();
    this->root->tokentraits.translationunit = 1;
}

/// @brief parses the given file, and generates an AST for the translation unit
result<void> ast::parse()
{
    this->tk = new tokenizer(this->filepath);


    result<size_t> res = this->tk->read();
    if (!res.ok)
        return result<void>(res.err);

    res = this->tk->parse();
    if (!res.ok)
        return result<void>(res.err);
    
    result<astnode> noderes;
    stringslice ssbuff;
    traits t;
    astnode *buffer, *tutok = new astnode();
    tutok->tokentraits.translationunit = 1;


    //main loop
    while (this->tk->available())
    {
        tk->prestrip();
        if (this->tk->available()) ssbuff = tk->peek();
        else break;
        t = gettraits(ssbuff);
     
        if (t.functiondeclarator) noderes = processfuncdef(*tk);
        else if (t.cflowkeyword) noderes = processcflow(*tk);
        else noderes = processstatement(*tk);

        if (!noderes.ok) return result<void>(noderes.err);
        noderes.value->parent = tutok;
        tutok->children.postpend(noderes.value);
    }

    
    this->root = tutok;
    
    result<void> ok;
    ok.ok = 1;
    return ok;
}


