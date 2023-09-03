#include <iostream>
#include "./headers/frontend/ast.h"
// #include "./headers/frontend/newtok.h"


// int main()
// {
//     std::cout << "Hajimemashite\n";

//     char* filename = "./playground.hy";

    
//     tokenizer tk(filename);
//     result<size_t> res;

//     res = tk.read();
//     _validateexit(res)

//     res = tk.parse();
//     _validateexit(res)

//     // llnode<token> *cursor = tk.tokens.root;
//     // while (cursor != NULL)
//     // {
//     //     cursor->val->print();
//     //     std::cout << "\n";
//     //     cursor = cursor->next;
//     // }


//     result<astnode> noderes;

//     // tk.prestrip();
//     // result<bool> huh = istuple(tk, tk.nexttok);
//     // _validateexit(huh)
//     // std::cout << "istuple: " << (int) *huh.value << "\n";

//     // for (size_t i = 0; i < 3; i++)
//     {
//         noderes = processmacro(tk);
//         // noderes = processtype(tk);
//         _validateexit(noderes);

//         ast dummy(filename);
//         dummy.root = noderes.value;

//         // std::cout << "reached printbf\n";
//         dummy.printbf();
//     }


//     return 0;
// }

int main()
{
    std::cout << "Hajimemashite\n\n";

    ast tree(getstring_nt("./playground.hy"));

    _consumeexit(tree.parse())

    tree.printbf();

    return 0;
}