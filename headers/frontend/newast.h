#pragma once

#include <limits>
#include "newtok.h"
#include "./../general/ll.h"
#include "./../general/traits.h"
#include "./../general/strings.h"

const bool truebool = true;
const bool falsebool = false;

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
    if ((this->tok != NULL)) ::print(this->tok->tokenstr);
    std::cout << "]" << "(" << this->tokentraits;
    if (this->tok != NULL) std::cout << this->tok->linenumber;
    std::cout << ")(" << this->children.length <<  ")";
}

result<astnode> processstatement(tokenizer &tk);
result<astnode> processexpression(tokenizer &tk) {
    result<token> tres = tk.gettoken();
    _validatecast(tres, astnode)

    while (!(tres.value->props.roundbracket && tres.value->props.right))
    {
        tres = tk.gettoken();
        _validatecast(tres, astnode)
    }

    astnode *placeholder = new astnode;
    placeholder->tokentraits.blob = 1;
    return result<astnode>(placeholder);
}
result<astnode> processfcall(tokenizer &tk, astnode *idtok);
result<astnode> processfuncdef(tokenizer &tk) {}
result<astnode> processrval(tokenizer &tk) {
    astnode *huh = new astnode;
    huh->tokentraits.rvalue = 1;
    return result<astnode>(huh);
}
result<astnode> processscope(tokenizer &tk);
result<astnode> processcflow(tokenizer &tk);
result<astnode> processtype(tokenizer &tk);
result<astnode> processtuple(tokenizer &tk);

string getlineat(tokenizer &tk, size_t ln)
{
    std::ifstream file(tk.filepath, std::ios::in);

    char buffer[1024] = { 0 };
    while (ln > 1)
    {
        *buffer = file.get();
        if (*buffer == '\n') ln--;
    }
    
    file.getline(buffer, 1024);

    return clonestring_nt(buffer);
}



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

    _consumecast(this->tk->read(), void)
    _consumecast(this->tk->parse(), void)


    // stringslice ssbuff;
    // traits t;

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
            _validatecast(tokres, void)
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



/// @brief Checks if the roundbracket pair represents a tuple. 
/// DOES NOT ADVANCE the token sequence.
/// @param start the node from which to check ( start must point to a `(` )
result<bool> istuple(tokenizer &tk, llnode<token> *start)
{
    if (!(start->val->props.roundbracket && start->val->props.left))
    {
        dstring emsg;

        emsg.append("Expected `(`, got `");
        emsg.append(start->val->tokenstr);
        emsg.append("` while checking for tuples!");

        return result<bool>(253, emsg.getstring());
    }

    start = start->next;
    size_t nrb = 0;
    while (true)
    {
        if (start == NULL)
            return result<bool>(254, "Unexpected EOF!");
        
        else if (start->val->props.roundbracket && start->val->props.left)
            nrb++;
        
        else if (start->val->props.roundbracket && start->val->props.right)
        {
            if (!nrb) return result<bool>((bool*) &falsebool);
            else nrb--;
        }
        
        else if (start->val->props.commasep && !nrb)
            return result<bool>((bool*) &truebool);
        
        start = start->next;
    }
}

/// @brief checks if the bracketed part is indeed an ftype or not
result<bool> isftype(tokenizer &tk, llnode<token> *start)
{
    if (!(start->val->props.roundbracket && start->val->props.left))
    {
        dstring emsg;

        emsg.append("Expected `(`, got `");
        emsg.append(start->val->tokenstr);
        emsg.append("` while checking for ftype!");

        return result<bool>(253, emsg.getstring());
    }

    start = start->next;
    size_t nrb = 0;
    while (true)
    {
        if (start == NULL)
            return result<bool>(254, "Unexpected EOF!");
        
        else if (start->val->props.roundbracket && start->val->props.left)
            nrb++;
        
        else if (start->val->props.roundbracket && start->val->props.right)
        {
            if (!nrb) return result<bool>((bool*) &falsebool);
            else nrb--;
        }
        
        else if (start->val->props.colon)
            return result<bool>((bool*) &truebool);
        
        start = start->next;
    }
}

