#pragma once

#include <fstream>

#include "./../general/ll.h"
#include "./../general/result.h"
#include "./../general/cvals.h"
#include "./../general/strings.h"
#include "./../general/traits.h"
#include "./../general/stringslice.h"

#define tokbuffsize 1 << 10

struct token
{
    string tokenstr;
    size_t linenumber = 0;
    traits props;

    token() {}

    token(string str, size_t ln)
    {
        this->tokenstr = str;
        this->linenumber = ln;
        this->props = gettraits(getslice(&this->tokenstr));
    }

    void print()
    {
        std::cout << "[";
        ::print(tokenstr);
        std::cout << "](" << props << " " << this->linenumber << ")";
    }

} typedef token;



class tokenizer
{
public:
    tokenizer(char *filepath);
    ~tokenizer();

    char *filepath = NULL;
    size_t ln = 1;
    char tokbuff[tokbuffsize];

    ll<token> tokens;
    llnode<token> *nexttok;

    std::ifstream inputfile;

    result<size_t> read();
    result<size_t> parse();

    result<token> gettoken();
    result<token> peek();
    bool available();

    void prestrip();
    // void prestrip();

    void flush();
};

tokenizer::tokenizer(char *filepath)
{
    this->filepath = filepath;
}

tokenizer::~tokenizer()
{
}

/// @brief flushes the entire token buffer to null characters
void tokenizer::flush()
{
    for (size_t i = 0; i < tokbuffsize; i++)
        this->tokbuff[i] = 0;
}


/// @brief reads file and stores all the detected tokens in a linked list
/// @return result that evaluates to no. of tokens in the file
result<size_t> tokenizer::read()
{
    if (this->filepath != NULL)
        this->inputfile = std::ifstream(this->filepath);
    else
        return result<size_t>(255, "No filepath passed!");
    
    if (!this->inputfile)
        return result<size_t>(255, "File cannot be opened!");
    
    for (size_t i = 0; i < tokbuffsize; i++)
        this->tokbuff[i] = 0;
    
    token *buffer;

    char *buff = this->tokbuff;
    while (!this->inputfile.eof())
    {
        if (((size_t)(buff - this->tokbuff)) >= (tokbuffsize))
            return result<size_t>(254, "Token size greater than tokbuffsize!");
        
        *buff = this->inputfile.get();

        if (*buff == '\n') this->ln++;

        if (istokenalpha(*buff)) buff++;
        else
        {
            if (buff == this->tokbuff)
                this->tokens.postpend(new token(clonestring(this->tokbuff, 1), this->ln));
            else
            {
                this->tokens.postpend(new token(clonestringfrom(this->tokbuff, buff), this->ln));
                this->tokens.postpend(new token(clonestring(buff, 1), this->ln));
            }
            buff = this->tokbuff;
        }
    }
    
    this->tokens.prepend(new token(string(" "), 0));
    this->tokens.postpend(new token(string(" "), 0));

    this->nexttok = this->tokens.root;

    return result<size_t>(&this->tokens.length);
}

