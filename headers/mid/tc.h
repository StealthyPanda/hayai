#pragma once

#include "./../general/basics.h"
#include "./../general/result.h"
#include "./../frontend/ast.h"


string* numcompat(string *t1, string *t2)
{
    if (equal(t1, t2)) return t1;
    
    if (*t1->string == 'f')
    {
        if (*t2->string == 'f')
        {
            if (*(t1->string + 1) > *(t2->string + 1)) return t1;
            else return t2;
        }
        else return t1;
    }

    if (*t2->string == 'f') return t2;

    char *b1 = t1->string, *b2 = t2->string;
    
    if (*b1 != 'i') b1++;
    if (*b2 != 'i') b2++;

    b1++; b2++;

    if (atoi(b1) > atoi(b2)) return t1;
    
    return t2;
}

bool numerictype(string *type)
{
    stringslice ss = getslice(type);
    return (
        equal(ss, _ui8) ||
        equal(ss, _ui16) ||
        equal(ss, _ui32) ||
        equal(ss, _ui64) ||
        equal(ss, _ui128) ||
        equal(ss, _i8) ||
        equal(ss, _i16) ||
        equal(ss, _i32) ||
        equal(ss, _i64) ||
        equal(ss, _i128) ||
        equal(ss, _f32) ||
        equal(ss, _f64) ||
        equal(ss, _f80)
    );
}

struct idtypeinfo
{
    astnode *id = NULL, *dt = NULL;
    bool isconst = false;
} typedef idtypeinfo;

struct ftypeinfo
{
    string *output = NULL;
    ll<string> inputs;
    astnode *id = NULL;
} typedef ftypeinfo;

class typechecker
{
public:
    typechecker(ast *tree);
    ~typechecker();
    
    ast *tree = NULL;

    ll<idtypeinfo> idstack;
    ll<ftypeinfo> fstack; 

    result<string> gettype(astnode *root);
    result<string> check(astnode *root);
};

typechecker::~typechecker()
{
}

typechecker::typechecker(ast *tree)
{
    this->tree = tree;
}


