
#include "./../general/strings.h"

string getstring_nt(char *s)
{
    char *buffer = s;
    size_t size = 0;
    while (*buffer++ != '\0') size++;
    
    string newstring = {s, size};
    return newstring;
}

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

string getstring(char *s, size_t size)
{
    string newstring = {s, size};
    return newstring;
}

string clonestring(char *s, size_t size)
{
    char *newstr = new char[size];
    for (size_t i = 0; i < size; i++) newstr[i] = s[i];
    
    string newstring = {newstr, size};
    return newstring;
}

string getstringfrom(char *start, char *end)
{
    string newstring = {start, (size_t)(end - start)};
    return newstring;
}

string clonestringfrom(char *start, char *end)
{
    size_t size = (size_t)(end - start);

    char *newstr = new char[size];
    for (size_t i = 0; i < size; i++) newstr[i] = start[i];

    string newstring = {start, size};
    return newstring;
}

void print(string *s)
{
    for (size_t i = 0; i < s->length; i++)
        printf("%c", s->string[i]);
}

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

void dstring::append(string str)
{
    string *newseg = new string(clonestring(str.string, str.length));

    this->segs.postpend(newseg);
    this->nsegs++;
    this->length += str.length;
}


 
void dstring::append(char *str)
{
    string *newseg = new string(clonestring_nt(str));
    this->segs.postpend(newseg);
    this->nsegs++;
    this->length += newseg->length;
}

void print(dstring& ds)
{
    llnode<string> *cursor = ds.segs.root;
    while (cursor != NULL)
    {
        print(cursor->val);
        cursor = cursor->next;
    }
}

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