result<astnode> processftype(tokenizer &tk);

/// @brief processes a tuple
result<astnode> processtuple(tokenizer &tk)
{
    tk.prestrip();
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.roundbracket && tokres.value->props.left))
    {
        dstring emsg;

        string errline = getlineat(tk, tokres.value->linenumber);
        strip(errline);

        emsg.append("In file `");
        emsg.append(tk.filepath);
        emsg.append("` at line ");
        emsg.append(tostring(tokres.value->linenumber));
        emsg.append(":\n\n\t");
        emsg.append(errline);
        emsg.append("\n\n");
        emsg.append("Expected `(` while parsing tuple, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("`!");

        return result<astnode>(253, emsg.getstring());
    }

    tk.gettoken();

    ll<astnode> items;

    size_t nrb = 0;
    while (true)
    {
        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        if (tokres.value->props.roundbracket)
        {
            if (tokres.value->props.right)
            {
                if (nrb == 0) break;
            }
            else
            {
                tk.nexttok = tk.nexttok->prev;

                bres = istuple(tk, tk.nexttok);
                _validatecast(bres, astnode)

                if (*bres.value) noderes = processtuple(tk);
                else
                {
                    bres = isftype(tk, tk.nexttok);
                    _validatecast(bres, astnode)

                    if (*bres.value)
                        noderes = processftype(tk);
                    else
                        noderes = processexpression(tk);
                }
                _validate(noderes)

                items.postpend(noderes.value);
            }
        }
        else if (!tokres.value->props.commasep)
            items.postpend(new astnode(tokres.value));
    }

    astnode *tuplenode = new astnode;
    tuplenode->children = items;
    tuplenode->tokentraits.tuple = 1;
    tuplenode->tok = NULL;
    return result<astnode>(tuplenode);
}


/// @brief processes an ftype
result<astnode> processftype(tokenizer &tk)
{
    tk.prestrip();
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.roundbracket && tokres.value->props.left))
        return result<astnode>(253, "Expecting `(` while processing ftype!");
    
    tk.gettoken();

    ll<astnode> inputs, outputs;

    while (true)
    {
        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        if (tokres.value->props.colon)
            break;
        
        else if (tokres.value->props.identifier)
        {
            inputs.postpend(new astnode(tokres.value));

            tk.prestrip();
            tokres = tk.peek();
            _validatecast(tokres, astnode);

            if (!(tokres.value->props.commasep || tokres.value->props.colon))
            {
                dstring emsg;

                emsg.append("\n");
                emsg.append(getlineat(tk, tokres.value->linenumber));
                emsg.append("\nExpected `,` or `:` after datatype `");
                emsg.append(inputs.end->val->tok->tokenstr);
                emsg.append("`, instead got `");
                emsg.append(tokres.value->tokenstr);
                emsg.append("`!");

                return result<astnode>(253, emsg.getstring());
            }
        }

        else if (tokres.value->props.commasep) {}

        else if ((tokres.value->props.roundbracket))
        {
            if (tokres.value->props.right)
            {
                dstring emsg;

                emsg.append("\n");
                emsg.append(getlineat(tk, tokres.value->linenumber));
                emsg.append("Unexpected `)` in ftype!");

                return result<astnode>(253, emsg.getstring());
            }
        
            if (tk.nexttok != NULL) tk.nexttok = tk.nexttok->prev;
            bres = istuple(tk, tk.nexttok);
            _validatecast(bres, astnode)

            if (*bres.value)
            {
                noderes = processtuple(tk);
                _validate(noderes)

                inputs.postpend(noderes.value);
            }
            else
            {
                bres = isftype(tk, tk.nexttok);
                _validatecast(bres, astnode)

                if (*bres.value)
                {
                    noderes = processftype(tk);
                    _validate(noderes)

                    inputs.postpend(noderes.value);
                }
                else
                {
                    dstring emsg;

                    emsg.append("\n");
                    emsg.append(getlineat(tk, tk.nexttok->val->linenumber));
                    emsg.append("\nInvalid roundbrackets in ftype!");

                    return result<astnode>(253, emsg.getstring());
                }
            }
        }
    
        // else if () clone, ptr keyword stuff goes here

        else
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("Unexpected token `");
            emsg.append(tokres.value->tokenstr);
            // emsg.append("`, instead got `");
            // emsg.append(tokres.value->tokenstr);
            emsg.append("` while processing ftype!");

            return result<astnode>(253, emsg.getstring());
        }
    }

    while (true)
    {
        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        if (tokres.value->props.roundbracket && tokres.value->props.right)
            break;
        
        else if (tokres.value->props.identifier)
        {
            outputs.postpend(new astnode(tokres.value));

            tk.prestrip();
            tokres = tk.peek();
            _validatecast(tokres, astnode);

            if (!(
                tokres.value->props.commasep ||
                (tokres.value->props.right && tokres.value->props.roundbracket)
                ))
            {
                dstring emsg;

                emsg.append("\n");
                emsg.append(getlineat(tk, tokres.value->linenumber));
                emsg.append("\nExpected `,` or `)` after datatype `");
                emsg.append(inputs.end->val->tok->tokenstr);
                emsg.append("`, instead got `");
                emsg.append(tokres.value->tokenstr);
                emsg.append("`!");

                return result<astnode>(253, emsg.getstring());
            }
        }

        else if (tokres.value->props.commasep) {}

        else if ((tokres.value->props.roundbracket))
        {
            if (tokres.value->props.right)
                break;
        
            if (tk.nexttok != NULL) tk.nexttok = tk.nexttok->prev;
            bres = istuple(tk, tk.nexttok);
            _validatecast(bres, astnode)

            if (*bres.value)
            {
                noderes = processtuple(tk);
                _validate(noderes)

                outputs.postpend(noderes.value);
            }
            else
            {
                bres = isftype(tk, tk.nexttok);
                _validatecast(bres, astnode)

                if (*bres.value)
                {
                    noderes = processftype(tk);
                    _validate(noderes)

                    outputs.postpend(noderes.value);
                }
                else
                {
                    dstring emsg;

                    emsg.append("\n");
                    emsg.append(getlineat(tk, tk.nexttok->val->linenumber));
                    emsg.append("\nInvalid roundbrackets in ftype!");

                    return result<astnode>(253, emsg.getstring());
                }
            }
        }

        else
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("Unexpected token `");
            emsg.append(tokres.value->tokenstr);
            // emsg.append("`, instead got `");
            // emsg.append(tokres.value->tokenstr);
            emsg.append("` while processing ftype!");

            return result<astnode>(253, emsg.getstring());
        }
    }


    astnode *inp = new astnode, *out = new astnode, *ftypenode = new astnode;
    inp->children = inputs;
    out->children = outputs;
    inp->parent = ftypenode;
    out->parent = ftypenode;

    inp->tokentraits.fins = 1;
    out->tokentraits.fouts = 1;

    ftypenode->children.postpend(inp)->postpend(out);
    ftypenode->tokentraits.ftype = 1;

    return result<astnode>(ftypenode);
}



