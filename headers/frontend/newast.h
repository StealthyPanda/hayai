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


result<astnode> processstatement(tokenizer &tk);
result<astnode> processexpression(tokenizer &tk);
result<astnode> processfcall(tokenizer &tk);
result<astnode> processfuncdef(tokenizer &tk);
result<astnode> processrval(tokenizer &tk);
result<astnode> processscope(tokenizer &tk);
result<astnode> processcflow(tokenizer &tk);
result<astnode> processtype(tokenizer &tk);
result<astnode> processtuple(tokenizer &tk);
result<astnode> processscope(tokenizer &tk);

result<bool> istuple(tokenizer &tk, llnode<token> *start);
result<bool> isftype(tokenizer &tk, llnode<token> *start);
//TODO: add an isinlinefuncdef checking function

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
        noderes = processexpression(tk);
        _validate(noderes)

        items.postpend(noderes.value);

        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        if (tokres.value->props.commasep) {}
        else if (tokres.value->props.roundbracket && tokres.value->props.right)
            break;
        else
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nExpecting `,` or `)`; got `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` while processing tuple!");

            return result<astnode>(253, emsg.getstring());
        }
    }

    // std::cout << "Tuple end: ";
    // tk.nexttok->val->print();
    // std::cout << "\n";

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

    // std::cout << "At the start of statement procsr: ";
    // currtoken->print();
    // std::cout << "\n";

    astnode *var = NULL, *type = NULL, *rval = NULL;
    astnode *statementnode = new astnode;
    statementnode->tokentraits.statement = 1;


    if (currtoken->props.rkeyword)
    {
        tk.gettoken();
        noderes = processrval(tk);
        _validate(noderes)

        statementnode->tokentraits.returner = 1;
        noderes.value->parent = statementnode;
        statementnode->children.postpend(noderes.value);

        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        if (!tokres.value->props.statementterminator)
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nExpecting `;`, got `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` instead!");

            return result<astnode>(252, emsg.getstring());
        }
    
        return result<astnode>(statementnode);
    }

    if (currtoken->props.breakkeyword)
    {
        tk.gettoken();
        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        if (!tokres.value->props.statementterminator)
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nExpecting `;` after `break`, got `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` instead!");

            return result<astnode>(252, emsg.getstring());
        }
    
        statementnode->tokentraits.breakstatement = 1;
        return result<astnode>(statementnode);
    }

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

        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (!(
            tokres.value->props.statementterminator ||
            tokres.value->props.assigner
        ))
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nExpected `;` or assigner; got `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` instead!");

            return result<astnode>(253, emsg.getstring());
        }
    }

    if (var == NULL)
    {
        var = type;
        type = NULL;
        if (var != NULL) var->tokentraits.datatype = 0;
    }
    
    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)


    if (tokres.value->props.assigner)
    {
        statementnode->tokentraits.varassign = 1;
        tk.gettoken();
    }

    
    
    if (statementnode->tokentraits.varassign || !statementnode->tokentraits.vardeclaration)
    {
        // std::cout << "finding rval...\n";
        noderes = processrval(tk);
        _validate(noderes)

        rval = noderes.value;
    }


    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    // std::cout << "At this point: ";
    // tk.nexttok->val->print();
    // std::cout << "\n";

    if (!tokres.value->props.statementterminator)
    {
        if ((rval != NULL) && tokres.value->props.brace && tokres.value->props.right)
        {
            std::cout << "Triggered\n";
            return result<astnode>(rval);
        }
        else
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nExpecting `;` or an expression at end of scope; got `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("`!");

            return result<astnode>(252, emsg.getstring());
        }
    }
    else
        tk.gettoken();



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


