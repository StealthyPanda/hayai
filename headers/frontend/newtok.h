#pragma once

#include "./../general/ll.h"
#include "./../general/result.h"
#include "./../general/cvals.h"

#include <fstream>

class tokenizer
{
public:
    tokenizer(char *filepath);
    ~tokenizer();

    char *filepath = NULL;

    std::ifstream inputfile;

    result<size_t> read();
};

tokenizer::tokenizer(char *filepath)
{
    this->filepath = filepath;
}

tokenizer::~tokenizer()
{
}

result<size_t> tokenizer::read()
{
    if (this->filepath != NULL)
        this->inputfile = std::ifstream(this->filepath);
    else
        return result<size_t>();
}
