#pragma once

#include "ll.h"
#include "stringslice.h"
#include <iostream>

/// @brief this makes a copy of the given string, and will be freed whenever the instance is freed.
typedef struct stringsegment
{
    char *string = NULL;
    size_t length = 0;

    stringsegment(char* string)
    {
        char *buffer = string;
        while (*buffer++ != '\0') this->length++;
        this->string = new char[this->length];
        
        for (size_t i = 0; i < this->length; i++)
        {
            this->string[i] = string[i];
        }
    }

    stringsegment(char* string, size_t length)
    {
        this->length = length;
        this->string = new char[length];
        for (size_t i = 0; i < length; i++)
        {
            this->string[i] = string[i];
        }
    }

    stringsegment(stringslice &ss)
    {
        this->length = ss.end - ss.start + 1;
        this->string = new char[this->length];

        for (size_t i = 0; i < this->length; i++)
        {
            this->string[i] = ss.start[i];
        }
    }

    ~stringsegment()
    {
        if (this->string != NULL) delete[] this->string;
    }
};

/// @brief prints the given stringsegment to the standard output
void print(stringsegment &ss)
{
    for (size_t i = 0; i < ss.length; i++)
        std::cout << ss.string[i];
}

/// @brief returns a stringslice of the given stringsegment from start to end.
stringslice slice(stringsegment &ss)
{
    stringslice sl = {ss.string, ss.string + ss.length - 1};
    return sl;
}


/// @brief A singular piece of codestring. Basically, a linked list of stringsegments.
/// All stringsegments made inside codestring's methods are automatically dealt with in its destructor.
class codestring
{
public:
    codestring();
    ~codestring();
    
    ll<stringsegment> segments;
    size_t nsegments = 0;

    void appendsegment(stringslice &ss);
    void prependsegment(stringslice &ss);

    void append(codestring &ss);
    void prepend(codestring &ss);

    void appendsegment(char *string);
    void prependsegment(char *string);

    void appendsegment(char *string, size_t length);
    void prependsegment(char *string, size_t length);
    // void insertsegment(stringslice &ss);
    // void prependsegment(stringslice &ss);

    void insert(stringslice &ss, size_t index);
    stringsegment* remove(size_t index);
    void nuke(size_t index);

    void print();

    stringslice conjoin();
    stringslice conjoin(char);
    stringslice conjoin(stringslice &ss);
};

codestring::codestring()
{
}

codestring::~codestring()
{
    llnode<stringsegment> *buffer = segments.root, *holder;
    size_t i = 0;
    while (buffer != NULL)
    {
        // std::cout << "deleted " << i++ << " times\n";
        delete buffer->val;
        // std::cout << "reacher here";
        holder = buffer->next;
        delete buffer;
        buffer = holder;
    }
}

/// @brief adds a segment at the end from the given stringslice
/// @param ss 
void codestring::appendsegment(stringslice &ss)
{
    this->segments.postpend(new stringsegment(ss));
    this->nsegments++;
}

/// @brief adds a stringsegment at the beginning from the stringslice.
/// @param ss 
void codestring::prependsegment(stringslice &ss)
{
    this->segments.prepend(new stringsegment(ss));
    this->nsegments++;
}

/// @brief adds a segment at the end from the given string.
/// @param string (HAS to be null terminated)
void codestring::appendsegment(char *string)
{
    this->segments.postpend(new stringsegment(string));
    this->nsegments++;
}


/// @brief adds a segment at the beginning from the given string
/// @param string (HAS to be null-terminated)
void codestring::prependsegment(char *string)
{
    this->segments.prepend(new stringsegment(string));
    this->nsegments++;
}

/// @brief adds a segment at the end from the given string.
/// @param string (need not be null terminated)
void codestring::appendsegment(char *string, size_t length)
{
    this->segments.postpend(new stringsegment(string, length));
    this->nsegments++;
}


/// @brief adds a segment at the beginning from the given string
/// @param string (need not be null-terminated)
void codestring::prependsegment(char *string, size_t length)
{
    this->segments.prepend(new stringsegment(string, length));
    this->nsegments++;
}

/// @brief appends the given codestring at the end of this one
void codestring::append(codestring& other)
{
    stringsegment *holder;
    for (size_t i = 0; i < other.nsegments; i++)
    {
        holder = other.segments[i];
        this->appendsegment(holder->string, holder->length);
    }
}

