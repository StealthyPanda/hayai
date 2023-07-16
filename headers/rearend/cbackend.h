#pragma once

#include "./../general/basics.h"
#include "./../general/result.h"
#include "./../general/strings.h"
#include "./../general/stringslice.h"
#include "./../frontend/ast.h"
#include <fstream>


result<dstring> generatefunction(astnode*); 
result<dstring> generatescopebody(astnode*);
result<dstring> generatestatement(astnode*);
result<dstring> generateexpression(astnode*);
result<dstring> generatecflow(astnode*);


class cbackend
{
public:
    cbackend(ast *tree);
    ~cbackend();

    ast *tree;

    result<void> generateoutput(char *output);
};

cbackend::cbackend(ast *tree)
{
    this->tree = tree;
}

cbackend::~cbackend()
{
}

result<void> cbackend::generateoutput(char *output)
{
    std::ofstream out(output);
    result<void> ok;
    ok.ok = 1;

    string holder;

    result<dstring> res;

    out << _stl;

    llnode<astnode> *pointer = tree->root->children.root;
    while (pointer != NULL)
    {
        if (pointer->val->tokentraits.statement) res = generatestatement(pointer->val);
        else if (pointer->val->tokentraits.functiondeclaration) res = generatefunction(pointer->val);

        if (!res.ok) return result<void>(res.err);

        holder = res.value->getstring();
        out.write(holder.string, holder.length);

        pointer = pointer->next;
    }

    return ok;
}



result<dstring> generatestatement(astnode *root)
{
    if (root == NULL)
    return result<dstring>(error(239, getstring_nt("Null root!")));

    result<dstring> res;

    dstring *output = new dstring;

    llnode<astnode> *cursor = root->children.root;

    if (root->tokentraits.returner)
    {
        output->append(_return);

        if (root->children.length > 0)
        {
            res = generateexpression(root->children.root->val);
            if (!res.ok) return res;

            output->append(_space);
            output->append(res.value->getstring());
        }

        output->append(_semicolon);
        return result<dstring>(output);
    }

    if (root->tokentraits.vardeclaration)
    {
        if (root->tokentraits.isconst)
        {
            output->append(_const);
            output->append(_space);
        }

        astnode *id = NULL, *dt = NULL;
        while (cursor != NULL)
        {
            if (cursor->val->tokentraits.datatype)
                dt = cursor->val;
            else if (cursor->val->tokentraits.identifier)
                id = cursor->val;
            cursor = cursor->next;
        }

        // string buffer = stringof(dt->tokenstr);
        output->append(stringof(dt->tokenstr));
        output->append(_space);
        // buffer = stringof(id->tokenstr);
        output->append(stringof(id->tokenstr));
        output->append(_space);
    }

    if (root->tokentraits.varassign)
    {
        std::cout << "Trigged\n";
        if (!root->tokentraits.vardeclaration)
        {
            astnode *id = NULL;
            while (cursor != NULL)
            {
                if (cursor->val->tokentraits.identifier)
                {
                    id = cursor->val;
                    break;
                }
                cursor = cursor->next;
            }
            output->append(stringof(id->tokenstr));
            output->append(_space);
        }
        
        output->append(_equal);
        output->append(_space);

        astnode *rval = NULL;
        cursor = root->children.root;
        while (cursor != NULL)
        {
            if (cursor->val->tokentraits.rvalue)
            {
                rval = cursor->val;
                break;
            }
            cursor = cursor->next;
        }

        res = generateexpression(rval);
        if (!res.ok) return res;

        output->append(res.value->getstring());
        std::cout << "What is even happening\n";
        print(*output);
    }


    if ((!root->tokentraits.varassign && !root->tokentraits.vardeclaration))
    {
        if (root->children.length > 0)
        {
            res = generateexpression(root->children.root->val);
            if (!res.ok) return res;

            output->append(res.value->getstring());
        }
    }


    std::cout << "Reached here too\n";
    output->append(_semicolon);
    return result<dstring>(output);
}


