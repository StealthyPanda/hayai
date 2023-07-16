#pragma once

#include <iostream>
#include "tokenizer.h"
#include "./../general/traits.h"

/// @brief node that stores a token in the AST.
class astnode
{
public:
    astnode();
    astnode(astnode& an);
    astnode(stringslice& tk);
    ~astnode();

    traits tokentraits;
    ll<astnode> children;
    stringslice tokenstr;
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

/// @brief creates a copy of the given astnode
astnode::astnode(astnode& an)
{
    this->tokentraits = an.tokentraits;
    this->tokenstr = an.tokenstr;
    this->children = an.children;
    this->parent = an.parent;
}

/// @brief creates a new node with the given token, and sets its traits automatically.
/// @param tk a stringslice reference to the token.
astnode::astnode(stringslice& tk)
{
    this->tokenstr = tk;
    this->tokentraits = gettraits(tk);
    this->parent = NULL;
}

/// @brief prints the given AST node to standard output in the format [*token string*](*token traits*)
void astnode::print()
{
    std::cout << "[";
    ::print(this->tokenstr);
    std::cout << "](" << this->tokentraits << this->children.length << ")";
}




/// @brief Makes and stores and AST.
class ast
{
public:
    ast(char *filepath);
    ~ast();

    char *filepath;

    tokenizer *tk = NULL;

    astnode *root, *current;

    // bool appendnode(stringslice &ss);
    void printbf();