/// @brief processes an expression (and also tuples)
result<astnode> processexpression(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres, peeker;
    result<astnode> noderes;
    result<bool> bres;

    astnode *expressionnode = new astnode, *current = expressionnode, *buffer = NULL, *holder = NULL;
    expressionnode->tokentraits.expression = 1;

    size_t nrb = 0;

    tokres = tk.gettoken();
    _validatecast(tokres, astnode)


    //simply return string literals
    if (tokres.value->props.stringliteral)
    {
        expressionnode->tok = tokres.value;
        expressionnode->tokentraits = tokres.value->props;
        return result<astnode>(expressionnode);
    }



    while (!(
        // (!tk.available()) ||
        (tokres.value->props.brace) ||
        (tokres.value->props.right && tokres.value->props.squarebracket) ||
        (tokres.value->props.commasep) ||
        (tokres.value->props.right && tokres.value->props.roundbracket && (nrb == 0)) ||
        tokres.value->props.statementterminator
    ))
    {

        if (current->tokentraits.op)
        {
            if (current->tokentraits.binary && (current->children.length >= 2))
            {
                current = current->parent;
                continue;
            }
            else if (current->tokentraits.unary && (current->children.length >= 1))
            {
                current = current->parent;
                continue;
            }
        }



        if (tokres.value->props.op)
        {
            buffer = new astnode(tokres.value);
            
            holder = current->children.postpop();
            holder->parent = buffer;
            buffer->children.postpend(holder);

            buffer->parent = current;
            current->children.postpend(buffer);

            current = buffer;
        }

        else if (tokres.value->props.roundbracket)
        {
            if (tokres.value->props.right)
            {
                if (nrb > 0) nrb--;
                else
                {
                    tk.nexttok = tk.nexttok->prev;
                    break;
                }
                
                if (current->parent != NULL) current = current->parent;
            }
            else
            {
                tk.nexttok = tk.nexttok->prev;
                bres = istuple(tk, tk.nexttok);
                _validatecast(bres, astnode)

                if (*bres.value) //this is indeed a tuple
                {
                    noderes = processtuple(tk);
                    _validate(noderes)

                    noderes.value->parent = current;
                    current->children.postpend(noderes.value);
                }
                else
                {
                    bres = isftype(tk, tk.nexttok);
                    _validatecast(bres, astnode)

                    if (*bres.value) // is indeed an ftype
                    {
                        dstring emsg;

                        emsg.append(getlineat(tk, tokres.value->linenumber));
                        emsg.append("\n\nUnexpected ftype while trying to process expression!");

                        return result<astnode>(253, emsg.getstring());
                    }
                    else
                    {
                        tk.gettoken();
                        nrb++;

                        buffer = new astnode;
                        buffer->tokentraits.expression = 1;

                        buffer->parent = current;
                        current->children.postpend(buffer);

                        current = buffer;
                    }
                }
            }
        }

        else if (tokres.value->props.identifier)
        {
            tk.prestrip();
            peeker = tk.peek();
            _validatecast(peeker, astnode)

            if (peeker.value->props.roundbracket && peeker.value->props.left)
            {
                //this is an fcall
                tk.nexttok = tk.nexttok->prev;
                noderes = processfcall(tk);
                _validate(noderes)

                noderes.value->parent = current;
                current->children.postpend(noderes.value);
            }
            else
            {
                buffer = new astnode(tokres.value);
                buffer->parent = current;
                current->children.postpend(buffer);
            }
        }

        else
        {
            buffer = new astnode(tokres.value);
            buffer->parent = current;
            current->children.postpend(buffer);
        }


        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)
    }

    if ((tk.nexttok != NULL) && !(tk.nexttok->val->props.roundbracket && tk.nexttok->val->props.right && (nrb != 0)))
        tk.nexttok = tk.nexttok->prev;
    // if ((tk.nexttok != NULL) && tk.nexttok->val->props.roundbracket) tk.nexttok = tk.nexttok->next;


    // if (tk.nexttok != NULL)
    // {
    //     std::cout << "At the end of expression processing: ";
    //     tk.nexttok->val->print();
    //     std::cout << "\n";
    // }

    return result<astnode>(expressionnode);
}