/// @brief appends the given codestring at the beginning of this one.
/// @param other 
void codestring::prepend(codestring& other)
{
    stringsegment *holder;
    for (size_t i = other.nsegments - 1; i >= 0; i--)
    {
        holder = other.segments[i];
        this->prependsegment(holder->string, holder->length);
    }
}

/// @brief inserts the given stringslice at the given index
/// @param ss 
/// @param index the new stringsegment is inserted between index and index + 1 indices.
void codestring::insert(stringslice &ss, size_t index)
{
    this->segments.insert(new stringsegment(ss), index);
    this->nsegments++;
}

/// @brief returns a pointer to a string that is NOT null terminated, and has to be deleted later.
// To automatically delete the string as well, use `nuke` instead.
stringsegment* codestring::remove(size_t index)
{
    this->nsegments--;
    return this->segments.remove(index);
}

/// @brief deletes the node at the given index, ALONG with the string.
// To not automatically delete the string as well, use `remove` instead.
void codestring::nuke(size_t index)
{
    this->nsegments--;
    delete[] this->segments.remove(index);
}

/// @brief simply prints all stringsegments in order
void codestring::print()
{
    llnode<stringsegment> *buffer = this->segments.root;
    std::cout << "[";
    while (buffer->next != NULL)
    {
        ::print(*buffer->val);
        std::cout << ", ";
        buffer = buffer->next;
    }
    ::print(*buffer->val);
    std::cout << "]";
}

/// @brief returns a string slice that has reference to a string that has been created with new.
// So basically the returned string slice can be deleted, changed, whatever, as long as the char* it is pointing to
// is not freed. Only free it if you are sure you will never need this conjoined string again. 
stringslice codestring::conjoin()
{
    if (this->segments.isempty()) return stringslice();

    size_t fulllength = 0;
    llnode<stringsegment> *buffer = this->segments.root;
    
    while (buffer != NULL)
    {
        fulllength += buffer->val->length;
        buffer = buffer->next;
    }

    char *fullstring = new char[fulllength];
    char *pointer = fullstring;

    buffer = this->segments.root;
    while (buffer != NULL)
    {
        for (size_t i = 0; i < buffer->val->length; i++)
            *pointer++ = buffer->val->string[i];
        
        buffer = buffer->next;
    }

    stringslice ss = {fullstring, fulllength + fullstring - 1};

    return ss;
}


/// @brief returns a string slice that has reference to a string that has been created with new.
// So basically the returned string slice can be deleted, changed, whatever, as long as the char* it is pointing to
// is not freed. Only free it if you are sure you will never need this conjoined string again. 
/// @param c character placed in between each stringsegment.
stringslice codestring::conjoin(char c)
{
    if (this->segments.isempty()) return stringslice();


    size_t fulllength = 0;
    llnode<stringsegment> *buffer = this->segments.root;
    
    while (buffer != NULL)
    {
        fulllength += buffer->val->length;
        buffer = buffer->next;
    }

    fulllength += this->nsegments - 1;

    char *fullstring = new char[fulllength];
    char *pointer = fullstring;

    buffer = this->segments.root;
    while (buffer != NULL)
    {
        for (size_t i = 0; i < buffer->val->length; i++)
            *pointer++ = buffer->val->string[i];
        
        *pointer++ = c;

        buffer = buffer->next;
    }

    stringslice ss = {fullstring, fulllength + fullstring - 1};

    return ss;
}


/// @brief returns a string slice that has reference to a string that has been created with new.
// So basically the returned string slice can be deleted, changed, whatever, as long as the char* it is pointing to
// is not freed. Only free it if you are sure you will never need this conjoined string again.
/// @param c string placed in between each stringsegment.
stringslice codestring::conjoin(stringslice &c)
{
    if (this->segments.isempty()) return stringslice();

    size_t fulllength = 0, clength = (c.end - c.start + 1);
    llnode<stringsegment> *buffer = this->segments.root;
    
    while (buffer != NULL)
    {
        fulllength += buffer->val->length;
        buffer = buffer->next;
    }

    fulllength += ((this->nsegments - 1) * clength);

    char *fullstring = new char[fulllength];
    char *pointer = fullstring;

    buffer = this->segments.root;
    while (buffer != NULL)
    {
        for (size_t i = 0; i < buffer->val->length; i++)
            *pointer++ = buffer->val->string[i];
        
        for (size_t i = 0; i < clength; i++)
            *pointer++ = c.start[i];

        buffer = buffer->next;
    }

    stringslice ss = {fullstring, fulllength + fullstring - 1};

    return ss;
}