    result<void> parse();
};

result<astnode> processstatement(tokenizer &tk);
result<astnode> processexpression(tokenizer &tk);
result<astnode> processfcall(tokenizer &tk, astnode *idtok);
result<astnode> processfuncdef(tokenizer &tk);
result<astnode> processscope(tokenizer &tk);
result<astnode> processcflow(tokenizer &tk);

ast::~ast()
{
    if (this->tk != NULL) delete this->tk;
    // std::cout << "dis deletion cool\n";
}

/// @brief makes an ast out of the given file
/// @param filepath 
ast::ast(char *filepath)
{
    this->filepath = filepath;

    this->root = new astnode();
    this->root->tokentraits.translationunit = 1;

    this->current = this->root;
}

/// @brief parses the given file, and generates an AST for the translation unit
result<void> ast::parse()
{
    this->tk = new tokenizer(this->filepath);


    result<size_t> res = this->tk->read();
    if (!res.ok) return result<void>(res.err);

    res = this->tk->parse();
    if (!res.ok) return result<void>(res.err);
    
    result<astnode> noderes;
    stringslice ssbuff;
    traits t;
    astnode *buffer, *tutok = new astnode();
    tutok->tokentraits.translationunit = 1;


    //main loop
    while (this->tk->tokensleft())
    {
        tk->prestrip();
        if (this->tk->tokensleft()) ssbuff = tk->peek();
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

/// @brief processes a statement starting from the beginning of the tokenizer.
/// @return a statement token after processing
result<astnode> processstatement(tokenizer &tk)
{
    stringslice ssbuff;
    astnode *cursor;
    traits t;

    tk.prestrip();

    if (!tk.tokensleft()) return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
    else
    {
        ssbuff = tk.peek();
        cursor = new astnode(ssbuff);
    }

    //setting up some vars
    astnode *statementtoken = new astnode;
    statementtoken->tokentraits.blob = 1;
    statementtoken->tokentraits.expression = 1;
    astnode *buffer;
    astnode *dt = NULL, *id = NULL, *rvalue = NULL, *lvalue = NULL;
    
    
    //variable decl stuff
    if (cursor->tokentraits.vardeclarator)
    {
        tk.gettoken();
        statementtoken->tokentraits.vardeclaration = 1;
        if (cursor->tokentraits.constdeclarator) statementtoken->tokentraits.isconst = 1;

        tk.prestrip();
        if (tk.tokensleft())
        {
            ssbuff = tk.gettoken();
            buffer = new astnode(ssbuff);
        }
        else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));

        if (buffer->tokentraits.identifier)
        {
            dt = buffer;
            dt->tokentraits.datatype = 1;
        }
        else
        {
            dstring emsg;
            
            size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

            emsg.append("Syntax error in `");
            emsg.append((char*) tk.filepath);
            emsg.append("` at line ");
            emsg.append(tostring(ln + 1));
            emsg.append(":\n\n");
            emsg.append(getlineat(tk.file, tk.filesize, ln));
            emsg.append("\n\nExpecting identifier/datatype, got`");
            emsg.append(stringof(buffer->tokenstr));
            emsg.append("`!");

            return result<astnode>(error(255, emsg.getstring()));
        }

        tk.prestrip();
        ssbuff = tk.peek();
        
        if (gettraits(ssbuff).identifier)
        {
            tk.prestrip();
            if (tk.tokensleft())
            {    
                ssbuff = tk.gettoken();
                buffer = new astnode(ssbuff);
            }
            else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
            
            id = buffer;
        }
        else
        {
            id = dt;

            dt = NULL;

            id->tokentraits.datatype = 0;
        }


        tk.prestrip();
        ssbuff = tk.peek();
        statementtoken->tokentraits.varassign = gettraits(ssbuff).assigner;

        if (statementtoken->tokentraits.isconst)
        {
            if (!statementtoken->tokentraits.varassign)
            {
                dstring emsg;
            
                size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

                emsg.append("Syntax error in `");
                emsg.append((char*) tk.filepath);
                emsg.append("` at line ");
                emsg.append(tostring(ln + 1));
                emsg.append(":\n\n");
                emsg.append(getlineat(tk.file, tk.filesize, ln));
                emsg.append("\n\nConsts must be initialised when declared; Expected `=`!");

                return result<astnode>(error(255, emsg.getstring()));
            }
        }


        if ((dt == NULL) && (!statementtoken->tokentraits.varassign))
        {
            dstring emsg;
        
            size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

            emsg.append("Syntax error in `");
            emsg.append((char*) tk.filepath);
            emsg.append("` at line ");
            emsg.append(tostring(ln + 1));
            emsg.append(":\n\n");
            emsg.append(getlineat(tk.file, tk.filesize, ln));
            emsg.append("\n\nCannot infer datatype of `");
            emsg.append(stringof(id->tokenstr));
            emsg.append("` in declaration alone!");

            return result<astnode>(error(255, emsg.getstring()));
        }


        if (dt != NULL)
        {
            dt->tokentraits.datatype = 1;
            dt->tokentraits.identifier = 1;
            dt->parent = statementtoken;
            statementtoken->children.postpend(dt);
        }

        id->tokentraits.identifier = 1;
        id->tokentraits.datatype = 0;
        id->parent = statementtoken;
        statementtoken->children.postpend(id);
    }

    //check if any kind of assignment is being made at all
    if (!statementtoken->tokentraits.varassign)
    {
        llnode<stringslice> *holder = tk.cursor;
        traits t;
        while (holder != NULL)
        {
            t = gettraits(*holder->val);
            if (t.statementterminator) break;
            if (t.assigner)
            {
                statementtoken->tokentraits.varassign = 1;
                break;
            }
            holder = holder->next;
        }
    }

    if (
        statementtoken->tokentraits.varassign &&
        !statementtoken->tokentraits.vardeclaration
    )
    {
        // std::cout << "An assignment is being made...\n";
        tk.prestrip();
        ssbuff = tk.peek();
        t = gettraits(ssbuff);

        if (!t.identifier)
        {
            dstring emsg;
        
            size_t ln = getlinenumber(tk.file, ssbuff.start);

            emsg.append("Syntax error in `");
            emsg.append((char*) tk.filepath);
            emsg.append("` at line ");
            emsg.append(tostring(ln + 1));
            emsg.append(":\n\n");
            emsg.append(getlineat(tk.file, tk.filesize, ln));
            emsg.append("\n\nExpecting identifier, got `");
            emsg.append(stringof(id->tokenstr));
            emsg.append("`!");

            return result<astnode>(error(255, emsg.getstring()));
        }
    
        buffer = new astnode(ssbuff);
        tk.gettoken();
        statementtoken->children.postpend(buffer);
    }
    
    
    tk.prestrip();
    ssbuff = tk.peek();
    t = gettraits(ssbuff);
    if (t.assigner) tk.gettoken();
    else if (t.rkeyword)
    {
        tk.gettoken();
        statementtoken->tokentraits.returner = 1;
    }
    tk.prestrip();
    // std::cout << "Peeking here: ";
    // print(ssbuff);
    // std::cout << "\n";

    result<astnode> res;

    tk.prestrip();
    if (tk.tokensleft()) ssbuff = tk.peek();
    else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
    t = gettraits(ssbuff);
    
    if (t.cflowkeyword) res = processcflow(tk);
    else
    {
        tk.prestrip();
        ssbuff = tk.peek();
        std::cout << "Sending to process expression: ";
        print(ssbuff);
        std::cout << "\n";
        res = processexpression(tk);
    }
    
    if (!res.ok) return res;

    if (
        statementtoken->tokentraits.varassign &&
        (res.value->children.length < 1)
    )
    {
        dstring emsg;

        size_t ln = getlinenumber(tk.file, statementtoken->children.end->val->tokenstr.start);

        emsg.append("Empty assignment at line ");
        emsg.append(tostring(ln));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        // emsg.append("\nUndefined variable `");
        // emsg.append(stringof(statementtoken->children.end->val->tokenstr));
        // emsg.append("`!");

        return result<astnode>(error(244, emsg.getstring()));
    }

    res.value->tokentraits.rvalue = 1;
    res.value->tokentraits.expression = 1;
    rvalue = res.value;
    //here boi

    rvalue->parent = statementtoken;
    statementtoken->children.postpend(rvalue);
    

    while (tk.tokensleft())
    {
        tk.prestrip();
        if (tk.tokensleft()) ssbuff = tk.gettoken();
        else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
        t = gettraits(ssbuff);
        if (t.statementterminator)
        {
            statementtoken->tokentraits.statement = 1;
            statementtoken->tokentraits.expression = 0;
            break;
        }
        else if (t.brace && t.right)
        {
            if (!(tk.cursor == NULL)) tk.cursor = tk.cursor->prev;
            else tk.cursor = tk.tokens.end;
            delete statementtoken;
            rvalue->parent = NULL;
            return result<astnode>(rvalue);
        }
    }
    

    return result<astnode>(statementtoken);
}


/// @brief processes an expression from the beginning of the tokenizer
/// @return an expression token
result<astnode> processexpression(tokenizer &tk)
{
    ///todo gotta implement expression evaluation and all that nonsense.
    astnode *expressiontoken = new astnode;
    expressiontoken->tokentraits.expression = 1;

    tk.prestrip();

    if (!tk.tokensleft())
    return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));

    stringslice ssbuff = tk.gettoken();
    astnode *buffer = new astnode(ssbuff), *current = expressiontoken, *holder = NULL;

    //if it's a string literal, sendit back brudda
    if (buffer->tokentraits.stringliteral)
    {
        tk.prestrip();
        ssbuff = tk.peek();
        traits t = gettraits(ssbuff);
        std::cout << "right before: ";
        ::print(ssbuff);
        std::cout << "\n";
        if (!(t.statementterminator || t.right || t.commasep))
        {
            dstring emsg;
            
            size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

            emsg.append("Syntax error in `");
            emsg.append((char*) tk.filepath);
            emsg.append("` at line ");
            emsg.append(tostring(ln + 1));
            emsg.append(":\n\n");
            emsg.append(getlineat(tk.file, tk.filesize, ln));
            emsg.append("\n\nExpecting `;`, got`");
            emsg.append(stringof(buffer->tokenstr));
            emsg.append("`!");

            return result<astnode>(error(237, emsg.getstring()));
        }

        // tk.gettoken();
        // buffer->tokentraits.expression = 1;
        expressiontoken->children.postpend(buffer);
        buffer->parent = expressiontoken;
        return result<astnode>(expressiontoken);
    }

    size_t nbracks = 0;

    //this block deals with all other expressions
    while (
        (!buffer->tokentraits.statementterminator) && 
        (!(buffer->tokentraits.brace)) &&
        (!buffer->tokentraits.commasep) &&
        (!(buffer->tokentraits.roundbracket && buffer->tokentraits.right && (nbracks == 0)))
    )
    {
        // std::cout << "\tProcessing token in exprn: ";
        // buffer->print();
        // std::cout << "\n";
        if (buffer->tokentraits.whitespace)
        {
            delete buffer; 
            ssbuff = tk.gettoken();
            buffer = new astnode(ssbuff);
            continue;
        }
        if (current->tokentraits.op && (current->children.length >= 2))
        {
            current = current->parent;
            continue;
        }

        if (buffer->tokentraits.op)
        {
            // tk.prestrip();
            holder = current->children.postpop();
            if (holder != NULL)
            {
                holder->parent = buffer;
                buffer->children.prepend(holder);
            }
            buffer->parent = current;
            current->children.postpend(buffer);
            current = buffer;
        }
        else if (buffer->tokentraits.identifier)
        {
            if (tk.tokensleft())
            {
                tk.prestrip();
                ssbuff = tk.peek();
                traits t  = gettraits(ssbuff);
                if (t.roundbracket && t.left)
                {
                    // std::cout << "Detected fcall for: ";
                    // buffer->print();
                    // std::cout << "\n";
                    result<astnode> res = processfcall(tk, buffer);
                    if (!res.ok) return res;
                    buffer = res.value;
                }
                
                buffer->parent = current;
                current->children.postpend(buffer);
                
            }
            else
            return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
        }
        else if (buffer->tokentraits.roundbracket)
        {
            if (buffer->tokentraits.left)
            {
                nbracks++;

                holder = new astnode;
                holder->tokentraits.expression = 1;
                holder->parent = current;

                current->children.postpend(holder);
                current = holder;
            }
            else if (buffer->tokentraits.right)
            {
                nbracks--;
                while (!current->tokentraits.expression) current = current->parent;
                current = current->parent;                
            }            
        }
        else
        {
            buffer->parent = current;
            current->children.postpend(buffer);
        }

        if (tk.tokensleft()) ssbuff = tk.gettoken();
        else break;
        buffer = new astnode(ssbuff);
        // buffer->print();
    }
    
    // std::cout << "\n\nSUPREME " << (tk.cursor == NULL);
    // buffer->print();
    // std::cout << "\n\n";
    if (tk.cursor != NULL) tk.cursor = tk.cursor->prev;
    else tk.cursor = tk.tokens.end;

    ssbuff = tk.peek();
    // std::cout << "\n\nSUPREME " << (tk.cursor == NULL);
    // print(ssbuff);
    // std::cout << "\n\n";
    traits t = gettraits(ssbuff);
    if (!(
        t.statementterminator ||
        t.right || 
        (t.left && t.brace)
    )) tk.cursor = tk.cursor->next;

    while (true)
    {
        if (
            (expressiontoken->children.length == 1) && 
            (expressiontoken->children.root->val->tokentraits.expression)
        )
        {
            expressiontoken = expressiontoken->children.root->val;
            delete expressiontoken->parent;
            expressiontoken->parent = NULL;
        }
        else break;
    }
    return result<astnode>(expressiontoken);
}