/// @brief processes an fcall. Tokeneizer sequence must be at the start of fcall; i.e at the identifier
result<astnode> processfcall(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tk.prestrip();
    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.identifier)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpecting identifier to process fcall, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("`!");

        return result<astnode>(253, emsg.getstring());
    }

    astnode *id = new astnode(tokres.value), *fcallnode = new astnode;
    fcallnode->tokentraits.fcall = 1;

    tk.prestrip();
    // std::cout << "Before istuple: ";
    // tk.nexttok->val->print();
    // std::cout << "\n";
    bres = istuple(tk, tk.nexttok);
    _validatecast(bres, astnode)

    tk.prestrip();
    // std::cout << "istuple for ";
    // id->print();
    // std::cout << ": " << (int)(*bres.value) << "\n";
    if (*bres.value)
        noderes = processtuple(tk);
    else
        noderes = processexpression(tk);
    _validate(noderes)

    // std::cout << "\tInside fcall after consumption for ";
    // id->print();
    // std::cout << ": ";
    // tk.nexttok->val->print();
    // std::cout << "\n";

    id->parent = fcallnode;
    noderes.value->parent = fcallnode;
    noderes.value->tokentraits.tuple = 0;
    noderes.value->tokentraits.arguments = 1;
    noderes.value->tokentraits.expression = 0;

    fcallnode->children.postpend(id)->postpend(noderes.value);
    fcallnode->tokentraits.expression = 1;

    return result<astnode>(fcallnode);
}


/// @brief processes an rvalue 
result<astnode> processrval(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.cflowkeyword)
        noderes = processcflow(tk);
    else if (tokres.value->props.functiondeclarator)
        noderes = processfuncdef(tk);
    else
        noderes = processexpression(tk);
    _validate(noderes)

    // std::cout << "nchildren: " << noderes.value->children.length << "\n";

    noderes.value->tokentraits.rvalue = 1;
    return result<astnode>(noderes.value);
}

/// @brief processes control flow blocks 
result<astnode> processcflow(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.cflowkeyword)
    {
        dstring emsg;

        emsg.append("\n");
        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected control flow keyword, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("`!");

        return result<astnode>(253, emsg.getstring());
    }


    astnode *blocknode = new astnode, *buffer = NULL;
    blocknode->tokentraits.controlflow = 1;

    if (tokres.value->props.ifkeyword)
    {
        blocknode->tokentraits.ifblock = 1;
        noderes = processexpression(tk);
        _validate(noderes)

        noderes.value->parent = blocknode;
        blocknode->children.postpend(noderes.value);

        noderes = processscope(tk);
        _validate(noderes)

        noderes.value->parent = blocknode;
        blocknode->children.postpend(noderes.value);

        tk.prestrip();
        if (!tk.available())
            return result<astnode>(blocknode);


        tokres = tk.peek();
        _validatecast(tokres, astnode)

        // std::cout << "pekkers: ";
        // tokres.value->print();
        // std::cout << "\n";

        while (tokres.value->props.elifkeyword)
        {
            tk.gettoken();

            buffer = new astnode;
            buffer->tokentraits.controlflow = 1;
            buffer->tokentraits.elifblock = 1;

            noderes = processexpression(tk);
            _validate(noderes)

            noderes.value->parent = buffer;
            buffer->children.postpend(noderes.value);

            noderes = processscope(tk);
            _validate(noderes)

            noderes.value->parent = buffer;
            buffer->children.postpend(noderes.value);

            buffer->parent = blocknode;
            blocknode->children.postpend(buffer);

            tk.prestrip();
            if (!tk.available())
                return result<astnode>(blocknode);

            tokres = tk.peek();
            _validatecast(tokres, astnode)
        }

        tk.prestrip();
        if (!tk.available())
            return result<astnode>(blocknode);
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.elsekeyword)
        {
            tk.gettoken();

            tk.prestrip();
            noderes = processscope(tk);
            _validate(noderes)

            buffer = new astnode;
            buffer->tokentraits.controlflow = 1;
            buffer->tokentraits.elseblock = 1;

            noderes.value->parent = buffer;
            buffer->children.postpend(noderes.value);

            buffer->parent = blocknode;
            blocknode->children.postpend(buffer);

            return result<astnode>(blocknode);
        }

    }


    else if (tokres.value->props.whilekeyword)
    {
        blocknode->tokentraits.whileblock = 1;
        noderes = processexpression(tk);
        _validate(noderes)

        noderes.value->parent = blocknode;
        blocknode->children.postpend(noderes.value);

        noderes = processscope(tk);
        _validate(noderes)

        noderes.value->parent = blocknode;
        blocknode->children.postpend(noderes.value);
    }


    return result<astnode>(blocknode);
}

