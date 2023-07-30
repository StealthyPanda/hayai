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
    std::cout << *res.value << std::endl;

    token *tok;
    result<token> holder;
    while (tk.available())
    {
        holder = tk.gettoken();
        if (!holder.ok)
        {
            holder.err.print();
            return 69;
        }

        holder.value->print();
        std::cout << "\n";
    }





    return 0;
}