/// @brief processes any type
result<astnode> processtype(tokenizer &tk)
{
    tk.prestrip();

    result<token> tokres;
    result<bool> bres;
    result<astnode> noderes;

    astnode *dt = NULL;

    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.identifier)
    {
        tk.gettoken();
        dt = new astnode(tokres.value);
    }
    else if (tokres.value->props.roundbracket && tokres.value->props.left)
    {
        bres = istuple(tk, tk.nexttok);
        _validatecast(bres, astnode)

        if (*bres.value)
        {
            noderes = processtuple(tk);
            _validate(noderes)

            dt = new astnode;
            dt->children = (noderes.value->children);
            dt->tokentraits.tuple = 1;
        }
        else
        {
            bres = isftype(tk, tk.nexttok);
            _validatecast(bres, astnode)

            if (*bres.value)
            {
                noderes = processftype(tk);
                _validate(noderes)

                dt = noderes.value;
            }
            else
            {
                dstring emsg;

                emsg.append("\n");
                emsg.append(getlineat(tk, tk.nexttok->val->linenumber));
                emsg.append("\n\nInvalid type/identifier specification; got unexpected token `");
                emsg.append(tk.nexttok->val->tokenstr);
                emsg.append("`!");

                return result<astnode>(252, emsg.getstring());
            }
        }
    }
    else
    {
        dstring emsg;

        emsg.append("\n");
        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nUnexpected token `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("`!");
        
        return result<astnode>(253, emsg.getstring());
    }

    dt->tokentraits.datatype = 1;
    return result<astnode>(dt);
}