/// @brief finds the type of the given expression token.
result<string> typechecker::gettype(astnode *root)
{
    if (root == NULL)
    return result<string>(error(253, getstring_nt("Null root!")));

    if (!(
        root->tokentraits.expression ||
        root->tokentraits.functiondeclaration ||
        root->tokentraits.numeric ||
        root->tokentraits.op ||
        root->tokentraits.stringliteral ||
        root->tokentraits.identifier
    ))
    return result<string>(
        error(251,
        getstring_nt("`gettype` can only be called on expressions/function declarations!"))
    );


    string *type = new string();
    *type = getstring_nt("void");

    result<string> res;

    if (root->tokentraits.numeric)
    {
        if (root->tokentraits.isinteger) *type = getstring_nt(_i32);
        else *type = getstring_nt(_f32);
        return result<string>(type);
    }

    if (root->tokentraits.fcall)
    {
        std::cout << "Validating fcall: ";
        astnode *id = NULL; //*body = NULL, *rt = NULL;
        ll<astnode> params;

        llnode<astnode> *cursor = root->children.root;
        while (cursor != NULL)
        {
            if (cursor->val->tokentraits.identifier)
                id = cursor->val;
            else if (cursor->val->tokentraits.expression)
                params.postpend(cursor->val);    
            cursor = cursor->next;
        }

        id->print();
        std::cout << "\n";
        
        llnode<ftypeinfo> *fcur = fstack.root;
        while (fcur != NULL)
        {
            print(fcur->val->id->tokenstr);
            print(id->tokenstr);
            if (equal(fcur->val->id->tokenstr, id->tokenstr)) break;
            fcur = fcur->next;
        }

        std::cout << "huh moment...\n";

        if (fcur == NULL)
        {
            dstring emsg;
            string holder = stringof(id->tokenstr);

            size_t ln1 = getlinenumber(this->tree->tk->file, id->tokenstr.start);

            emsg.append("Unknown fcall at line ");
            emsg.append(tostring(ln1));
            emsg.append(":\n");
            emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln1));
            emsg.append("\n\n");
            emsg.append("Cannot find `");
            emsg.append(holder);
            emsg.append("` in the current scope!");

            return result<string>(error(242, emsg.getstring()));
        }
    
        llnode<astnode> *param = params.root;
        llnode<string> *tp = fcur->val->inputs.root;
        while (param != NULL)
        {
            res = gettype(param->val);
            if (!res.ok) return res;

            stringslice ss = getslice(tp->val);

            if (!(
                equal(res.value, tp->val) ||
                (numerictype(res.value) && numerictype(tp->val)) ||
                equal(ss, _void)
            ))
            {
                dstring emsg;
                param->val->print();
                std::cout << "\n";
                size_t ln1 = getlinenumber(this->tree->tk->file, param->val->tokenstr.start),
                        ln2 = getlinenumber(this->tree->tk->file, tp->val->string);

                emsg.append("Type Error at line ");
                emsg.append(tostring(ln2));
                emsg.append(" and ");
                emsg.append(tostring(ln1));
                emsg.append(":\n");
                emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln2));
                emsg.append("\n\n");
                emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln1));
                emsg.append("\n\n");
                emsg.append("Expecting `");
                emsg.append(*tp->val);
                emsg.append("` and got `");
                emsg.append(*res.value);
                emsg.append("`!");

                return result<string>(error(249, emsg.getstring()));
            }

            param = param->next;
            tp = tp->next;
        }

        std::cout << "Output type is ";
        print(fcur->val->output);
        return result<string>(fcur->val->output);    
    }
    else if ((root->children.length == 1) && 
        (
            root->children.root->val->tokentraits.op ||
            root->children.root->val->tokentraits.expression ||
            root->tokentraits.expression
        )
    )
    {
        // std::cout << "Sending to child...\n";
        return gettype(root->children.root->val);
    }

    // std::cout << "got here\n";
    if (root->tokentraits.op)
    {
        // std::cout << "reached here\n";
        if (root->tokentraits.binary)
        {
            result<string> t1, t2;
            
            t1 = gettype(root->children.root->val);
            if (!t1.ok) return t1;

            t2 = gettype(root->children.root->next->val);
            if (!t2.ok) return t2;

            if (
                equal(t1.value, t2.value) ||
                (numerictype(t1.value) && numerictype(t2.value))
            )
            {
                if (root->tokentraits.boolop)
                {
                    *type = getstring_nt(_bool);
                    return result<string>(type);
                }
                else if (root->tokentraits.opslash)
                {
                    *type = getstring_nt(_f32);
                    return result<string>(type);
                }

                return result<string>(numcompat(t1.value, t2.value));
            }
            else
            {
                dstring emsg;

                size_t ln1 = getlinenumber(this->tree->tk->file, root->children.root->val->tokenstr.start),
                        ln2 = getlinenumber(this->tree->tk->file, root->children.root->next->val->tokenstr.start);

                emsg.append("Type Error at line ");
                emsg.append(tostring(ln1));
                emsg.append(" and ");
                emsg.append(tostring(ln2));
                emsg.append(":\n");
                emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln1));
                emsg.append("\n\n");
                emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln2));
                emsg.append("\n\n");
                emsg.append("Expression types `");
                emsg.append(*t1.value);
                emsg.append("` and `");
                emsg.append(*t2.value);
                emsg.append("` do not match!");

                return result<string>(error(249, emsg.getstring()));
            }
        }
    }

    if (root->tokentraits.stringliteral)
    {
        *type = getstring_nt(_str);
        return result<string>(type);
    }

    if (root->tokentraits.identifier)
    {
        //todo: this
        if (equal(root->tokenstr, _true) || equal(root->tokenstr, _false))
        {
            *type = getstring_nt(_bool);
            return result<string>(type);
        }

        if (root->tokentraits.datatype)
        {
            *type = stringof(root->tokenstr);
            return result<string>(type);
        }

        llnode<idtypeinfo> *cursor = idstack.root;
        while (cursor != NULL)
        {
            if (equal(cursor->val->id->tokenstr, root->tokenstr))
            {
                *type = stringof(cursor->val->dt->tokenstr);
                return result<string>(type);
            }
            cursor = cursor->next;
        }

        //if identifier not found
        {
            dstring emsg;

            size_t ln = getlinenumber(this->tree->tk->file, root->tokenstr.start);

            emsg.append("Undeclared identifier at line ");
            emsg.append(tostring(ln));
            emsg.append(":\n\n");
            emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln));
            emsg.append("\n\nCannot find `");
            emsg.append(stringof(root->tokenstr));
            emsg.append("` in current scope!");

            return result<string>(error(242, emsg.getstring()));
        }
    }

    

    return result<string>(type);
}

string okstr = getstring_nt("void");

