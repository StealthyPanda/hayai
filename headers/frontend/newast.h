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
result<astnode> processindex(tokenizer &tk);
// result<astnode> processscope(tokenizer &tk);

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


result<bool> isftype(tokenizer &tk, llnode<token> *start);


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

    result<bool> bres;
    // llnode<token> *os = start;

    bres = isftype(tk, start);
    _validate(bres)

    if (*bres.value)
        return result<bool>((bool*) &falsebool);


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
            if (nrb == 0) return result<bool>((bool*) &falsebool);
            else nrb--;
        }
        
        else if (start->val->props.commasep && (nrb == 0))
        {
            // bres = isftype(tk, os);
            // _validate(bres)
            // if (*bres.value)
            //     return result<bool>((bool*) &falsebool);
            // return result<bool>((bool*) &truebool);
            return result<bool>((bool*) &truebool);
        }
        
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
        
        else if (start->val->props.colon && (nrb == 0))
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

    llnode<astnode> *cursor = items.root;
    while (cursor != NULL)
    {
        cursor->val->parent = tuplenode;
        cursor = cursor->next;
    }

    tuplenode->children = items;
    tuplenode->tokentraits.tuple = 1;
    tuplenode->tok = NULL;
    return result<astnode>(tuplenode);
}


/// @brief processes an ftype
result<astnode> processftype(tokenizer &tk)
{
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.roundbracket && tokres.value->props.left))
        return result<astnode>(253, "Expecting `(` while processing ftype!");
    
    tk.gettoken();

    astnode *inp = new astnode, *out = new astnode, *ftypenode = new astnode;
    // ll<astnode> inputs, outputs;

    while (true)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.colon)
        {
            tk.gettoken();
            break;
        }
        else if (tokres.value->props.commasep)
        {
            tk.gettoken();
        }

        noderes = processtype(tk);
        _validate(noderes)

        noderes.value->parent = inp;
        inp->children.postpend(noderes.value);
    }

    while (true)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.roundbracket && tokres.value->props.right)
        {
            tk.gettoken();
            break;
        }
        else if (tokres.value->props.commasep)
        {
            tk.gettoken();
        }

        noderes = processtype(tk);
        _validate(noderes)

        noderes.value->parent = out;
        out->children.postpend(noderes.value);
    }



    inp->parent = ftypenode;
    out->parent = ftypenode;

    inp->tokentraits.fins = 1;
    out->tokentraits.fouts = 1;

    ftypenode->children.postpend(inp)->postpend(out);
    ftypenode->tokentraits.ftype = 1;

    return result<astnode>(ftypenode);
}