/// @brief processes a statement
result<astnode> processstatement(tokenizer &tk)
{
    result<token> tokres;
    result<astnode> noderes;
    token *currtoken;

    tk.prestrip();

    if (!tk.available())
        return result<astnode>(255, "Unexpected EOF when trying to parse statement!");
    else
    {
        tokres = tk.peek();
        _validatecast(tokres, astnode)
        currtoken = tokres.value;
    }

    astnode *var = NULL, *type = NULL, *rval = NULL;
    astnode *statementnode = new astnode;
    statementnode->tokentraits.statement = 1;

    // dealing with variable declaration stuff
    if (currtoken->props.vardeclarator)
    {
        tk.gettoken();

        statementnode->tokentraits.vardeclaration = 1;

        noderes = processtype(tk);
        _validate(noderes)

        type = noderes.value;

        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (!(
            tokres.value->props.identifier ||
            tokres.value->props.assigner ||
            (tokres.value->props.roundbracket && tokres.value->props.left) ||
            tokres.value->props.statementterminator
        ))
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\nExpecting identifier, `=` or `;` after `");
            emsg.append(type->tok->tokenstr);
            emsg.append("`, got `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` instead!");

            return result<astnode>(253, emsg.getstring());
        }

        if (tokres.value->props.roundbracket && tokres.value->props.left)
        {
            noderes = processtuple(tk);
            _validate(noderes)

            var = noderes.value;
        }
        else if (tokres.value->props.identifier)
        {
            tokres = tk.gettoken();
            _validatecast(tokres, astnode)

            var = new astnode(tokres.value);
        }
        else
        {
            var = type;
            var->tokentraits.datatype = 0;
            type = NULL;
        }
    }

    if (var == NULL)
    {
        var = type;
        type = NULL;
        var->tokentraits.datatype = 0;
    }
    
    tk.prestrip();
    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    std::cout << "At this point: ";
    tokres.value->print();
    std::cout << "\n";

    if (tokres.value->props.assigner)
        statementnode->tokentraits.varassign = 1;

    /*
        TODO: 
        implement rvalue stuff here
        placeholder stuff for now
    */

    noderes = processrval(tk);
    _validate(noderes)

    rval = noderes.value;


    if (type != NULL)
    {
        type->parent = statementnode;
        statementnode->children.postpend(type);
    }
    if (var != NULL)
    {
        var->parent = statementnode;
        statementnode->children.postpend(var);
    }
    if (rval != NULL)
    {
        rval->parent = statementnode;
        statementnode->children.postpend(rval);
    }

    return result<astnode>(statementnode);
}



/// @brief processes a statement starting from the beginning of the tokenizer.
/// @return a statement token after processing
// result<astnode> processstatement(tokenizer &tk)
// {
//     // stringslice ssbuff;
//     // astnode *cursor;
//     // traits t;

//     result<token> tres;
//     token *currtoken;

//     tk.prestrip();

//     if (!tk.available())
//         return result<astnode>(255, "Unexpected EOF when trying to parse statement!");
//     else
//     {
//         tres = tk.gettoken();
//         _validatecast(tres, astnode)
//         currtoken = tres.value;
//     }

