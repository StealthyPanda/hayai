#pragma once

#include "basics.h"
#include <stdio.h>
#include "ll.h"
#include <iostream>

/// @brief a simple string type
struct string
{
    char *string;
    size_t length;
} typedef string;

/// @brief creates and returns a string instance from a null terminated string
/// @param s a character array (HAS TO BE NULL TERMINATED)
string getstring_nt(char *s)
{
    char *buffer = s;
    size_t size = 0;
    while (*buffer++ != '\0') size++;
    
    string newstring = {s, size};
    return newstring;
}

/// @brief this makes a new copy of the string. needs to be freed manually when done.
/// @param s null terminated string.
string clonestring_nt(char *s)
{
    char *buffer = s;
    while (*buffer != '\0') buffer++;

    char *newstr = (char*) malloc((size_t)(buffer - s));
    buffer = s;

    size_t i = 0;
    while (*buffer != '\0') newstr[i++] = *buffer++;

    string newstring = {newstr, (size_t)(buffer - s)};
    return newstring;
}

/// @brief creates a string out of the given array and size 
string getstring(char *s, size_t size)
{
    string newstring = {s, size};
    return newstring;
}

/// @brief creates a new string out of the given array and size 
string clonestring(char *s, size_t size)
{
    char *newstr = new char[size];
    for (size_t i = 0; i < size; i++) newstr[i] = s[i];
    
    string newstring = {newstr, size};
    return newstring;
}

/// @brief creates a string NEW(needs to be manually freed) from start till end (END EXCLUDED).
/// Note the string created is NOT new, and simply points to the location where string literal is stored.
/// NOT RECOMMENDED to use.
string getstringfrom(char *start, char *end)
{
    string newstring = {start, (size_t)(end - start)};
    return newstring;
}


/// @brief creates a NEW(needs to be freed manually) string from start till end (END EXCLUDED).
string clonestringfrom(char *start, char *end)
{
    size_t size = (size_t)(end - start);

    char *newstr = new char[size];
    for (size_t i = 0; i < size; i++) newstr[i] = start[i];

    string newstring = {start, size};
    return newstring;
}


/// @brief prints the given string to stdout
/// @param s 
void print(string *s)
{
    for (size_t i = 0; i < s->length; i++)
        printf("%c", s->string[i]);
}

/// @brief checks if the given strings are equal
/// @return true if strings are equal
bool equal(string *s1, string *s2)
{
    if (s1->length != s2->length) return false;

    char *b1 = s1->string, *b2 = s2->string;

    for (size_t i = 0; i < s1->length; i++)
    {
        if (*b1++ != *b2++) return false;
    }
    
    return true;
}

string tostring(size_t i)
{
    char holder[100];
    sprintf(holder, "%d", i);

    return clonestring_nt(holder);
}


/// @brief dynamic strings
class dstring
{
public:
    ll<string> segs;
    size_t nsegs = 0, length = 0;

    void append(string);
    void append(char*);
    // void append(char*, char*);

    string getstring();

    dstring();
    ~dstring();
};

dstring::dstring()
{
}

dstring::~dstring()
{
    llnode<string> *buffer = segs.root;
    while (buffer != NULL)
    {
        delete[] buffer->val->string;
        delete buffer->val;
        buffer = buffer->next;
    }
}

/// @brief appends str to the string
/// @param str 
void dstring::append(string str)
{
    string *newseg = new string(clonestring(str.string, str.length));

    this->segs.postpend(newseg);
    this->nsegs++;
    this->length += str.length;
}

/// @brief appends str to the string
/// @param str 
void dstring::append(char *str)
{
    string *newseg = new string(clonestring_nt(str));
    this->segs.postpend(newseg);
    this->nsegs++;
    this->length += newseg->length;
}

/// @brief prints the dynamic string to stdout
/// @param ds 
void print(dstring& ds)
{
    llnode<string> *cursor = ds.segs.root;
    while (cursor != NULL)
    {
        print(cursor->val);
        cursor = cursor->next;
    }
}

/// @brief returns a new normal string (needs to be manually freed)
string dstring::getstring()
{
    char *newstr = new char[this->length];
    string newstring = ::getstring(newstr, this->length);

    llnode<string> *cursor = this->segs.root;

    size_t i = 0;
    while (cursor != NULL)
    {
        for (size_t k = 0; k < cursor->val->length; k++)
        {
            newstr[i++] = cursor->val->string[k];
        }
        cursor = cursor->next;
    }

    return newstring;
}