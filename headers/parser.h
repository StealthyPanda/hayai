#pragma once

#include <iostream>
#include "ast.h"
#include "tokenizer.h"



class parser
{
public:
    parser(char *filename);
    ~parser();

    char *filename = NULL;

    size_t filesize = 0;

    ast tree;
    tokenizer *tk;

    void printtree();
    bool parse();
};

parser::parser(char *filename)
{
    this->filename = filename;
    tk = new tokenizer(filename);
}

bool parser::parse()
{
    if (!tk->parse())
    {
        std::cout << "\nError in tokenisation, exiting compilation...\n";
        return false;
    }

    this->filesize = tk->filesize;

    // char buff;
    for (size_t i = 0; i < tk->length; i++)
    {
        if (!tree.appendnode(*(*tk)[i]))
        {
            std::cout << "\nError in processing token `";
            ::print(*(*tk)[i]);
            std::cout << "`, exiting compilation...\n";
            return false;
        }
        // tree.printbf();
        // std::cout << '\n\n';
        // std::cin >> buff;
    }
    
    std::cout << "Parsed file successfully, read " << this->filesize << " bytes\n";
    return true;
}

parser::~parser()
{
}


void parser::printtree()
{
    this->tree.printbf();
}

