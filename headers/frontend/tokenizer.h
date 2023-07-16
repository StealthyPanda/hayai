#pragma once

#include <fstream>
#include "./../general/stringslice.h"
#include "./../general/ll.h"
#include "./../general/result.h"

/*
Important:

TODO:
    ->Add comments feature by removing those tokens at the tokenizer level.
    ->Right now if the input file doesn't exist the compiler is completely crashing,
      deal with that at the tokenizer level.

*/


size_t getlinenumber(char *file, char *pointer)
{
    size_t line = 0;
    char *buffer = file;
    while (buffer < pointer)
    {
        if (*buffer++ == '\n') line++;
    }
    return line;
}

string getlineat(char *file, size_t size, size_t index)
{
    char *cursor = file;
    while (index --> 0)
    {
        while ((*cursor != '\n') && ((cursor - file) < size)) cursor++;
        cursor++;
    }

    char *end = cursor + 1;
    while ((*end != '\n') && ((end - file) < size)) end++;

    return clonestring(cursor, (size_t)(end - cursor));
}

/// @brief A tokenizer that processes the raw code from a file, and gives individual tokens in stringslices.
class tokenizer
{
public:
    tokenizer(const char* filepath);
    ~tokenizer();

    const char *filepath;
    char *file, *start, *end, *buffer;


    ll<stringslice> tokens;
    llnode<stringslice> *cursor;

    size_t filesize, length, index = 0, nstrmark = 0;

    stringslice _gettoken();
    stringslice gettoken();
    bool _tokensleft();
    bool tokensleft();

    result<size_t> read();
    result<size_t> parse();

    void prestrip();

    stringslice peek();

    stringslice* operator[](size_t index)
    {
        return this->tokens[index];
    }

};

tokenizer::tokenizer(const char* filepath)
{
    this->filepath = filepath;
}


/// @brief Creates a tokenizer. After creating the object, all the tokens in the code are stored in a linked
/// list called tokens, and need to be parsed to work properly.
/// @returns a result that evaluates to number of tokens read successfully.
result<size_t> tokenizer::read()
{
    result<size_t> res;
    res.ok = false;

    //this block creates a file stream, seeks to the end, and gives the number of bytes till this point
    //(essentially the size of the file in bytes)
    std::ifstream infile(filepath, std::ifstream::ate | std::ifstream::binary);
    this->filesize = infile.tellg();


    //if file is empty or cant be opened, return error.
    if (!this->filesize)
    {
        dstring emsg;

        emsg.append("Unable to open file or file is empty at `");
        emsg.append((char*) this->filepath);
        emsg.append("`!");

        res = result<size_t>(error(42, emsg.getstring()));
    }

    //store file in a char field
    this->file = new char[this->filesize];

    infile = std::ifstream(filepath);
    infile.read(this->file, this->filesize);

    this->start = file;
    this->end = file;
    this->buffer = file;

    //count number of \n in the file, and remove them from file size. (some weird bs)
    while ( (size_t)(this->buffer - this->file) < this->filesize)
    {
        if (*this->buffer++ == '\n') filesize--;
    }
    this->buffer = NULL;

    if (!this->filesize)
    {
        dstring emsg;

        emsg.append("Empty file at `");
        emsg.append((char*) this->filepath);
        emsg.append("`!");

        res = result<size_t>(error(42, emsg.getstring()));
        return res;
    }

    //append all tokens from the file read to linkedlist
    while (this->_tokensleft())
    {
        this->tokens.postpend(new stringslice(this->_gettoken()));
    }

    this->length = this->tokens.length;

    size_t *ntokens = new size_t(this->length);
    res = result<size_t>(ntokens);
    return res;
}

///IMPORTANT:
/// Fix bug in tokenizer::parse where if the first token of the program is ",
/// things go haywire.

