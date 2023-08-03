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
    void reset();

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

    this->inputfile.close();

    return result<size_t>(&this->tokens.length);
}

/// @brief parses the read tokens, dealing with 2 character operaters, comments etc.
/// @return result evaluating to number of tokens after parsing
result<size_t> tokenizer::parse()
{
    llnode<token> *curr = this->tokens.root;

    llnode<token> *buffer, *end;


    while ((curr != NULL) && (curr->next != NULL))
    {
        if (
            (curr->val->props.assigner && curr->next->val->props.assigner) ||
            (curr->val->props.opexclamation && curr->next->val->props.assigner) ||
            (curr->val->props.oplesserthan && curr->next->val->props.assigner) ||
            (curr->val->props.opgreaterthan && curr->next->val->props.assigner) ||
            (curr->val->props.opminus && curr->next->val->props.assigner) ||
            (curr->val->props.opplus && curr->next->val->props.assigner) ||
            (curr->val->props.opslash && curr->next->val->props.assigner) ||
            (curr->val->props.opstar && curr->next->val->props.assigner) ||
            (curr->val->props.vertline && curr->next->val->props.opgreaterthan) ||
            (curr->val->props.opminus && curr->next->val->props.opgreaterthan) ||
            (curr->val->props.assigner && curr->next->val->props.opgreaterthan) ||
            (curr->val->props.opampersand && curr->next->val->props.opampersand) ||
            (curr->val->props.vertline && curr->next->val->props.vertline) 
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
            curr->val->props.opdot 
        )
        {
            if (
                (curr->prev != NULL) &&
                (curr->next != NULL) &&
                (curr->prev->val->props.isinteger) &&
                (curr->next->val->props.isdigits) 
            )
            {
                size_t tsize = curr->prev->val->tokenstr.length +
                               curr->next->val->tokenstr.length +
                               curr->val->tokenstr.length;
                
                char *total = new char[tsize];
                
                for (size_t i = 0; i < curr->prev->val->tokenstr.length; i++)
                    total[i] = curr->prev->val->tokenstr.str[i];

                total[curr->prev->val->tokenstr.length] = '.';

                for (size_t i = 0; i < curr->next->val->tokenstr.length; i++)
                    total[i + curr->prev->val->tokenstr.length + 1] = curr->next->val->tokenstr.str[i];
                
                token *newtok = new token(string(total, tsize), curr->val->linenumber);
                llnode<token> *newnode = new llnode<token>(newtok);
                newnode->prev = curr->prev->prev;
                newnode->next = curr->next->next;

                if (curr->next->next != NULL) curr->next->next->prev = newnode;
                if (curr->prev->prev != NULL) curr->prev->prev->next = newnode;

                this->tokens.length -= 2;            
            }
        }
        
        else if (
            curr->val->props.strmark
        )
        {
            size_t x = 1, tsize = 2;
            end = curr->next;

            while ((end != NULL) && !end->val->props.strmark)
            {
                tsize += end->val->tokenstr.length;
                end = end->next;
                x++;
            }


            if (end == NULL)
                return result<size_t>(253, "Unexpected EOF while looking for `\"`!");


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
            buffer->next = end->next;


            if (curr->prev != NULL) curr->prev->next = buffer;
            if (end->next != NULL) end->next->prev = buffer;

            if (curr == this->tokens.root) this->tokens.root = buffer;
            if (end->next == this->tokens.end) this->tokens.end = buffer;

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


/// @brief moves forward in the sequence until next item is not a whitespace
void tokenizer::prestrip()
{
    while ((this->nexttok != NULL) && this->nexttok->val->props.whitespace)
        this->nexttok = this->nexttok->next;
}

/// @brief resets the sequence back to the start
void tokenizer::reset()
{
    this->nexttok = this->tokens.root;
}
