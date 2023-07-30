#include <iostream>
#include "./headers/frontend/newtok.h"



int main()
{
    std::cout << "Hajimemashite\n";

    char* filename = "./syntaxstuff.hy";

    tokenizer tk(filename);
    result<size_t> res = tk.read();

    if (!res.ok)
    {
        res.err.print();
        return 69;
    }

    std::cout << tk.tokens.length << std::endl;

    res = tk.parse();

    if (!res.ok)
    {
        res.err.print();
        return 69;
    }

    llnode<token> *cursor = tk.tokens.root;
    while (cursor != NULL)
    {
        cursor->val->print();
        std::cout << "\n";
        cursor = cursor->next;
    }

    std::cout << *res.value << std::endl;


    return 0;
}