/// @brief processes control flow blocks 
result<astnode> processscope(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.brace && tokres.value->props.left))
    {
        dstring emsg;

        emsg.append("\n");
        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected `{` while processing scope, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("`!");

        return result<astnode>(253, emsg.getstring());
    }


    astnode *scopeblock = new astnode;
    scopeblock->tokentraits.body = 1;

    while (true)
    {
        tk.prestrip();
        tokres = tk.peek();
        // std::cout << "Right now at: ";
        // tokres.value->print();
        // std::cout << "\n";
        _validatecast(tokres, astnode)
        
        if ((tokres.value->props.brace && tokres.value->props.right))
            break;

        if (tokres.value->props.cflowkeyword)
        {
            noderes = processcflow(tk);
            _validate(noderes)
            noderes.value->parent = scopeblock;
            scopeblock->children.postpend(noderes.value);
        }

        else if ((tokres.value->props.brace && tokres.value->props.left))
        {
            noderes = processscope(tk);
            _validate(noderes)
            noderes.value->parent = scopeblock;
            scopeblock->children.postpend(noderes.value);
        }
        
        else
        {
            noderes = processstatement(tk);
            _validate(noderes)
            noderes.value->parent = scopeblock;
            scopeblock->children.postpend(noderes.value);
        }
    }
    tk.gettoken();

    return result<astnode>(scopeblock);
}


/// @brief processes all types of function definitions, except inline
result<astnode> processfuncdef(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.functiondeclarator)
    {
        dstring emsg;

        emsg.append("\n");
        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected `fun` while processing funcdef, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("`!");

        return result<astnode>(253, emsg.getstring());
    }
    else
        tk.gettoken();

    astnode *funcdef = new astnode, *buffer = NULL, *paramsnode = new astnode;
    funcdef->tokentraits.functiondeclaration = 1;


    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.identifier)
    {
        tk.gettoken();
        buffer = new astnode(tokres.value);
        buffer->parent = funcdef;
        funcdef->children.postpend(buffer);
    }

    ll<astnode> params;

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.left && tokres.value->props.roundbracket))
    {
        dstring emsg;

        emsg.append("\n");
        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected list of params while processing funcdef, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("`!");

        return result<astnode>(253, emsg.getstring());
    }
    else
        tk.gettoken();


    // std::cout << "Gothere\n";

    while (!(tokres.value->props.right && tokres.value->props.roundbracket))
    {
        buffer = new astnode;
        buffer->tokentraits.param = 1;
        
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.clonekeyword)
        {
            tk.gettoken();
            buffer->tokentraits.clone = 1;
        }
        else if (tokres.value->props.roundbracket && tokres.value->props.right)
        {
            tk.gettoken();
            break;
        }

        noderes = processtype(tk);
        _validate(noderes)

        noderes.value->parent = buffer;
        buffer->children.postpend(noderes.value);

        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.clonekeyword)
        {
            tk.gettoken();
            buffer->tokentraits.clone = 1;
        }

        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)
        
        if (tokres.value->props.identifier)
        {
            tk.gettoken();
            astnode *holder = new astnode(tokres.value);
            holder->parent = buffer;
            buffer->children.postpend(holder);
        }
        else
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nUnnamed parameter in func params!");
            // emsg.append("` in func params!");

            return result<astnode>(253, emsg.getstring());
        }


        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.clonekeyword)
        {
            tk.gettoken();
            buffer->tokentraits.clone = 1;
        }

        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.commasep)
            tk.gettoken();
        else if (tokres.value->props.roundbracket && tokres.value->props.right)
        {
            tk.gettoken();
            buffer->parent = paramsnode;
            paramsnode->children.postpend(buffer);
            break;
        }
        else
        {
            dstring emsg;

            emsg.append("\n");
            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nUnexpected token `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` in func params!");

            return result<astnode>(253, emsg.getstring());
        }


        buffer->parent = paramsnode;
        paramsnode->children.postpend(buffer);
    }

    
    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.colon)
    {
        tk.gettoken();
        noderes = processtype(tk);
        _validate(noderes)

        noderes.value->parent = funcdef;
        noderes.value->tokentraits.freturntype = 1;
        funcdef->children.postpend(noderes.value);
    }
    
    
    paramsnode->tokentraits.fparams = 1;
    paramsnode->parent = funcdef;
    funcdef->children.postpend(paramsnode);


    noderes = processscope(tk);
    _validate(noderes)

    noderes.value->parent = funcdef;
    funcdef->children.postpend(noderes.value);

    return result<astnode>(funcdef);
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