/// @brief processes any type, along with associated keywords like ptr, clone etc.
result<astnode> processtype(tokenizer &tk)
{

    result<token> tokres;
    result<bool> bres;
    result<astnode> noderes;

    astnode *dt = new astnode, *holder = NULL;
    

    bool dtflag = false;
    while (!dtflag)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        // if (
        //     (tokres.value->props.identifier && dtflag) ||
        //     tokres.value->props.colon ||
        //     tokres.value->props.commasep ||
        //     tokres.value->props.assigner ||
        //     (tokres.value->props.roundbracket && tokres.value->props.right)
        // )
        // break;

        if (tokres.value->props.identifier)
        {
            // if (dtflag)
            //     break;
            
            tk.gettoken();
            dtflag = true;
            holder = new astnode(tokres.value);
            holder->parent = dt;
            dt->children.postpend(holder);
        }
        else if (tokres.value->props.clonekeyword)
        {
            tk.gettoken();
            dt->tokentraits.clone = 1;
        }
        else if (tokres.value->props.ptrdeclarator)
        {
            tk.gettoken();
            dt->tokentraits.pointer = 1;
        }
        else if (tokres.value->props.roundbracket && tokres.value->props.left)
        {
            bres = istuple(tk, tk.nexttok);
            _validatecast(bres, astnode)

            if (*bres.value)
            {
                noderes = processtuple(tk);
                _validate(noderes)

                noderes.value->parent = dt;
                dt->children.postpend(noderes.value);
                dtflag = true;
            }
            else
            {
                bres = isftype(tk, tk.nexttok);
                _validatecast(bres, astnode)

                if (*bres.value)
                {
                    noderes = processftype(tk);
                    _validate(noderes)

                    noderes.value->parent = dt;
                    dt->children.postpend(noderes.value);
                    dtflag = true;
                }
                else
                {
                    dstring emsg;

                    emsg.append(getlineat(tk, tokres.value->linenumber));
                    emsg.append("\n\nInvalid type in roundbrackets `");
                    emsg.append(tokres.value->tokenstr);
                    emsg.append("` while parsing type!");

                    return result<astnode>(253, emsg.getstring());
                }
            }
        }
        else
        {
            dstring emsg;

            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nUnexpected token `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` while parsing type!");

            return result<astnode>(253, emsg.getstring());
        }
    }


    while (true)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (
            (tokres.value->props.identifier) ||
            tokres.value->props.colon ||
            tokres.value->props.commasep ||
            tokres.value->props.assigner ||
            tokres.value->props.statementterminator ||
            tokres.value->props.brace ||
            (tokres.value->props.roundbracket)
        )
        break;

        if (tokres.value->props.clonekeyword)
        {
            tk.gettoken();
            dt->tokentraits.clone = 1;
        }
        else if (tokres.value->props.ptrdeclarator)
        {
            tk.gettoken();
            dt->tokentraits.pointer = 1;
        }
        else
        {
            dstring emsg;

            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nUnexpected token `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` while parsing type!");

            return result<astnode>(253, emsg.getstring());
        }
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
        if (currtoken->props.constdeclarator)
            statementnode->tokentraits.isconst = 1;

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
            // var = type;
            // var->tokentraits.datatype = 0;
            // var->tokentraits.identifier = 1;
            // type = NULL;

            if (type->children.root->val->tokentraits.identifier)
            {
                var = type->children.root->val;
            }
            else if (type->children.root->val->tokentraits.tuple)
            {
                var = type->children.root->val;
            }
            else if (type->children.root->val->tokentraits.ftype)
            {
                dstring emsg;

                emsg.append(getlineat(tk, tokres.value->linenumber));
                emsg.append("\n\nMissing identifier!");
                // emsg.append(getlineat(tk, tokres.value->linenumber));

                return result<astnode>(252, emsg.getstring());
            }

            var->tokentraits.datatype = 0;
            type = NULL;
        }

        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        
        
        
        
        // tk.prestrip();
        // llnode<token> *cursor = tk.nexttok;
        // size_t dist = 0;
        // while (true)
        // {
        //     if (cursor == NULL)
        //     {
        //         dstring emsg;

        //         emsg.append("\nUnexpected EOF!");

        //         return result<astnode>(250, emsg.getstring());
        //     }
        //     if (cursor->val->props.assigner) break;
        //     dist++;
        //     cursor = cursor->next;
        // }

        // tk.prestrip();
        // tokres = tk.peek();
        // _validatecast(tokres, astnode)

        // if (tokres.value->props.identifier)
        // {
        //     if ()
        // }
        


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

    
    llnode<token> *cursor = tk.nexttok;
    // std::cout << "Starting from: ";
    // cursor->val->print();
    // std::cout << '\n';
    while (cursor != NULL)
    {
        if (cursor->val->props.assigner)
            statementnode->tokentraits.varassign = 1;
        
        if (
            cursor->val->props.statementterminator ||
            cursor->val->props.assigner ||
            cursor->val->props.vardeclarator ||
            cursor->val->props.brace
            // cursor->val->props.roundbracket 
        )
            break;
        
        cursor = cursor->next;
    }
    


    if (statementnode->tokentraits.varassign)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (!tokres.value->props.assigner)
        {
            // std::cout << "gothere\n";
            noderes = processrval(tk);
            _validate(noderes)

            var = noderes.value;
            var->tokentraits.expression = 0;
            var->tokentraits.rvalue = 0;
            
            if ((var->children.length == 1) && !var->tokentraits.tuple)
                var = var->children.root->val;
        }

        tk.prestrip();
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        if (!tokres.value->props.assigner)
            return result<astnode>(250, "Something went wrong in processing statement...");
    }

    
    
    if (statementnode->tokentraits.varassign || !statementnode->tokentraits.vardeclaration)
    {
        // std::cout << "finding rval...\n";
        // std::cout << "here: ";
        // tk.nexttok->val->print();
        // std::cout << "\n";
        noderes = processrval(tk);
        _validate(noderes)

        rval = noderes.value;


    }


    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    // std::cout << "reached here\n";

    // std::cout << "At this point: ";
    // tk.nexttok->val->print();
    // std::cout << "\n";

    if (!tokres.value->props.statementterminator)
    {
        traits t;
        t.statement = 1;
        if (
            (rval != NULL) &&
            (tokres.value->props.brace && tokres.value->props.right) && 
            (t == statementnode->tokentraits)
        )
        {
            // std::cout << "Triggered\n";
            statementnode->tokentraits.returner = 1;
            rval->parent = statementnode;
            statementnode->children.postpend(rval);
            return result<astnode>(statementnode);
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
        (tokres.value->props.assigner) ||
        (tokres.value->props.right && tokres.value->props.squarebracket) ||
        (tokres.value->props.commasep) ||
        (tokres.value->props.right && tokres.value->props.roundbracket && (nrb == 0)) ||
        tokres.value->props.statementterminator ||
        tokres.value->props.vardeclarator
        // tokres.value->props.cflowkeyword ||
         
    ))
    {
        // if (tokres.value->props.vardeclarator)
        // {
        //     std::cout << "Got a keyword\n";
        //     tk.nexttok = tk.nexttok->prev;
        //     break;
        // }

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
            
            if (buffer->tokentraits.binary)
            {
                holder = current->children.postpop();
                if (holder != NULL) holder->parent = buffer;
                buffer->children.postpend(holder);
            }

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

                    // noderes.value->parent = current;
                    // current->children.postpend(noderes.value);

                    if (current->children.end == NULL || current->tokentraits.op)
                    {
                        noderes.value->parent = current;
                        current->children.postpend(noderes.value);
                    }
                    else
                    {
                        buffer = new astnode;
                        buffer->tokentraits.fcall = 1;
                        buffer->tokentraits.expression = 1;

                        holder = current->children.postpop();
                        holder->parent = buffer;
                        holder->tokentraits.function = 1;
                        buffer->children.postpend(holder);


                        noderes.value->parent = buffer;
                        noderes.value->tokentraits.arguments = 1;
                        noderes.value->tokentraits.tuple = 0;
                        buffer->children.postpend(noderes.value);

                        // buffer->children.postpend(catcher);

                        buffer->parent = current;
                        current->children.postpend(buffer);
                    }

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
                    // else
                    // {
                    //     tk.gettoken();
                    //     nrb++;

                    //     buffer = new astnode;
                    //     buffer->tokentraits.expression = 1;

                    //     buffer->parent = current;
                    //     current->children.postpend(buffer);

                    //     current = buffer;
                    // }
                    else
                    {
                        tk.gettoken();
                        nrb++;

                        noderes = processexpression(tk);
                        _validate(noderes)

                        tokres = tk.gettoken();
                        if (tokres.value->props.roundbracket && tokres.value->props.right)
                            nrb--;
                        else
                        {
                            dstring emsg;

                            emsg.append(getlineat(tk, tokres.value->linenumber));
                            emsg.append("\n\nUnpaired roundbrackets, expected `)`; got `");
                            emsg.append(tokres.value->tokenstr);
                            emsg.append("`!");

                            return result<astnode>(253, emsg.getstring());
                        }

                        if (current->children.end == NULL || current->tokentraits.op)
                        {
                            noderes.value->parent = current;
                            current->children.postpend(noderes.value);
                        }
                        else
                        {
                            buffer = new astnode;
                            buffer->tokentraits.fcall = 1;
                            buffer->tokentraits.expression = 1;

                            holder = current->children.postpop();
                            holder->parent = buffer;
                            holder->tokentraits.function = 1;
                            buffer->children.postpend(holder);


                            astnode *catcher = new astnode;
                            catcher->tokentraits.arguments = 1;
                            catcher->parent = buffer;
                            buffer->children.postpend(catcher);

                            noderes.value->parent = catcher;
                            catcher->children.postpend(noderes.value);

                            // buffer->children.postpend(catcher);

                            buffer->parent = current;
                            current->children.postpend(buffer);
                        }
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

        else if (tokres.value->props.functiondeclarator)
        {
            tk.nexttok = tk.nexttok->prev;

            noderes = processfuncdef(tk);
            _validate(noderes)

            noderes.value->parent = current;
            current->children.postpend(noderes.value);
        }

        else if (tokres.value->props.cflowkeyword)
        {
            tk.nexttok = tk.nexttok->prev;

            noderes = processcflow(tk);
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
    else
    {
        // std::cout << "\tProcessing fcall for: ";
        // tokres.value->print();
        // std::cout << "\n";
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
    {
        tk.gettoken();
        // std::cout << "\tSending to process expression with: ";
        // tk.nexttok->val->print();
        // std::cout << "\n";
        noderes = processexpression(tk);
    }
    _validate(noderes)

    // std::cout << "\tInside fcall after consumption for ";
    // id->print();
    // std::cout << ": ";
    // tk.nexttok->val->print();
    // std::cout << "\n";

    id->parent = fcallnode;
    id->tokentraits.function = 1;
    noderes.value->parent = fcallnode;
    noderes.value->tokentraits.tuple = 0;
    noderes.value->tokentraits.arguments = 1;
    noderes.value->tokentraits.expression = 0;

    fcallnode->children.postpend(id)->postpend(noderes.value);
    fcallnode->tokentraits.expression = 1;

    if (!(*bres.value)) tk.gettoken();
    // std::cout << "\tAt the end of fcall: ";
    // tk.nexttok->val->print();
    // std::cout << "\n";


    return result<astnode>(fcallnode);
}

/// @brief processes an index block (enclosed with []) 
result<astnode> processindex(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.squarebracket && tokres.value->props.left))
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected '[', got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` instead!");

        return result<astnode>(253, emsg.getstring());
    }

    astnode *index = new astnode;
    index->tokentraits.index = 1;

    while (true)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.squarebracket && tokres.value->props.right)
        {
            tk.gettoken();
            break;
        }
        else if (tokres.value->props.commasep)
            tk.gettoken();
        else
        {
            noderes = processexpression(tk);
            _validate(noderes)

            noderes.value->parent = index;
            index->children.postpend(noderes.value);
        }
    }


    return result<astnode>(index);
}


///@brief processes a pointer
result<astnode> processpointer(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres, peeker;
    result<astnode> noderes;
    result<bool> bres;

    astnode *pointerdeclaration = new astnode, *defval = NULL, *id = NULL, *dt = NULL, *rval = NULL;
    pointerdeclaration->tokentraits.ptrdeclaration = 1;
    pointerdeclaration->tokentraits.statement = 1;


    tk.prestrip();
    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.ptrdeclarator)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpecting `ptr` keyword, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` instead!");

        return result<astnode>(253, emsg.getstring());
    }


    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.squarebracket && tokres.value->props.left)
    {
        noderes = processindex(tk);
        _validate(noderes)

        noderes.value->tokentraits.ptrdim = 1;
        pointerdeclaration->children.postpend(noderes.value);
    }

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.identifier)
    {
        tk.gettoken();
        dt = new astnode(tokres.value);
        dt->tokentraits.datatype = 1;
    }
    else
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpecting a datatype or identifier, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` instead!");

        return result<astnode>(253, emsg.getstring());
    }

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.squarebracket && tokres.value->props.left)
    {
        noderes = processindex(tk);
        _validate(noderes)
        noderes.value->tokentraits.ptrsize = 1;

        noderes.value->parent = pointerdeclaration;
        pointerdeclaration->children.postpend(noderes.value);
    }

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (tokres.value->props.identifier)
    {
        tk.gettoken();
        id = new astnode(tokres.value);
        // id->tokentraits.datatype = 1;
    }
    else
    {
        // dstring emsg;

        // emsg.append(getlineat(tk, tokres.value->linenumber));
        // emsg.append("\n\nExpecting an identifier, got `");
        // emsg.append(tokres.value->tokenstr);
        // emsg.append("` instead!");

        // return result<astnode>(253, emsg.getstring());
        id = dt;
        id->tokentraits.datatype = 0;
        dt = NULL;
    }

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)


    if (tokres.value->props.roundbracket && tokres.value->props.left)
    {
        noderes = processexpression(tk);
        _validate(noderes)
        noderes.value->tokentraits.fillvalue = 1;

        noderes.value->parent = pointerdeclaration;
        pointerdeclaration->children.postpend(noderes.value);
    }

    else if (tokres.value->props.assigner)
    {
        tk.gettoken();
        tk.prestrip();

        noderes = processrval(tk);
        _validate(noderes)
        noderes.value->tokentraits.rvalue = 1;

        noderes.value->parent = pointerdeclaration;
        pointerdeclaration->children.postpend(noderes.value);
    }



    tk.prestrip();
    tokres = tk.gettoken();
    _validatecast(tokres, astnode)
    
    if (!tokres.value->props.statementterminator)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpecting `;`, got `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` instead!");

        return result<astnode>(253, emsg.getstring());
    }

    id->parent = pointerdeclaration;
    pointerdeclaration->children.postpend(id);

    if (dt != NULL)
    {
        dt->parent = pointerdeclaration;
        pointerdeclaration->children.postpend(dt);
    }

    if (defval != NULL)
    {
        defval->parent = pointerdeclaration;
        pointerdeclaration->children.postpend(defval);
    }


    // if (rval != NULL)
    // {
    //     rval->parent = pointerdeclaration;
    //     pointerdeclaration->children.postpend(rval);
    // }

    return result<astnode>(pointerdeclaration);
}



