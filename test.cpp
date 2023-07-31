#include <iostream>
// #include "./headers/frontend/newast.h"
#include "./headers/frontend/newtok.h"



int main()
{
    std::cout << "Hajimemashite\n";

    char* filename = "./syntaxstuff.hy";

    
    tokenizer tk(filename);
    result<size_t> res;

    res = tk.read();
    _validateexit(res)

    result<token> tres;
    // while (tk.available())
    // {
    //     // tk.prestrip();
    //     if (tk.available()) tres = tk.gettoken();
    //     else break;
    //     _validateexit(tres)

    //     tres.value->print();
    //     std::cout << "\n";
    // }
    // std::cout << "\n\n";


    // std::cout << "Reached here\n";
    res = tk.parse();
    _validateexit(res)

    // tk.prestrip();

    
    // result<token> tres;
    while (tk.available())
    {
        tk.prestrip();
        if (tk.available()) tres = tk.gettoken();
        else break;
        _validateexit(tres)

        tres.value->print();
        std::cout << "\n";
    }




    // ast tree(filename);
    // // tree.parse();
    // result<astnode> r = processstatement(tk);
    // if (!r.ok)
    // {
    //     r.err.print();
    //     return 69;
    // }




    return 0;
}