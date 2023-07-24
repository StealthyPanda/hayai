#include <iostream>
#include "headers/frontend/ast.h"
#include "headers/mid/opt.h"
#include "headers/mid/tc.h"
#include "headers/rearend/cbackend.h"
// #include "headers/rearend/cbackend.h"

int main()
{
    std::cout << "Hajimemashite\n";


    ast tree("./main.hy");
    cbackend cb(&tree);


    // std::cout << "reached here\n";
    result<void> res = tree.parse();
    if (!res.ok)
    {
        res.err.print();
        return 69;
    }

    prune(tree.root);
    tree.printbf();


    // ftypeinfo *printsting = new ftypeinfo[5];
    // for (size_t i = 0; i < 5; i++)
    // {
    //     printsting[i].id = getstring_nt("printf");
    // }
    




    typechecker tc(&tree);
    result<string> r;
    r = tc.check(tree.root);
    if (!r.ok)
    {
        r.err.print();
        return 69;
    }

    std::cout << "\n\n";
    tree.printbf();
    std::cout << "\n\n";


    char *ofile = "./devoutput/output.c";
    result<void> fr = cb.generateoutput(ofile);

    if (!fr.ok)
    {
        fr.err.print();
        return 69;
    }

    std::cout << "\nOutput file sucessfully generated at `"
    << "./devoutput/output.c" << "`!";

    char wait;
    // std::cin >> wait;

    return 0;
}