//     //setting up some vars
//     astnode *statementnode = new astnode;
//     statementnode->tokentraits.blob = 1;
//     statementnode->tokentraits.expression = 1;
//     astnode *buffer;
//     astnode *dt = NULL, *id = NULL, *rvalue = NULL, *lvalue = NULL;
    
    
//     //variable decl stuff
//     if (currtoken->props.vardeclarator)
//     {
//         // tk.gettoken();
//         statementnode->tokentraits.vardeclaration = 1;
//         if (currtoken->props.constdeclarator) statementnode->tokentraits.isconst = 1;

//         tk.prestrip();
//         if (tk.available())
//         {
//             tres = tk.gettoken();
//             if (!tres.ok)
//                 return result<astnode>(tres.err);
//         }
//         else
//             return result<astnode>(254, "Unexpected EOF when trying to parse statement!");

//         if (buffer->tokentraits.identifier)
//         {
//             dt = buffer;
//             dt->tokentraits.datatype = 1;
//         }
//         else
//         {
//             dstring emsg;
            
//             size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

//             emsg.append("Syntax error in `");
//             emsg.append((char*) tk.filepath);
//             emsg.append("` at line ");
//             emsg.append(tostring(ln + 1));
//             emsg.append(":\n\n");
//             emsg.append(getlineat(tk.file, tk.filesize, ln));
//             emsg.append("\n\nExpecting identifier/datatype, got`");
//             emsg.append(stringof(buffer->tokenstr));
//             emsg.append("`!");

//             return result<astnode>(error(255, emsg.getstring()));
//         }

//         tk.prestrip();
//         ssbuff = tk.peek();
        
//         if (gettraits(ssbuff).identifier)
//         {
//             tk.prestrip();
//             if (tk.tokensleft())
//             {    
//                 ssbuff = tk.gettoken();
//                 buffer = new astnode(ssbuff);
//             }
//             else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
            
//             id = buffer;
//         }
//         else
//         {
//             id = dt;

//             dt = NULL;

//             id->tokentraits.datatype = 0;
//         }


//         tk.prestrip();
//         ssbuff = tk.peek();
//         statementnode->tokentraits.varassign = gettraits(ssbuff).assigner;

//         if (statementnode->tokentraits.isconst)
//         {
//             if (!statementnode->tokentraits.varassign)
//             {
//                 dstring emsg;
            
//                 size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

//                 emsg.append("Syntax error in `");
//                 emsg.append((char*) tk.filepath);
//                 emsg.append("` at line ");
//                 emsg.append(tostring(ln + 1));
//                 emsg.append(":\n\n");
//                 emsg.append(getlineat(tk.file, tk.filesize, ln));
//                 emsg.append("\n\nConsts must be initialised when declared; Expected `=`!");

//                 return result<astnode>(error(255, emsg.getstring()));
//             }
//         }


//         if ((dt == NULL) && (!statementnode->tokentraits.varassign))
//         {
//             dstring emsg;
        
//             size_t ln = getlinenumber(tk.file, buffer->tokenstr.start);

//             emsg.append("Syntax error in `");
//             emsg.append((char*) tk.filepath);
//             emsg.append("` at line ");
//             emsg.append(tostring(ln + 1));
//             emsg.append(":\n\n");
//             emsg.append(getlineat(tk.file, tk.filesize, ln));
//             emsg.append("\n\nCannot infer datatype of `");
//             emsg.append(stringof(id->tokenstr));
//             emsg.append("` in declaration alone!");

//             return result<astnode>(error(255, emsg.getstring()));
//         }


//         if (dt != NULL)
//         {
//             dt->tokentraits.datatype = 1;
//             dt->tokentraits.identifier = 1;
//             dt->parent = statementnode;
//             statementnode->children.postpend(dt);
//         }

//         id->tokentraits.identifier = 1;
//         id->tokentraits.datatype = 0;
//         id->parent = statementnode;
//         statementnode->children.postpend(id);
//     }

