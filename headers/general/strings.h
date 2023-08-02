#pragma once

#include "basics.h"
#include <stdio.h>
#include "ll.h"
#include <iostream>

/// @brief a simple string type
struct string
{
    char *str = NULL;
    size_t length = 0;
    // string *nextseg = NULL;

    string() {}

    string(char *str)
    {
        this->str = str;
        this->length = 0;

        char *buff = str;
        while (*buff++ != '\0') this->length++;
    }
    
    string(char *str, size_t size)
    {
        this->str = str;
        this->length = size;
    }

    /// @brief this creates a string from start to end, INCLUDING both start and end.
    string(char *start, char *end)
    {
        this->str = start;
        this->length = (size_t)(end - start + 1);
    }

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

    string newstring = {newstr, size};
    return newstring;
}


/// @brief prints the given string to stdout
/// @param s 
void print(string *s)
{
    if ((s != NULL) && (s->str != NULL))
    {
        for (size_t i = 0; i < s->length; i++)
            printf("%c", s->str[i]);
    }
}

/// @brief prints the given string to stdout
/// @param s 
void print(string s)
{
    if (s.str == NULL) return;
    for (size_t i = 0; i < s.length; i++)
    {
        if (s.str[i] != '\n') printf("%c", s.str[i]);
        else printf("\\n");
    }
}


/// @brief checks if the given strings are equal
/// @return true if strings are equal
bool equal(string s1, string s2)
{
    if (s1.length != s2.length) return false;

    char *b1 = s1.str, *b2 = s2.str;

    for (size_t i = 0; i < s1.length; i++)
    {
        if (*b1++ != *b2++) return false;
    }
    
    return true;
}

/// @brief returns true if the strings are equal
/// @param s1 string
/// @param cs char string (HAS TO BE NULL TERMINATED)
bool equal(string s1, char *cs)
{
    string s2 = getstring_nt(cs);
    if (s1.length != s2.length) return false;

    char *b1 = s1.str, *b2 = s2.str;

    for (size_t i = 0; i < s1.length; i++)
    {
        if (*b1++ != *b2++) return false;
    }
    
    return true;
}

/// @brief converts given size_t value to a NEW string
string tostring(size_t i)
{
    char holder[100];
    sprintf(holder, "%d", i);

    return clonestring_nt(holder);
}

/// @brief returns true if the character is an alphabet, digit or underscore
bool istokenalpha(char c)
{
    return (
        ((48 <= c) && (c <= 57)) ||
        ((97 <= c) && (c <= 122)) ||
        (c == 95) ||
        ((65 <= c) && (c <= 90)) 
    );
}

bool operator==(string s1, string s2)
{
    return equal(s1, s2);
}


string operator+(string s1, string s2)
{
    char *sumstr = new char[s1.length + s2.length];

    for (size_t i = 0; i < s1.length; i++) sumstr[i] = s1.str[i];
    for (size_t i = 0; i < s2.length; i++) sumstr[i + s1.length] = s2.str[i];
    
    string sum(sumstr, s1.length + s2.length);
    return sum;
}

/// @brief returns if the given char is a whitespace
bool iswhitespace(char c)
{
    return ((c <= 32) || (c == 127));
}

/// @brief removes whitespace from the beginning and end of a string
void strip(string &s)
{
    while ((s.length > 0) && iswhitespace(*s.str))
    {
        s.str++;
        s.length--;
    }

    while ((s.length > 0) && iswhitespace(*(s.str + s.length))) s.length--;
    if ((s.length > 0) && !iswhitespace(*(s.str + s.length))) s.length++;
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
        delete[] buffer->val->str;
        delete buffer->val;
        buffer = buffer->next;
    }
}

/// @brief appends str to the string
/// @param str 
void dstring::append(string str)
{
    string *newseg = new string(clonestring(str.str, str.length));

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
            newstr[i++] = cursor->val->str[k];
        }
        cursor = cursor->next;
    }

    return newstring;
}