result<dstring> generateexpression(astnode *root)
{
    if (root == NULL)
    return result<dstring>(error(238, getstring_nt("Null root!")));

    dstring *output = NULL;

    result<dstring> temp;


    // if (root->tokentraits.numeric || root->tokentraits.stringliteral)
    // {
    //     if (output == NULL) output = new dstring;


    // }

    if (root->tokentraits.numeric)
    {
        output = new dstring;
        output->append(stringof(root->tokenstr));
    }
    else if (root->tokentraits.fcall)
    {
        // std::cout << "Processing: ";
        astnode *id = NULL;
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
        // id->print();
        // std::cout << "\n";

        output = new dstring;
        output->append(stringof(id->tokenstr));
        output->append(_lrb);

        bool flag = params.length > 0;

        while (!params.isempty())
        {
            temp = generateexpression(params.prepop());
            if (!temp.ok) return temp;

            // std::cout << "reached here " << (temp.value == NULL) << "\n";
            output->append(temp.value->getstring());
            output->append(_comma);
            output->append(_space);
        }

        if (flag)
        {
            output->segs.postpop();
            output->segs.postpop();
        }
        output->append(_rrb);
        
        // print(*output);
        // std::cout << "\n";
    }
    else if (root->tokentraits.op)
    {
        astnode *left = root->children.root->val,
                *right = root->children.end->val;
        
        output = new dstring;
        if (root->tokentraits.unary)
        {
            output->append(_lrb);
            output->append(stringof(root->tokenstr));
            
            if (left->tokentraits.expression||left->tokentraits.op)
            {
                temp = generateexpression(left);
                if (!temp.ok) return temp;
                output->append(temp.value->getstring());
            }
            else
            {
                output->append(stringof(left->tokenstr));
            }


            output->append(_rrb);
        }
        else if (root->tokentraits.binary)
        {
            output->append(_lrb);
            
            if (left->tokentraits.expression||left->tokentraits.op)
            {
                temp = generateexpression(left);
                if (!temp.ok) return temp;
                output->append(temp.value->getstring());
            }
            else
            {
                output->append(stringof(left->tokenstr));
            }
            
            output->append(_space);
            output->append(stringof(root->tokenstr));
            
            if (right->tokentraits.expression||right->tokentraits.op)
            {
                temp = generateexpression(right);
                if (!temp.ok) return temp;
                output->append(temp.value->getstring());
            }
            else
            {
                output->append(_space);
                output->append(stringof(right->tokenstr));
            }

            output->append(_rrb);
        }
    }
    else if (root->tokentraits.expression && (root->children.length > 0))
    {
        if (output == NULL) output = new dstring;

        output->append(_lrb);

        llnode<astnode> *pointer = root->children.root;
        while (pointer != NULL)
        {
            // std::cout << "@" ;
            // pointer->val->print();
            // std::cout << "\n" ;
            if (
                pointer->val->tokentraits.expression ||
                pointer->val->tokentraits.op 
                // pointer->val->tokentraits.identifier
            )
            {
                temp = generateexpression(root->children.root->val);
                if (!temp.ok) return temp;

                output->append(temp.value->getstring());
            }
            else
            {
                output->append(_space);
                output->append(stringof(pointer->val->tokenstr));
            }
            pointer = pointer->next;
        }

        // temp = generateexpression(root->children.root->val);
        // if (!temp.ok) return temp;
        // output->append(temp.value->getstring());


        output->append(_space);
        output->append(_rrb);
    }


    if (output != NULL) output->append("");
    return result<dstring>(output);
}


result<dstring> generatescopebody(astnode *root)
{
    if (root == NULL)
    return result<dstring>(error(238, getstring_nt("Null root!")));

    dstring *output = NULL;

    result<dstring> temp;

    if (!root->tokentraits.body)
    return result<dstring>(error(237, getstring_nt("Root is not a scope!")));

    output = new dstring;
    output->append(_lcb);
    output->append(_newline);
    output->append(_newline);

    llnode<astnode> *pointer = root->children.root;
    while (pointer != NULL)
    {
        if (pointer->val->tokentraits.statement)
        {
            std::cout << "So far so good\n";
            temp = generatestatement(pointer->val);
            if (!temp.ok) return temp;

            output->append(temp.value->getstring());
            output->append(_newline);
            output->append(_newline);
        }
        else if (pointer->val->tokentraits.controlflow)
        {
            temp = generatecflow(pointer->val);
            if (!temp.ok) return temp;

            output->append(temp.value->getstring());
            output->append(_newline);
            output->append(_newline);
        }
        else if (pointer->val->tokentraits.functiondeclaration)
        {
            temp = generatefunction(pointer->val);
            if (!temp.ok) return temp;

            output->append(temp.value->getstring());
            output->append(_newline);
            output->append(_newline);
        }


        pointer = pointer->next;
    }


    output->append(_rcb);
    output->append(_newline);
    return result<dstring>(output);
}