/// @brief processes an rvalue 
result<astnode> processrval(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    
    noderes = processexpression(tk);
    _validate(noderes)

    noderes.value->tokentraits.rvalue = 1;

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    // if (!tokres.value->props.statementterminator)
    // {
    //     dstring emsg;

    //     emsg.append(getlineat(tk, tokres.value->linenumber));
    //     emsg.append("\n\nExpecting `;`, got `");
    //     emsg.append(tokres.value->tokenstr);
    //     emsg.append("` instead!");

    //     return result<astnode>(252, emsg.getstring());
    // }

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

        else if (tokres.value->props.ptrdeclarator)
        {
            noderes = processpointer(tk);
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


result<astnode> processparam(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    astnode *param = new astnode, *id = NULL;
    param->tokentraits.param = 1;

    
    
    noderes = processtype(tk);
    _validate(noderes)

    noderes.value->parent = param;
    param->children.postpend(noderes.value);

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.identifier)
    {

        if (tokres.value->props.left && tokres.value->props.roundbracket)
        {
            bres = istuple(tk, tk.nexttok);
            _validatecast(bres, astnode)

            if (*bres.value)
            {
                noderes = processtuple(tk);
                _validate(noderes)

                id = noderes.value;
            }
            else
            {
                dstring emsg;

                emsg.append(getlineat(tk, tokres.value->linenumber));
                emsg.append("\n\nIdentifier expected, got `");
                emsg.append(tokres.value->tokenstr);
                emsg.append("` instead!");

                return result<astnode>(253, emsg.getstring());
            }
        }
        else
        {
            dstring emsg;

            emsg.append(getlineat(tk, tokres.value->linenumber));
            emsg.append("\n\nIdentifier expected, got `");
            emsg.append(tokres.value->tokenstr);
            emsg.append("` instead!");

            return result<astnode>(253, emsg.getstring());
        }

    }

    else
    {
        tokres = tk.gettoken();
        _validatecast(tokres, astnode)

        id = new astnode(tokres.value);
    }

    if (id == NULL)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nNo identifier provided!");

        return result<astnode>(252, emsg.getstring());
    }
    


    id->parent = param;
    // dt->parent = param;
    // dt->tokentraits.datatype = 1;

    param->children.postpend(id);

    return result<astnode>(param);
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



    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    while (!(tokres.value->props.roundbracket && tokres.value->props.right))
    {
        noderes = processparam(tk);
        _validate(noderes)

        noderes.value->parent = paramsnode;
        paramsnode->children.postpend(noderes.value);

        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.commasep)
        {
            tk.gettoken();
            tk.prestrip();
            tokres = tk.peek();
            _validatecast(tokres, astnode)
        }

        else if ((tokres.value->props.roundbracket && tokres.value->props.right))
        {
            break;
        }
    }
    tk.gettoken();

    
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