/// @brief Parses the tokens to deal with 2 char operators like ==, =>, |> etc.
/// @return returns result that evaluates to number of tokens.
result<size_t> tokenizer::parse() 
{
    result<size_t> r;
    r.ok = true;

    //traversing the linked list of tokens
    llnode<stringslice> *sslln = this->tokens.root->next;
    stringslice *ssbuff;
    while (sslln != NULL)
    {
        ssbuff = sslln->val;

        //this block deals with all 2 char tokens like <=, >=, |> etc.
        if ((equal(*ssbuff, (char*)">") && (equal(*sslln->prev->val, (char*)"=") || equal(*sslln->prev->val, (char*)"|") || equal(*sslln->prev->val, (char*)"-"))) ||
            (equal(*ssbuff, (char*)"=") && (
            equal(*sslln->prev->val, (char*)"<") ||
            equal(*sslln->prev->val, (char*)">") ||
            equal(*sslln->prev->val, (char*)"+") ||
            equal(*sslln->prev->val, (char*)"-") ||
            equal(*sslln->prev->val, (char*)"*") ||
            equal(*sslln->prev->val, (char*)"/") ||
            equal(*sslln->prev->val, (char*)"!") ||
            equal(*sslln->prev->val, (char*)"=")
            )) ||
            (equal(*ssbuff, (char*)"&") && equal(*sslln->prev->val, (char*)"&")) ||
            (equal(*ssbuff, (char*)"|") && equal(*sslln->prev->val, (char*)"|")) 
        )
        {
            llnode<stringslice> *newnode = new llnode<stringslice>();
            stringslice *newslice = new stringslice;

            newnode->val = newslice;

            newslice->start = sslln->prev->val->start;
            newslice->end = ssbuff->end;

            newnode->prev = sslln->prev->prev;
            newnode->next = sslln->next;

            sslln->prev->prev->next = newnode;
            sslln->next->prev = newnode;

            delete sslln->prev;
            delete sslln;

            this->tokens.length -= 1;
            this->length -= 1;
        }

        //dealing with string tokens at tokenizer level
        if (equal(*ssbuff, (char*)"\""))
        {
            llnode<stringslice> *till = sslln->next;

            size_t nnodes = 0;
            while (!equal(*till->val, (char*)"\""))
            {
                //if matching " is not found:
                if (till == this->tokens.end)
                {
                    dstring emsg;

                    size_t ln = getlinenumber(this->file, sslln->val->start);

                    emsg.append("Unpaired `\"` at line ");
                    emsg.append(tostring(ln + 1));
                    emsg.append(":\n");
                    emsg.append(getlineat(this->file, this->filesize, ln));

                    r = result<size_t>(error(69, emsg.getstring()));
                    
                    return r;
                }
                till = till->next;
                nnodes++;
            }
            nnodes++;

            // this block turns the entire string literal into one token.
            llnode<stringslice> *newnode = new llnode<stringslice>();
            newnode->prev = sslln->prev;
            newnode->next = till->next;

            sslln->prev->next = newnode;
            till->next->prev = newnode;

            newnode->val = new stringslice;
            newnode->val->start = ssbuff->start;
            newnode->val->end = till->val->end;

            till = till->prev;
            while (till != sslln)
            {
                delete till->next;
                till = till->prev;
            }

            delete sslln;
            sslln = newnode;

            this->tokens.length -= nnodes;
            this->length -= nnodes;
        }

        
        //this block deals with numerics, integers, numbers etc.
        if (sslln->next != NULL)
        {
            if (equal(*ssbuff, (char*)".") && isint(*sslln->prev->val) && isdigitsonly(*sslln->next->val))
            {
                llnode<stringslice> *newnode = new llnode<stringslice>();
                stringslice *newslice = new stringslice;
                newnode->val = newslice;

                newslice->start = sslln->prev->val->start;
                newslice->end = sslln->next->val->end;

                newnode->prev = sslln->prev->prev;
                newnode->next = sslln->next->next;

                if (sslln->prev->prev != NULL) sslln->prev->prev->next = newnode;
                if (sslln->next->next != NULL) sslln->next->next->prev = newnode;


                if (sslln->prev == this->tokens.root) this->tokens.root = newnode;
                if (sslln->next == this->tokens.end) this->tokens.end = newnode;

                delete sslln->prev;
                delete sslln->next;
                delete sslln;

                this->tokens.length -= 2;
                this->length -= 2;
            }
            else if (equal(*ssbuff, (char*)"-") && isnum(*sslln->next->val))
            {
                llnode<stringslice> *newnode = new llnode<stringslice>();
                stringslice *newslice = new stringslice;
                newnode->val = newslice;

                newslice->start = ssbuff->start;
                newslice->end = sslln->next->val->end;

                newnode->prev = sslln->prev;
                newnode->next = sslln->next->next;

                sslln->prev->next = newnode;
                sslln->next->next->prev = newnode;

                delete sslln->next;
                delete sslln;

                this->tokens.length -= 1;
                this->length -= 1;
            }
        }
        

        sslln = sslln->next;
    }

    this->cursor = this->tokens.root;

    size_t *ntoks = new size_t(this->tokens.length);
    r = result<size_t>(ntoks);
    return r;
}

tokenizer::~tokenizer()
{
}



/// @brief for internal use only
stringslice tokenizer::_gettoken()
{
    stringslice ss = {this->start, this->start};
    if (!istokenalpha(*this->start))
    {
        if (!((*this->start == ' ') || (*this->start == '\n') || (*this->start == '\t')))
        {
            ss.start = this->start;
            ss.end = this->start;
        }
        this->start++;
        this->end = this->start;
    }
    else
    {
        this->end = this->start + 1;
        while (istokenalpha(*this->end)) this->end++;
        ss.start = this->start;
        ss.end = this->end - 1;
        this->start = this->end;
    }
    return ss;
}

/// @brief returns the next stringslice in the sequence, from the raw code.
stringslice tokenizer::gettoken()
{
    if (cursor != NULL)
    {
        stringslice retter = *this->cursor->val;
        this->cursor = this->cursor->next;
        return retter;
    }
}

/// @brief returns the next stringslice in the sequence, from the raw code WITHOUT ADVANCING
stringslice tokenizer::peek()
{
    if (cursor != NULL) return *this->cursor->val;
}

/// @brief for internal use only
bool tokenizer::_tokensleft()
{
    return ( (size_t)(end - file) < filesize);
}

/// @brief returns true if there are tokens still left in the sequence.
bool tokenizer::tokensleft()
{
    return (cursor != NULL);
}

/// @brief removes all whitespace tokens from the beginning of the list
void tokenizer::prestrip()
{
    stringslice ssbuff = this->peek();
    while (this->tokensleft() && iswhitespace(ssbuff))
    {
        this->gettoken();
        ssbuff = this->peek();
    }
}