result<dstring> generatecflow(astnode *root)
{
    if (root == NULL)
    return result<dstring>(error(238, getstring_nt("Null root!")));

    dstring *output = NULL;

    result<dstring> temp;

    if (!root->tokentraits.controlflow)
    return result<dstring>(error(237, getstring_nt("Root is not a control flow block!")));

    output = new dstring;


    if (root->tokentraits.ifblock)
    {
        output->append(_if);
        astnode *condition = NULL, *body = NULL, *elseb = NULL;

        ll<astnode> elifs;

        llnode<astnode> *cursor = root->children.root;
        while (cursor != NULL)
        {
            if (cursor->val->tokentraits.expression)
                condition = cursor->val;
            else if (cursor->val->tokentraits.body)
                body = (cursor->val);
            else if (cursor->val->tokentraits.elifblock)
                elifs.postpend(cursor->val);
            else if (cursor->val->tokentraits.elseblock)
                elseb = (cursor->val);
            cursor = cursor->next;
        }


        output->append(_space);
        
        temp = generateexpression(condition);
        if (!temp.ok) return temp;

        output->append(_lrb);
        output->append(temp.value->getstring());
        output->append(_rrb);

        output->append(_space);

        temp = generatescopebody(body);
        if (!temp.ok) return temp;
        std::cout << "reached here\n";

        output->append(temp.value->getstring());
        output->append(_space);

        astnode *elifb;
        
        while (!elifs.isempty())
        {
            elifb = elifs.prepop();
            condition = elifb->children.root->val;
            body = elifb->children.root->next->val;

            output->append(_else);
            output->append(_space);
            output->append(_if);
            output->append(_space);

            temp = generateexpression(condition);
            if (!temp.ok) return temp;

            output->append(_lrb);
            output->append(temp.value->getstring());
            output->append(_rrb);

            output->append(_space);

            temp = generatescopebody(body);
            if (!temp.ok) return temp;

            output->append(temp.value->getstring());
            output->append(_space);
        }

        if (elseb != NULL)
        {
            body = elseb->children.root->val;
            output->append(_space);
            output->append(_else);
            output->append(_space);

            temp = generatescopebody(body);
            if (!temp.ok) return temp;

            output->append(temp.value->getstring());
            output->append(_space);
        }
        
    }
    else if (root->tokentraits.whileblock)
    {
        output->append(_while);
        astnode *condition = NULL, *body = NULL, *elseb = NULL;

        ll<astnode> elifs;

        llnode<astnode> *cursor = root->children.root;
        while (cursor != NULL)
        {
            if (cursor->val->tokentraits.expression)
                condition = cursor->val;
            else if (cursor->val->tokentraits.body)
                body = (cursor->val);
            else if (cursor->val->tokentraits.elifblock)
                elifs.postpend(cursor->val);
            else if (cursor->val->tokentraits.elseblock)
                elseb = (cursor->val);
            cursor = cursor->next;
        }


        output->append(_space);
        
        temp = generateexpression(condition);
        if (!temp.ok) return temp;

        output->append(_lrb);
        output->append(temp.value->getstring());
        output->append(_rrb);

        output->append(_space);

        temp = generatescopebody(body);
        if (!temp.ok) return temp;
        // std::cout << "reached here\n";

        output->append(temp.value->getstring());
        output->append(_space);
    }


    return result<dstring>(output);
}


result<dstring> generatefunction(astnode* root)
{
    if (root == NULL)
    return result<dstring>(error(238, getstring_nt("Null root!")));

    dstring *output = NULL;

    result<dstring> temp;

    if (!root->tokentraits.functiondeclaration)
    return result<dstring>(error(237, getstring_nt("Root is not a function declaration!")));

    output = new dstring;


    astnode *id = NULL, *frt = NULL, *body = NULL;
    ll<astnode> args;

    llnode<astnode> *cursor = root->children.root;
    while (cursor != NULL)
    {
        if (cursor->val->tokentraits.freturntype)
            frt = cursor->val;
        else if (cursor->val->tokentraits.identifier)
            id = (cursor->val);
        else if (cursor->val->tokentraits.body)
            body = (cursor->val);
        else if (cursor->val->tokentraits.argument)
            args.postpend(cursor->val);
        cursor = cursor->next;
    }    

    output->append(stringof(frt->tokenstr));
    output->append(_space);
    if (!equal(id->tokenstr, "print")) output->append(stringof(id->tokenstr));
    else 
    {
        dstring *empty = new dstring;

        empty->append("");

        return result<dstring>(empty);
    }
    output->append(_lrb);

    cursor = args.root;
    astnode *dt = NULL;// *id = NULL;
    while (cursor != NULL)
    {
        dt = cursor->val->children.root->val;
        id = cursor->val->children.root->next->val;

        output->append(stringof(dt->tokenstr));   
        output->append(_space);   
        output->append(stringof(id->tokenstr));   
        output->append(_comma);   
        output->append(_space);   

        cursor = cursor->next;
    }

    if (args.length > 0)
    {
        output->segs.postpop();
        output->segs.postpop();
    }

    output->append(_rrb);
    output->append(_space);

    temp = generatescopebody(body);
    if (!temp.ok) return temp;

    output->append(temp.value->getstring());

    return result<dstring>(output);
}