/// @brief processes struct definitions
result<astnode> processstruct(tokenizer &tk)
{
    tk.prestrip();

    if (!tk.available())
        return result<astnode>(254, "Unexpected EOF while processing expression!");
    
    result<token> tokres;
    result<astnode> noderes;
    result<bool> bres;

    astnode *structnode = new astnode, *id = NULL, *memberbuff = NULL, *holder = NULL;
    structnode->tokentraits.structdef = 1;

    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.structkeyword)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected `struct` keyword, found `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` while processing struct!");

        return result<astnode>(253, emsg.getstring());
    }

    tk.prestrip();
    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.identifier)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected identifier for struct definition, found `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` while processing struct!");

        return result<astnode>(253, emsg.getstring());
    }

    id = new astnode(tokres.value);

    tk.prestrip();
    tokres = tk.peek();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.brace && tokres.value->props.left))
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected `{` for struct definition, found `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` while processing struct!");

        return result<astnode>(253, emsg.getstring());
    }
    else tk.gettoken();

    while (true)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.brace && tokres.value->props.right)
        {
            tk.gettoken();
            break;
        }

        noderes = processtype(tk);
        _validate(noderes)

        while (true)
        {
            tk.prestrip();
            tokres = tk.gettoken();
            _validatecast(tokres, astnode)

            if (tokres.value->props.statementterminator)
                break;
            else if (tokres.value->props.identifier)
            {
                memberbuff = new astnode;
                memberbuff->tokentraits.member = 1;

                holder = new astnode(*noderes.value);
                holder->parent = memberbuff;
                memberbuff->children.postpend(holder);

                holder = new astnode(tokres.value);
                holder->parent = memberbuff;
                memberbuff->children.postpend(holder);

                memberbuff->parent = structnode;
                structnode->children.postpend(memberbuff);
            }
            else if (tokres.value->props.commasep) {}
            else
            {
                dstring emsg;

                emsg.append(getlineat(tk, tokres.value->linenumber));
                emsg.append("\n\nUnexpected token `");
                emsg.append(tokres.value->tokenstr);
                emsg.append("` while processing struct!");

                return result<astnode>(253, emsg.getstring());
            }
        }

    }

    id->parent = structnode;
    structnode->children.postpend(id);

    return result<astnode>(structnode);
}

