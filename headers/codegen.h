#pragma once

#include "codestrings.h"
#include "ast.h"

codestring genexpression(astnode &node)
{
    //deal with making expressions and stuff 
}

codestring genstatement(astnode& node)
{
    codestring statementstring;


    if (node.children[0]->tokentraits.vardeclarator)
    {
        stringslice dt, id;
        
        size_t i;
        for (i = 0; i < node.children.length; i++)
        {
            if (node.children[i]->tokentraits.datatype)
            {
                dt = node.children[i]->tokenstr;
                id = node.children[i + 1]->tokenstr;
                break;
            }
        }

        statementstring.appendsegment(dt);
        statementstring.appendsegment(id);

        for (i; i < node.children.length; i++)
        {
            if (node.children[i]->tokentraits.assigner)
            {
                statementstring.append(genexpression(*node.children[i + 1]));
                break;
            }
        }

        statementstring.appendsegment((char*) ";");
    }

    return statementstring;
}