#pragma once

#include "newtok.h"
#include "./../general/ll.h"
#include "./../general/traits.h"
#include "./../general/strings.h"

/// @brief node that stores a token in the AST.
class astnode
{
public:
    astnode();
    astnode(token *tk);
    ~astnode();

    traits tokentraits;
    ll<astnode> children;
    token *tok = NULL;
    astnode *parent = NULL;

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
astnode::astnode(token *tk)
{
    this->tok = tk;
    this->tokentraits = tk->props;
    this->parent = NULL;
}

/// @brief prints the given AST node to standard output in the format [*token string*](*token traits*)
void astnode::print()
{
    std::cout << "[";
    if (this->tok != NULL) ::print(this->tok->tokenstr);
    std::cout << "]" << "(" << this->tokentraits << this->tok->linenumber << ")";
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

ast::~ast() {}


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

    // result<size_t> res = this->tk->read();
    // if (!res.ok)
    //     return result<void>(res.err);
    _consumecast(this->tk->read(), void)
    _consumecast(this->tk->parse(), void)

    // res = this->tk->parse();
    // if (!res.ok)
    //     return result<void>(res.err);
    
    stringslice ssbuff;
    traits t;

    result<token> tokres;
    result<astnode> noderes;

    token *currtoken;
    // astnode *bruh;


    //main loop
    while (this->tk->available())
    {
        tk->prestrip();
        if (this->tk->available())
        {
            tokres = tk->peek();
            if (!tokres.ok)
                return result<void>(tokres.err);
        }
        else break;

        currtoken = tokres.value;
     
        if (currtoken->props.functiondeclarator) noderes = processfuncdef(*tk);
        // else if (t.cflowkeyword) noderes = processcflow(*tk);
        // else noderes = processstatement(*tk);

        // if (!noderes.ok) return result<void>(noderes.err);
        // noderes.value->parent = tutok;
        // tutok->children.postpend(noderes.value);
        // this->tk->gettoken().value->print();
        // std::cout << "\n";
    }

    
    // this->root = tutok;
    
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

    result<token> tres;
    token *currtoken;

    tk.prestrip();

    if (!tk.available())
        return result<astnode>(255, "Unexpected EOF when trying to parse statement!");
    else
    {
        tres = tk.gettoken();
        if (!tres.ok)
            return result<astnode>(tres.err);
        currtoken = tres.value;
    }

    //setting up some vars
    astnode *statementnode = new astnode;
    statementnode->tokentraits.blob = 1;
    statementnode->tokentraits.expression = 1;
    astnode *buffer;
    astnode *dt = NULL, *id = NULL, *rvalue = NULL, *lvalue = NULL;
    
    
    //variable decl stuff
    if (currtoken->props.vardeclarator)
    {
        // tk.gettoken();
        statementnode->tokentraits.vardeclaration = 1;
        if (currtoken->props.constdeclarator) statementnode->tokentraits.isconst = 1;

        tk.prestrip();
        if (tk.available())
        {
            tres = tk.gettoken();
            if (!tres.ok)
                return result<astnode>(tres.err);
        }
        else
            return result<astnode>(254, "Unexpected EOF when trying to parse statement!");

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
        statementnode->tokentraits.varassign = gettraits(ssbuff).assigner;

        if (statementnode->tokentraits.isconst)
        {
            if (!statementnode->tokentraits.varassign)
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


        if ((dt == NULL) && (!statementnode->tokentraits.varassign))
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
            dt->parent = statementnode;
            statementnode->children.postpend(dt);
        }

        id->tokentraits.identifier = 1;
        id->tokentraits.datatype = 0;
        id->parent = statementnode;
        statementnode->children.postpend(id);
    }

    //check if any kind of assignment is being made at all
    if (!statementnode->tokentraits.varassign)
    {
        llnode<stringslice> *holder = tk.cursor;
        traits t;
        while (holder != NULL)
        {
            t = gettraits(*holder->val);
            if (t.statementterminator) break;
            if (t.assigner)
            {
                statementnode->tokentraits.varassign = 1;
                break;
            }
            holder = holder->next;
        }
    }

    if (
        statementnode->tokentraits.varassign &&
        !statementnode->tokentraits.vardeclaration
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
        statementnode->children.postpend(buffer);
    }
    
    
    tk.prestrip();
    ssbuff = tk.peek();
    t = gettraits(ssbuff);
    if (t.assigner) tk.gettoken();
    else if (t.rkeyword)
    {
        tk.gettoken();
        statementnode->tokentraits.returner = 1;
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
        statementnode->tokentraits.varassign &&
        (res.value->children.length < 1)
    )
    {
        dstring emsg;

        size_t ln = getlinenumber(tk.file, statementnode->children.end->val->tokenstr.start);

        emsg.append("Empty assignment at line ");
        emsg.append(tostring(ln));
        emsg.append(":\n\n");
        emsg.append(getlineat(tk.file, tk.filesize, ln));
        // emsg.append("\nUndefined variable `");
        // emsg.append(stringof(statementnode->children.end->val->tokenstr));
        // emsg.append("`!");

        return result<astnode>(error(244, emsg.getstring()));
    }

    res.value->tokentraits.rvalue = 1;
    res.value->tokentraits.expression = 1;
    rvalue = res.value;
    //here boi

    rvalue->parent = statementnode;
    statementnode->children.postpend(rvalue);
    

    while (tk.tokensleft())
    {
        tk.prestrip();
        if (tk.tokensleft()) ssbuff = tk.gettoken();
        else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
        t = gettraits(ssbuff);
        if (t.statementterminator)
        {
            statementnode->tokentraits.statement = 1;
            statementnode->tokentraits.expression = 0;
            break;
        }
        else if (t.brace && t.right)
        {
            if (!(tk.cursor == NULL)) tk.cursor = tk.cursor->prev;
            else tk.cursor = tk.tokens.end;
            delete statementnode;
            rvalue->parent = NULL;
            return result<astnode>(rvalue);
        }
    }
    

    return result<astnode>(statementnode);
}