/// @brief processes trait definitions
result<astnode> processtrait(tokenizer &tk)
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

    if (!tokres.value->props.traitkeyword)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected `trait` keyword, found `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` while processing trait!");

        return result<astnode>(253, emsg.getstring());
    }

    astnode *traitnode = new astnode, *id = NULL;
    traitnode->tokentraits.traitdef = 1;

    tk.prestrip();
    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!tokres.value->props.identifier)
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected identifier for trait definition, found `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` while processing trait!");

        return result<astnode>(253, emsg.getstring());
    }

    id = new astnode(tokres.value);
    id->parent = traitnode;

    tk.prestrip();
    tokres = tk.gettoken();
    _validatecast(tokres, astnode)

    if (!(tokres.value->props.left && tokres.value->props.brace))
    {
        dstring emsg;

        emsg.append(getlineat(tk, tokres.value->linenumber));
        emsg.append("\n\nExpected `{`, found `");
        emsg.append(tokres.value->tokenstr);
        emsg.append("` while processing trait!");

        return result<astnode>(253, emsg.getstring());
    }
    // else tk.gettoken();

    while (true)
    {
        tk.prestrip();
        tokres = tk.peek();
        _validatecast(tokres, astnode)

        if (tokres.value->props.brace && tokres.value->props.right)
        {
            tk.gettoken();
            break;
        }

        noderes = processfuncdef(tk);
        _validate(noderes)

        noderes.value->parent = traitnode;
        traitnode->children.postpend(noderes.value);
    }
    


    id->parent = traitnode;
    traitnode->children.postpend(id);

    // noderes.value->parent = traitnode;
    // traitnode->children.postpend(noderes.value);

    return result<astnode>(traitnode);
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