/// @brief parses the read tokens, dealing with 2 character operaters, comments etc.
/// @return result evaluating to number of tokens after parsing
result<size_t> tokenizer::parse()
{
    llnode<token> *curr = this->tokens.root;

    llnode<token> *buffer, *end;

    bool done = false;

    while ((curr != NULL) && (curr->next != NULL))
    {
        if (
            (equal(curr->val->tokenstr, _equal) && equal(curr->next->val->tokenstr, _equal)) ||
            (equal(curr->val->tokenstr, _exclamationmark) && equal(curr->next->val->tokenstr, _equal)) ||
            (equal(curr->val->tokenstr, _lesser) && equal(curr->next->val->tokenstr, _equal)) ||
            (equal(curr->val->tokenstr, _greater) && equal(curr->next->val->tokenstr, _equal)) ||
            (equal(curr->val->tokenstr, _minus) && equal(curr->next->val->tokenstr, _greater)) ||
            (equal(curr->val->tokenstr, _vertline) && equal(curr->next->val->tokenstr, _greater)) ||
            (equal(curr->val->tokenstr, _plus) && equal(curr->next->val->tokenstr, _equal)) ||
            (equal(curr->val->tokenstr, _star) && equal(curr->next->val->tokenstr, _equal)) ||
            (equal(curr->val->tokenstr, _fslash) && equal(curr->next->val->tokenstr, _equal)) ||
            (equal(curr->val->tokenstr, _minus) && equal(curr->next->val->tokenstr, _equal))
        )
        {

            buffer = new llnode<token>;
            buffer->val = new token((curr->val->tokenstr + curr->next->val->tokenstr), curr->val->linenumber);
            buffer->prev = curr->prev;
            buffer->next = curr->next->next;

            if (curr->prev != NULL) curr->prev->next = buffer;
            if (curr->next->next != NULL) curr->next->next->prev = buffer;

            if (curr == this->tokens.root) this->tokens.root = buffer;
            if (curr->next == this->tokens.end) this->tokens.end = buffer;

            this->tokens.length--;
        }
        else if (
            (equal(curr->val->tokenstr, _fslash) && equal(curr->next->val->tokenstr, _fslash))
        )
        {
            size_t x = 0;
            end = curr;
            // std::cout << "Reached here\n";
            while ((end->next != NULL) && !equal(end->next->val->tokenstr, _newline))
            {
                end = end->next;
                x++;
            }

            if (curr->prev != NULL) curr->prev->next = end->next;
            if (end->next != NULL) end->next->prev = curr->prev;

            if (curr == this->tokens.root) this->tokens.root = buffer;
            if (curr->next == this->tokens.end) this->tokens.end = buffer;

            this->tokens.length -= (x + 1);

            curr->next = end->next;
        }
        else if (
            curr->val->props.strmark && !done
        )
        {
            // done = true;
            // std::cout << "in here\n";
            size_t x = 0, tsize = 1;
            end = curr->next;
            // std::cout << "Reached here\n";
            while ((end != NULL) && !end->val->props.strmark)
            {
                tsize += end->val->tokenstr.length;
                end = end->next;
                x++;
            }
            // if (x == 0) x = 1;

            std::cout << "Currently end is at ";
            end->val->print();
            std::cout << "\n";


            if (end == NULL)
            {
                return result<size_t>(253, "Unexpected EOF while looking for `\"`!");
            }

            // std::cout << "Got here\n";

            char *concated = new char[tsize];
            char *buff = concated;
            llnode<token> *cursor = curr;
            while (cursor != end->next)
            {
                for (size_t i = 0; i < cursor->val->tokenstr.length; i++)
                {
                    *buff++ = cursor->val->tokenstr.str[i];
                }
                cursor = cursor->next;
            }

            buffer = new llnode<token>(new token(string(concated, buff - 1), curr->val->linenumber));
            buffer->prev = curr->prev;
            buffer->next = end->next->next;
            // std::cout << "Processed string literal: ";
            // buffer->val->print();
            // std::cout << "\n";


            if (curr->prev != NULL) curr->prev->next = buffer;
            if (end->next != NULL) end->next->prev = buffer;

            if (curr == this->tokens.root) this->tokens.root = buffer;
            if (curr->next == this->tokens.end) this->tokens.end = buffer;

            this->tokens.length -= x;

            curr->next = end->next;
        }

        curr = curr->next;
    }

    this->nexttok = this->tokens.root;

    return result<size_t>(&this->tokens.length);
}


/// @brief returns the next token in sequence, if available
result<token> tokenizer::gettoken()
{

    if (this->nexttok != NULL)
    {
        auto retter = result<token>(this->nexttok->val);
        this->nexttok = this->nexttok->next;
        return retter;
    }
    else
        return result<token>(255, "All out of tokens!");
}

/// @brief returns the next token in sequence, if available, WITHOUT ADVANCING THE SEQUENCE
result<token> tokenizer::peek()
{

    if (this->nexttok != NULL)
        return result<token>(this->nexttok->val);
    else
        return result<token>(255, "All out of tokens!");
}

///@brief returns true if tokens are left in sequence
bool tokenizer::available()
{
    return (this->nexttok != NULL);
}



void tokenizer::prestrip()
{
    while ((this->nexttok != NULL) && this->nexttok->val->props.whitespace)
        this->nexttok = this->nexttok->next;
}