/// @brief processes a function call from the beginning of the tokeniser
/// @param tk tokenizer (after the identifier token has been taken)
/// @param idtok aforementioned identifier token
/// @return an fcall token
result<astnode> processfcall(tokenizer &tk, astnode *idtok)
{
    tk.prestrip();

    stringslice ssbuff = tk.gettoken();
    astnode *buffer = new astnode(ssbuff);
    traits t;

    // std::cout << ""

    if (!(buffer->tokentraits.roundbracket && buffer->tokentraits.left))
    {
        dstring emsg;
            
        size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nExpected `(`, got`");
        emsg.append(stringof(buffer->tokenstr));
        emsg.append("`!");

        return result<astnode>(error(255, emsg.getstring()));
    }

    ll<astnode> params;

    result<astnode> tempres;
    while (true)
    {
        tk.prestrip();
        tempres = processexpression(tk);
        if (!tempres.ok) return tempres;

        params.postpend(tempres.value);

        tk.prestrip();
        ssbuff = tk.peek();
        t = gettraits(ssbuff);
        
        if (t.commasep)
        {
            tk.gettoken();
            tk.prestrip();
            continue;
        }
        else if (t.roundbracket && t.right)
        {
            tk.gettoken();
            break;
        }
        else if (!(
            t.identifier ||
            t.ifkeyword ||
            t.numeric ||
            t.stringliteral
            // t.?
        ))
        {
            dstring emsg;
            
            size_t ln = getlinenumber(tk.file, ssbuff.start);

            emsg.append("Syntax error in `");
            emsg.append((char*) tk.filepath);
            emsg.append("` at line ");
            emsg.append(tostring(ln + 1));
            emsg.append(":\n\n");
            emsg.append(getlineat(tk.file, tk.filesize, ln));
            emsg.append("\n\nExpected `,` or `)` or value, got`");
            emsg.append(stringof(ssbuff));
            emsg.append("`!");

            return result<astnode>(error(255, emsg.getstring()));
        }
    }

    astnode *fcalltok = new astnode();
    fcalltok->tokentraits.fcall = 1;
    fcalltok->tokentraits.expression = 1;

    idtok->parent = fcalltok;
    fcalltok->children.postpend(idtok);

    for (llnode<astnode> *lln = params.root; lln != NULL; lln = lln->next)
    {
        lln->val->parent = fcalltok;
        fcalltok->children.postpend(lln->val);
    }
    
    return result<astnode>(fcalltok);
}

