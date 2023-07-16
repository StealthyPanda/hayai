#pragma once

#include "./../general/basics.h"
#include "./../general/result.h"
#include "./../frontend/ast.h"




/// @brief prunes the tree from the given root (removing empty expressions, statements etc)
/// and does basic type inferencing, type checking stuff, etc.
/// @param root 
/// @return ok if no errors, else the error
result<void> prune(astnode *root)
{
    if (root == NULL)
    return result<void>(error(253, getstring_nt("Null root!")));

    result<void> temp, ok;
    ok.ok = 1;

    llnode<astnode> *cursor = root->children.root;


    while (cursor != NULL)
    {
        temp = prune(cursor->val);
        if (!temp.ok) return temp;
        cursor = cursor->next;
    }

    if (root->tokentraits.expression)
    {
        if (root->children.length == 0)
        {
            if (!root->parent->children.remove(root))
            return result<void>(error(248, getstring_nt("Couldn't find root in root's parent's children!")));

            delete root;
            return ok;
        }
        else if (root->children.length == 1)
        {
            if (
                root->children.root->val->tokentraits.expression ||
                root->children.root->val->tokentraits.numeric
            )
            {
            
                cursor = root->parent->children.root;
                while (cursor->val != root) cursor = cursor->next;


                cursor->val = root->children.root->val;
                cursor->val->parent = root->parent;
                cursor->val->tokentraits = unionof(cursor->val->tokentraits, root->tokentraits);



                delete root;
                return ok;
            }
        }
    }
    
    return ok;
}