result<string> typechecker::check(astnode *root)
{
    if (root == NULL)
    return result<string>(error(253, getstring_nt("Null root!")));

    if (!(
        root->tokentraits.translationunit ||
        root->tokentraits.body ||
        root->tokentraits.functiondeclaration
    ))
    return result<string>(error(242, getstring_nt("`check` can only be called on scopes!")));

    std::cout << "Got a ";
    root->print();
    std::cout << "\n";

    llnode<astnode> *overallcursor = root->children.root;
    llnode<idtypeinfo> *holder = NULL, *idbase = idstack.end;
    llnode<ftypeinfo> *fbase = fstack.end;
    string *stringbuff;
    llnode<astnode> *cursor = NULL;

    result<string> overallresult = result<string>(&okstr), temp;

    ftypeinfo *fbuff;
    if (root->tokentraits.functiondeclaration)
    {
        fbuff = new ftypeinfo;
        astnode *id = NULL, *body = NULL, *frt = NULL;
        ll<astnode> args;

        llnode<astnode> *cursor = root->children.root, *pointer = NULL;
        while (cursor != NULL)
        {
            if (cursor->val->tokentraits.identifier)
            {
                if (cursor->val->tokentraits.freturntype)
                    frt = cursor->val;
                else
                    id = cursor->val;
            }
            else if (cursor->val->tokentraits.body)
                body = cursor->val;
            else if (cursor->val->tokentraits.argument)
                args.postpend(cursor->val);
            
            cursor = cursor->next;
        }
        // std::cout << "FRT: ";
        // frt->print();
        // std::cout << "\n";

        //a LOT more stuff is gonna go here
        string *sbuff;
        if (args.length > 0)
        {
            idtypeinfo *buff;
            cursor = args.root;
            llnode<idtypeinfo> *checker = NULL;
            while (cursor != NULL)
            {
                buff = new idtypeinfo();
                pointer = cursor->val->children.root;
                while (pointer != NULL)
                {
                    if (pointer->val->tokentraits.datatype)
                        buff->dt = pointer->val;
                    else if (pointer->val->tokentraits.identifier)
                        buff->id = pointer->val;
                    pointer = pointer->next;
                }

                checker = idstack.root;
                while (checker != NULL)
                {
                    if (equal(checker->val->id->tokenstr, buff->id->tokenstr))
                    {
                        dstring emsg;

                        size_t ln1 = getlinenumber(this->tree->tk->file, checker->val->id->tokenstr.start),
                        ln2 = getlinenumber(this->tree->tk->file, buff->id->tokenstr.start);

                        emsg.append("Reclaration of `");
                        emsg.append(stringof(buff->id->tokenstr));
                        emsg.append("` at line ");
                        emsg.append(tostring(ln2));
                        emsg.append(":\n\n");
                        emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln2));
                        emsg.append("\n\nPreviously declared at line ");
                        emsg.append(tostring(ln1));
                        emsg.append(":\n\n");
                        emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln1));

                        return result<string>(error(242, emsg.getstring()));
                    }
                    checker = checker->next;
                }

                idstack.postpend(buff);

                sbuff = new string();
                *sbuff = stringof(buff->dt->tokenstr);
                fbuff->inputs.postpend(sbuff);

                cursor = cursor->next;
            }
        }

        fbuff->output = new string;
        fbuff->id = id;
        
        llnode <astnode> *argpoint = args.root;
        string *tbuff;
        while (argpoint != NULL)
        {
            tbuff = new string;
            *tbuff = stringof(argpoint->val->tokenstr);
            fbuff->inputs.postpend(tbuff);
            argpoint = argpoint->next;
        }

        if (frt != NULL) *fbuff->output = stringof(frt->tokenstr);
        fstack.postpend(fbuff);

        std::cout << "At the end of funcdecl, IDlen: " <<
        idstack.length << "\n";

        //processing the goddamn body of the function
        temp = check(body);
        if (!temp.ok) return temp;
        
        if (frt == NULL)
        {
            frt = new astnode();
            frt->parent = root;
            root->children.postpend(frt);

            frt->tokentraits.freturntype = 1;
            frt->tokentraits.datatype = 1;
            frt->tokentraits.identifier = 1;

            frt->tokenstr = getslice(temp.value);

            fbuff->output = temp.value;
        }
        else
        {
            string holder = stringof(frt->tokenstr);
            if (!(
                equal(&holder, temp.value) ||
                (numerictype(temp.value) && numerictype(&holder))
            ))
            {
                dstring emsg;

                // size_t ln = getlinenumber(this->tree->tk->file, buffer->id->tokenstr.start),
                //         ln2 = getlinenumber(this->tree->tk->file, holder->val->id->tokenstr.start);

                // emsg.append("Redeclaration of `");
                // emsg.append(stringof(buffer->id->tokenstr));
                // emsg.append("` at line ");
                // emsg.append(tostring(ln));
                // emsg.append(":\n\n");
                // emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln));
                // emsg.append("\nPreviously variable declared at line ");
                // emsg.append(tostring(ln2));
                // emsg.append(":\n\n");
                // emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln2));
                // emsg.append("\n");
                // string *what = new string;
                // *what = stringof(fbuff->id->tokenstr);
                emsg.append("Mismatching return type in `");
                emsg.append(stringof(fbuff->id->tokenstr));
                emsg.append("`: ");
                emsg.append(holder);
                emsg.append(" and ");
                emsg.append(*temp.value);
                emsg.append("!");
                // emsg.append("`!");

                return result<string>(error(241, emsg.getstring()));
            }
        

        }
        
        overallresult = temp;

        // overallresult = result<string>(&okstr);
    }
    else
    {
        if (root->children.length == 0)
        {
            // overallresult = result<string>(&okstr);
        }
        while (overallcursor != NULL)
        {
            std::cout << "Dealing with: ";
            overallcursor->val->print();
            std::cout << "\n";
            std::cout << "Current status: IDstacklen: " << idstack.length
            << " Fstacklen: " << fstack.length << "\n\n"; 
            root = overallcursor->val;

            idtypeinfo *buffer = NULL;
            if (root->tokentraits.returner)
            {
                string *ns = new string;
                *ns = getstring_nt(_void);
                if (root->children.length > 0)
                {
                    std::cout << "IT should be here...\n";
                    temp = gettype(root->children.root->val);
                    if (!temp.ok) return temp;

                    overallresult = temp;
                }
                // else overallresult = result<string>(ns);
                break;
            }
            else if (root->tokentraits.statement)
            {
                if (root->tokentraits.vardeclaration)
                {
                    buffer = new idtypeinfo();
                    cursor = root->children.root;
                    while (cursor != NULL)
                    {
                        if (cursor->val->tokentraits.datatype) 
                        buffer->dt = cursor->val;
                        else if (cursor->val->tokentraits.identifier)
                        {
                            buffer->id = cursor->val;
                            // std::cout << "Got the id brah\n";
                            // buffer->id->print();
                        }

                        cursor = cursor->next;
                    }
                    buffer->isconst = root->tokentraits.isconst;

                    holder = idstack.root;
                    stringslice ss = buffer->id->tokenstr;
                    while (holder != NULL)
                    {
                        if (equal(holder->val->id->tokenstr, ss)) break;
                        holder = holder->next;
                    }

                    if (holder != NULL)
                    {
                        dstring emsg;

                        size_t ln = getlinenumber(this->tree->tk->file, buffer->id->tokenstr.start),
                                ln2 = getlinenumber(this->tree->tk->file, holder->val->id->tokenstr.start);

                        emsg.append("Redeclaration of `");
                        emsg.append(stringof(buffer->id->tokenstr));
                        emsg.append("` at line ");
                        emsg.append(tostring(ln));
                        emsg.append(":\n\n");
                        emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln));
                        emsg.append("\nPreviously variable declared at line ");
                        emsg.append(tostring(ln2));
                        emsg.append(":\n\n");
                        emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln2));
                        emsg.append("\n");
                        // emsg.append("`!");

                        return result<string>(error(243, emsg.getstring()));
                    }


                    this->idstack.postpend(buffer);

                    stringbuff = new string();
                    *stringbuff = stringof(buffer->id->tokenstr);
                    // std::cout << "In here dawg\n";
                    // overallresult = result<string>(&okstr);
                }
                // std::cout << (buffer == NULL) << "\n";
                // buffer->id->print();
                if (root->tokentraits.varassign)
                {
                    std::cout << "Detected varassign...\n";
                    astnode *var;
                    cursor = root->children.root;
                    while (!
                        (cursor->val->tokentraits.identifier &&
                        !cursor->val->tokentraits.datatype)
                    ) cursor = cursor->next;
                    
                    
                    holder = this->idstack.root;
                    while (holder != NULL)
                    {
                        if (equal(holder->val->id->tokenstr, cursor->val->tokenstr)) break;
                        holder = holder->next;
                    }

                    if (holder == NULL)
                    {
                        dstring emsg;

                        size_t ln = getlinenumber(this->tree->tk->file, cursor->val->tokenstr.start);

                        emsg.append("Unrecognised identifier at line ");
                        emsg.append(tostring(ln));
                        emsg.append(":\n\n");
                        emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln));
                        emsg.append("\nUndefined variable `");
                        emsg.append(stringof(cursor->val->tokenstr));
                        emsg.append("`!");

                        return result<string>(error(246, emsg.getstring()));
                    }
                    
                    size_t lnid = getlinenumber(this->tree->tk->file, cursor->val->tokenstr.start);
                    
                    
                    if (holder->val->isconst)
                    {
                        if (!root->tokentraits.vardeclaration)
                        {
                            dstring emsg;

                            size_t ln = getlinenumber(this->tree->tk->file, cursor->val->tokenstr.start);

                            emsg.append("Const reassignment at line ");
                            emsg.append(tostring(ln));
                            emsg.append(":\n\n");
                            emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln));
                            emsg.append("\nCannot reassign `const` variable `");
                            emsg.append(stringof(cursor->val->tokenstr));
                            emsg.append("`!");

                            return result<string>(error(245, emsg.getstring()));
                        }
                    }

                    std::cout << "So far so good...\n";

                    while (!cursor->val->tokentraits.rvalue) cursor = cursor->next;

                    temp = gettype(cursor->val);
                    if (!temp.ok) return temp;


                    stringslice ssbuff = getslice(temp.value);
                    if ((buffer != NULL) && (buffer->dt == NULL))
                    {
                        std::cout << "Inferring type `";
                        astnode *dtnode = new astnode(ssbuff);
                        dtnode->tokentraits.datatype = 1;
                        dtnode->parent = root;

                        root->children.prepend(dtnode);

                        buffer->dt = dtnode;

                        buffer->dt->print();
                        std::cout << "`\n";
                        // overallresult = result<string>(&okstr);
                    }

                    // if (buffer == NULL)
                    // std::cout << "Reached here...\n";


                    // print(holder->val->dt->tokenstr);
                    // std::cout << " ";
                    // print(ssbuff);
                    // std::cout << "\n";
                    if (equal(holder->val->dt->tokenstr, ssbuff))
                    {
                        // overallresult = result<string>(&okstr);
                    }
                    else
                    {
                        dstring emsg;

                        // size_t ln1 = getlinenumber(this->tree->tk->file, temp.value->string),
                        //         ln2 = getlinenumber(this->tree->tk->file, holder->val->id->tokenstr.start);

                        emsg.append("Type Error at line ");
                        emsg.append(tostring(lnid));
                        // emsg.append(" and ");
                        // emsg.append(tostring(ln2));
                        emsg.append(":\n\n");
                        emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, lnid));
                        // emsg.append("\n\n");
                        // emsg.append(getlineat(this->tree->tk->file, this->tree->tk->filesize, ln2));
                        emsg.append("\n\n");
                        emsg.append("Expression type `");
                        emsg.append(*temp.value);
                        emsg.append("` does not match the variable type `");
                        emsg.append(stringof(holder->val->dt->tokenstr));
                        emsg.append("`!");

                        return result<string>(error(249, emsg.getstring()));
                    }
                }

                

                cursor = root->children.root;
                while (cursor != NULL)
                {
                    if (cursor->val->tokentraits.rvalue) break;
                    cursor = cursor->next;
                }

                if (cursor != NULL)
                {
                    temp = gettype(cursor->val);
                    if (!temp.ok) return temp;
                    // overallresult = result<string>(&okstr);
                }
            }
            else if (root->tokentraits.body || root->tokentraits.functiondeclaration)
            {
                temp = check(root);
                if (!temp.ok) return temp;
                overallresult = temp;
            }
            else if (root->tokentraits.controlflow)
            {
                astnode *condition = NULL, *body = NULL;
                cursor = root->children.root;
                while (cursor != NULL)
                {
                    if (cursor->val->tokentraits.expression)
                        condition = cursor->val;
                    else if (cursor->val->tokentraits.body)
                        body = cursor->val;
                    cursor = cursor->next;
                }

                temp = gettype(condition);
                if (!temp.ok) return temp;

                temp = check(body);
                if (!temp.ok) return temp;

                // overallresult = result<string>(&okstr);
            }

            std::cout << "what in the what\n";
            overallcursor = overallcursor->next;
        }
    }

    std::cout << "IDSTACK len: " << idstack.length << "\n";
    while (idstack.end != idbase) 
    {
        std::cout << "\nDELETION ";
        std::cout << idstack.length << " ";
        idstack.postpop()->id->print();
        std::cout << "\n";
    }
    std::cout << "\n";
    // while (fstack.end != fbase) delete fstack.postpop();
    if ((overallcursor != NULL) && (overallcursor->val->parent->tokentraits.functiondeclaration))
    {
        std::cout << "Leaving funcdecl...\n";
    }

    // if (!overallresult.ok)
    // {
    //     print(overallresult.value);
    // }
    return result<string>(overallresult);
}