/// @brief processes a classic style function definition from the beginning of the tokenizer
/// @return a funcdef token
result<astnode> processfuncdef(tokenizer &tk)
{
    tk.prestrip();
    stringslice ssbuff = tk.peek();
    astnode *buffer, *fundeftoken = new astnode();
    traits t;

    fundeftoken->tokentraits.functiondeclaration = 1;

    if (!gettraits(ssbuff).functiondeclarator)
    {
        dstring emsg;
            
        size_t ln = getlinenumber(tk.file, ssbuff.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nExpected `fun`, got`");
        emsg.append(stringof(ssbuff));
        emsg.append("`!");

        return result<astnode>(error(255, emsg.getstring()));
    }
    else tk.gettoken();

    tk.prestrip();
    ssbuff = tk.gettoken();
    buffer = new astnode(ssbuff);

    if (!buffer->tokentraits.identifier)
    {
        dstring emsg;
            
        size_t ln = getlinenumber(tk.file, ssbuff.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nExpected an identifier, got`");
        emsg.append(stringof(ssbuff));
        emsg.append("`!");

        return result<astnode>(error(255, emsg.getstring()));
    }
    else
    {
        buffer->parent = fundeftoken;
        fundeftoken->children.postpend(buffer);
    }

    tk.prestrip();
    ssbuff = tk.gettoken();
    t = gettraits(ssbuff);

    if (!(t.roundbracket && t.left))
    {
        dstring emsg;
            
        size_t ln = getlinenumber(tk.file, ssbuff.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nExpected `(`, got`");
        emsg.append(stringof(ssbuff));
        emsg.append("`!");

        return result<astnode>(error(255, emsg.getstring()));
    }

    astnode *fargtok = new astnode();
    fargtok->tokentraits.argument = 1;

    //this block deals with all arguments in the thing
    while (true)
    {
        tk.prestrip();
        ssbuff = tk.gettoken();
        t = gettraits(ssbuff);
        if (t.roundbracket && t.right)
        {
            if (fargtok->children.length > 0)
            {
                fargtok->parent = fundeftoken;
                fundeftoken->children.postpend(fargtok);
                if (fargtok->children.root->val->tokentraits.identifier)
                fargtok->children.root->val->tokentraits.datatype = 1;
            }
            break;
        }

        if (!(t.identifier || t.clonekeyword || t.commasep))
        {
            dstring emsg;
            
            size_t ln = getlinenumber(tk.file, ssbuff.start);

            emsg.append("Syntax error in `");
            emsg.append((char*) tk.filepath);
            emsg.append("` at line ");
            emsg.append(tostring(ln + 1));
            emsg.append(":\n\n");
            emsg.append(getlineat(tk.file, tk.filesize, ln));
            emsg.append("\n\nExpected identifier or `,`, got`");
            emsg.append(stringof(ssbuff));
            emsg.append("`!");

            return result<astnode>(error(255, emsg.getstring()));
        }
    
        if (t.clonekeyword) fargtok->tokentraits.clone = 1;
        else if (t.identifier)
        {
            buffer = new astnode(ssbuff);
            buffer->parent = fargtok;
            fargtok->children.postpend(buffer);
        }
        else
        {
            if (fargtok->children.root->val->tokentraits.identifier)
            fargtok->children.root->val->tokentraits.datatype = 1;

            fargtok->parent = fundeftoken;
            fundeftoken->children.postpend(fargtok);

            fargtok = new astnode();
            fargtok->tokentraits.argument = 1;
        }
    }

    tk.prestrip();
    ssbuff = tk.gettoken();
    buffer = new astnode(ssbuff);

    if (!(
        buffer->tokentraits.colon ||
        (buffer->tokentraits.brace && buffer->tokentraits.left) 
    ))
    {
        dstring emsg;
            
        size_t ln = getlinenumber(tk.file, ssbuff.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nExpected `:` or `{`, got`");
        emsg.append(stringof(ssbuff));
        emsg.append("`!");

        return result<astnode>(error(255, emsg.getstring()));
    }

    

    if (buffer->tokentraits.colon)
    {
        while (true)
        {
            tk.prestrip();
            ssbuff = tk.gettoken();
            buffer = new astnode(ssbuff);

            if (!(buffer->tokentraits.identifier ||
            (buffer->tokentraits.brace && buffer->tokentraits.left) || 
            buffer->tokentraits.commasep
            ))
            {
                dstring emsg;
                    
                size_t ln = getlinenumber(tk.file, ssbuff.start);

                emsg.append("Syntax error in `");
                emsg.append((char*) tk.filepath);
                emsg.append("` at line ");
                emsg.append(tostring(ln + 1));
                emsg.append(":\n\n");
                emsg.append(getlineat(tk.file, tk.filesize, ln));
                emsg.append("\n\nExpected return type after `:`, got`");
                emsg.append(stringof(ssbuff));
                emsg.append("`!");

                return result<astnode>(error(255, emsg.getstring()));
            }

            if (buffer->tokentraits.commasep) continue;

            if (buffer->tokentraits.brace && buffer->tokentraits.left)
            {
                if (tk.cursor != NULL) tk.cursor = tk.cursor->prev;
                else tk.cursor = tk.tokens.end;
                break;
            }

            buffer->tokentraits.freturntype = 1;
            buffer->tokentraits.datatype = 1;
            buffer->parent = fundeftoken;
            fundeftoken->children.postpend(buffer);
        }

        ssbuff = tk.peek();
        t = gettraits(ssbuff);
    }
    else
    {
        if (tk.cursor != NULL) tk.cursor = tk.cursor->prev;
        else tk.cursor = tk.tokens.end;
    }

    // tk.prestrip();
    ssbuff = tk.peek();
    t = gettraits(ssbuff);
    
    if (!(t.brace && t.left))
    {
        std::cout << "Trigged here\n";
        dstring emsg;
                    
        size_t ln = getlinenumber(tk.file, ssbuff.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nExpected `{`, got`");
        emsg.append(stringof(ssbuff));
        emsg.append("`!");

        return result<astnode>(error(255, emsg.getstring()));
    }

    //make it so that this processscope also finds an freturntype
    result<astnode> res = processscope(tk);
    if (!res.ok) return res;

    res.value->parent = fundeftoken;
    fundeftoken->children.postpend(res.value);

    return result<astnode>(fundeftoken);
}

/// @brief processes a scope (enclosed by {}) from the beginning of the tokenizer
/// @return a body token
result<astnode> processscope(tokenizer &tk)
{
    tk.prestrip();
    if (!tk.tokensleft())
    return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));

    stringslice ssbuff = tk.gettoken();
    traits t = gettraits(ssbuff);
    astnode buffer, *bodytoken, *holder;
    result<astnode> res;

    if (!(t.brace && t.left))
    {
        dstring emsg;
                    
        size_t ln = getlinenumber(tk.file, ssbuff.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nExpected `{`, got`");
        emsg.append(stringof(ssbuff));
        emsg.append("`!");

        return result<astnode>(error(237, emsg.getstring()));
    }

    bodytoken = new astnode();
    bodytoken->tokentraits.body = 1;

    while (true)
    {
        tk.prestrip();
        ssbuff = tk.peek();
        buffer = astnode(ssbuff);

        if (buffer.tokentraits.brace && buffer.tokentraits.right)
        {
            tk.gettoken();
            std::cout << "here\n";
            break;
        }

        //come back here bruh
        if (buffer.tokentraits.functiondeclarator) res = processfuncdef(tk);
        else if (buffer.tokentraits.cflowkeyword) res = processcflow(tk);
        else res = processstatement(tk);

        if (!res.ok) return res;

        res.value->parent = bodytoken;
        bodytoken->children.postpend(res.value);

    }

    std::cout << "while leaving scope: ";
    ssbuff = tk.peek();
    print(ssbuff);
    std::cout << "\n";

    return result<astnode>(bodytoken);
}

/// @brief processes control flow structures from the beginning of the tokenizer
/// IMPORTANT: TODO: implement for loops
/// @return a controlflow token
result<astnode> processcflow(tokenizer &tk)
{
    stringslice ssbuff;
    traits t;
    result<astnode> res;
    astnode *buffer, *cflowtok = new astnode();
    cflowtok->tokentraits.controlflow = 1;

    tk.prestrip();
    if (tk.tokensleft()) ssbuff = tk.gettoken();
    else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));

    t = gettraits(ssbuff);

    if (t.ifkeyword)
    {
        cflowtok->tokentraits.ifblock = 1;
        //if block stuff
        tk.prestrip();

        res = processexpression(tk);
        if (!res.ok) return res;

        res.value->parent = cflowtok;
        cflowtok->children.postpend(res.value);

        ssbuff = tk.peek();
        while (!equal(ssbuff, "{"))
        {
            if (tk.cursor->prev != NULL) tk.cursor = tk.cursor->prev;
            ssbuff = tk.peek();
        }

        res = processscope(tk);
        if (!res.ok) return res;

        res.value->parent = cflowtok;
        cflowtok->children.postpend(res.value);

        tk.prestrip();
        while (tk.tokensleft())
        {
            tk.prestrip();
            ssbuff = tk.peek();
            t = gettraits(ssbuff);
            
            if (!(t.elifkeyword || t.elsekeyword)) break;
            tk.gettoken();
            
            buffer = new astnode();
            buffer->tokentraits.controlflow = 1;
            if (t.elifkeyword) buffer->tokentraits.elifblock = 1;            
            else buffer->tokentraits.elseblock = 1;            

            if (t.elifkeyword)
            {
                tk.prestrip();
                res = processexpression(tk);
                if (!res.ok) return res;

                res.value->parent = buffer;
                buffer->children.postpend(res.value);
            }

            tk.prestrip();
            std::cout << "Before PS: ";
            ssbuff = tk.peek();
            print(ssbuff);
            std::cout << "\n";

            res = processscope(tk);
            if (!res.ok) return res;

            res.value->parent = buffer;
            buffer->children.postpend(res.value);
            
            buffer->parent = cflowtok;
            cflowtok->children.postpend(buffer);

            if (t.elsekeyword) break;
        }
    }
    else if (t.whilekeyword)
    {
        cflowtok->tokentraits.whileblock = 1;

        tk.prestrip();

        res = processexpression(tk);
        if (!res.ok) return res;

        res.value->parent = cflowtok;
        cflowtok->children.postpend(res.value);

        ssbuff = tk.peek();
        while (!equal(ssbuff, "{"))
        {
            if (tk.cursor->prev != NULL) tk.cursor = tk.cursor->prev;
            ssbuff = tk.peek();
        }

        res = processscope(tk);
        if (!res.ok) return res;

        res.value->parent = cflowtok;
        cflowtok->children.postpend(res.value);

    }
    else if (t.forkeyword)
    {
        cflowtok->tokentraits.forblock = 1;
        //to do for loops
    }
    else
    {
        dstring emsg;
                    
        size_t ln = getlinenumber(tk.file, ssbuff.start);

        emsg.append("Syntax error in `");
        emsg.append((char*) tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(ln + 1));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        emsg.append("\n\nUnxpected token, got`");
        emsg.append(stringof(ssbuff));
        emsg.append("`!");

        return result<astnode>(error(255, emsg.getstring()));
    }

    return result<astnode>(cflowtok);
}

/// @brief Breadth first traversal and printing of nodes of the tree.
/// Nodes are separated by a '---' and 'x' marks the end of a single row in the tree.
void ast::printbf()
{
    ll<astnode> q, p;
    q.postpend(this->root);

    astnode *buffer;

    bool flag = true;

    while (!(q.isempty() && p.isempty()))
    {
        // std::cout << "newcycle";
        if (flag)
        {
            // std::cout <<"entered there";
            buffer = q.prepop();
            // std::cout << "there";
            buffer->print();
            std::cout << "---";

            if ((!buffer->children.isempty())) p.append(&buffer->children);
            if (q.isempty())
            {
                std::cout << "x\n\n";
                flag = false;
            }
        }
        else
        {
            // std::cout <<"entered here";
            buffer = p.prepop();
            // std::cout << "here";
            buffer->print();
            std::cout << "---";

            if ((!buffer->children.isempty())) q.append(&buffer->children);
            if (p.isempty())
            {
                std::cout << "x\n\n";
                flag = true;
            }
        }
    }


    std::cout << "END OF AST\n";
}