//     //check if any kind of assignment is being made at all
//     if (!statementnode->tokentraits.varassign)
//     {
//         llnode<stringslice> *holder = tk.cursor;
//         traits t;
//         while (holder != NULL)
//         {
//             t = gettraits(*holder->val);
//             if (t.statementterminator) break;
//             if (t.assigner)
//             {
//                 statementnode->tokentraits.varassign = 1;
//                 break;
//             }
//             holder = holder->next;
//         }
//     }

//     if (
//         statementnode->tokentraits.varassign &&
//         !statementnode->tokentraits.vardeclaration
//     )
//     {
//         // std::cout << "An assignment is being made...\n";
//         tk.prestrip();
//         ssbuff = tk.peek();
//         t = gettraits(ssbuff);

//         if (!t.identifier)
//         {
//             dstring emsg;
        
//             size_t ln = getlinenumber(tk.file, ssbuff.start);

//             emsg.append("Syntax error in `");
//             emsg.append((char*) tk.filepath);
//             emsg.append("` at line ");
//             emsg.append(tostring(ln + 1));
//             emsg.append(":\n\n");
//             emsg.append(getlineat(tk.file, tk.filesize, ln));
//             emsg.append("\n\nExpecting identifier, got `");
//             emsg.append(stringof(id->tokenstr));
//             emsg.append("`!");

//             return result<astnode>(error(255, emsg.getstring()));
//         }
    
//         buffer = new astnode(ssbuff);
//         tk.gettoken();
//         statementnode->children.postpend(buffer);
//     }
    
    
//     tk.prestrip();
//     ssbuff = tk.peek();
//     t = gettraits(ssbuff);
//     if (t.assigner) tk.gettoken();
//     else if (t.rkeyword)
//     {
//         tk.gettoken();
//         statementnode->tokentraits.returner = 1;
//     }
//     tk.prestrip();
//     // std::cout << "Peeking here: ";
//     // print(ssbuff);
//     // std::cout << "\n";

//     result<astnode> res;

//     tk.prestrip();
//     if (tk.tokensleft()) ssbuff = tk.peek();
//     else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
//     t = gettraits(ssbuff);
    
//     if (t.cflowkeyword) res = processcflow(tk);
//     else
//     {
//         tk.prestrip();
//         ssbuff = tk.peek();
//         std::cout << "Sending to process expression: ";
//         print(ssbuff);
//         std::cout << "\n";
//         res = processexpression(tk);
//     }
    
//     if (!res.ok) return res;

//     if (
//         statementnode->tokentraits.varassign &&
//         (res.value->children.length < 1)
//     )
//     {
//         dstring emsg;

//         size_t ln = getlinenumber(tk.file, statementnode->children.end->val->tokenstr.start);

//         emsg.append("Empty assignment at line ");
//         emsg.append(tostring(ln));
//         emsg.append(":\n\n");
//         emsg.append(getlineat(tk.file, tk.filesize, ln));
//         // emsg.append("\nUndefined variable `");
//         // emsg.append(stringof(statementnode->children.end->val->tokenstr));
//         // emsg.append("`!");

//         return result<astnode>(error(244, emsg.getstring()));
//     }

//     res.value->tokentraits.rvalue = 1;
//     res.value->tokentraits.expression = 1;
//     rvalue = res.value;
//     //here boi

//     rvalue->parent = statementnode;
//     statementnode->children.postpend(rvalue);
    

//     while (tk.tokensleft())
//     {
//         tk.prestrip();
//         if (tk.tokensleft()) ssbuff = tk.gettoken();
//         else return result<astnode>(error(254, getstring_nt("Unexpected EOF!")));
//         t = gettraits(ssbuff);
//         if (t.statementterminator)
//         {
//             statementnode->tokentraits.statement = 1;
//             statementnode->tokentraits.expression = 0;
//             break;
//         }
//         else if (t.brace && t.right)
//         {
//             if (!(tk.cursor == NULL)) tk.cursor = tk.cursor->prev;
//             else tk.cursor = tk.tokens.end;
//             delete statementnode;
//             rvalue->parent = NULL;
//             return result<astnode>(rvalue);
//         }
//     }
    

//     return result<astnode>(statementnode);
// }


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



