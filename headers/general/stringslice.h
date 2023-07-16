#pragma once

#include <iostream>
#include "strings.h"

/// @brief returns if the given character is either an alphabet or number or '_'
/// @param c the character to be evaluated 
bool istokenalpha(char c)
{
    if ((97 <= c) && (c <= 122)) return true;
    if ((65 <= c) && (c <= 90)) return true;
    if ((48 <= c) && (c <= 57)) return true;
    return (c == '_');
}

/// @brief a struct that makes working with string slices easier
/// Contains a start and end char* that point from the first till the last (both inclusive) characters of the string.
struct stringslice
{
    char *start = NULL, *end = NULL;
} typedef stringslice;

/// @brief returns length of the given stringslice
/// @param ss the slice to be evaluated
size_t length(stringslice &ss)
{
    if ((ss.start == NULL) || (ss.end == NULL)) return 0;
    return (ss.end - ss.start + 1)/sizeof(char);
}

/// @brief returns a slice of the entire string
stringslice getslice(string *str)
{
    stringslice ss = {str->string, str->string + str->length - 1};
    return ss;
}


/// @brief returns a slice of the string from index start to end (start included, end excluded).
stringslice getslice(string *str, size_t start, size_t end)
{
    stringslice ss = {str->string + start, str->string + end - 1};
    return ss;
}

/// @brief gives a string from the stringslice (NOT NEW)
string stringof(stringslice &ss)
{
    string s = getstring(ss.start, (size_t)(ss.end - ss.start + 1));
    return s;
}

/// @brief prints the given stringslice to standard output
void print(stringslice &ss)
{
    if ((ss.start == NULL) || (ss.end == NULL)) return;
    char *buffer = ss.start;
    while (buffer <= ss.end)
    {
        if (*buffer == '\n') std::cout << "\\n";
        else if ((*buffer == ' ') && (length(ss) == 1)) std::cout << "*space*";
        else if ((*buffer == '\t') && (length(ss) == 1)) std::cout << "*tab*";
        else std::cout << *buffer;
        buffer++;
    }
}

/// @brief returns if the stringslice is exactly '\\n'
bool isnewline(stringslice &ss)
{
    return ((ss.start == ss.end) && (*ss.start == '\n'));
}

/// @brief returns if the given stringslice is whitespace.
/// any character that doesn't print anything concrete to the standard output (for eg newline, return carriage, null)
/// is considered a whitespace character.
/// @param ss the stringslice to be evaluated
/// @return true if all the characters are whitespace characters in the stringslice, else false
bool iswhitespace(stringslice &ss)
{
    char *buffer = ss.start;
    while (buffer <= ss.end)
    {
        if (*buffer++ > 32) return false;
    }
    return true;
}


/// @brief returns if a stringslice is a number
/// numbers are those that contain only digits, and a single period (for decimal point) and a single + or - 
/// at the beginning (for sign).
/// are nums: 69, 420, 3.14, -770.123, +770.123
/// aren't nums: 3.2.1, 3-4.2 etc.
/// @param ss stringslice to be evaluated
/// @return true if is a number
bool isnum(stringslice &ss)
{
    char *buff = ss.start;

    bool flag = true;

    while (buff <= ss.end)
    {
        if (!((48 <= *buff) && (*buff <= 57)))
        {
            if (!((buff == ss.start) && ((*buff == '-') || (*buff == '+'))))
            {
                if ((*buff == '.') && flag) flag = false;
                else return false;
            }    
        }
        buff++;
    }

    return true;
}


/// @brief returns if a stringslice is strictly an integer
/// valid integers are those that only have digits, and optionally a leading '-' sign.
/// @param ss stringslice to be evaluated
/// @return if stringslice is strictly an integer
bool isint(stringslice &ss)
{
    char *buff = ss.start;

    while (buff <= ss.end)
    {
        if (!((48 <= *buff) && (*buff <= 57)))
        {
            if (!((buff == ss.start) && ((*buff == '-'))))
            {
                return false;
            }    
        }
        buff++;
    }

    return true;
}

/// @brief returns if the stringslice represents a string literal (has a leading and ending '"')
bool isstringliteral(stringslice &ss)
{
    return ((*ss.start == '"') && (*ss.end == '"'));
}


/// @brief returns if the stringslice contains only alphanumerics. Alphanumerics are all lowercase and
/// uppercase alphabets, numbers and '_'.
/// @param ss stringslice to be evaluated
/// @return if the stringslice contains only alphanumerics
bool isonlyalpha(stringslice &ss)
{
    char *buff = ss.start;

    while (buff <= ss.end)
    {
        if (!(
            (('a' <= *buff) && (*buff <= 'z')) ||
            (('A' <= *buff) && (*buff <= 'Z')) ||
            ((('0' <= *buff) && (*buff <= '9')) && (buff != ss.start)) ||
            (*buff == '_') 
        )) return false;
        
        buff++;
    }

    return true;
}

/// @brief returns length of the string
size_t stringlen(char *str)
{
    size_t len = 0;
    while (*str++ != '\0') len++;
    return len;
}

/// @brief checks if the given stringlsice and string are the same
/// @param ss stringslice
/// @param str string (HAS to be null-terminated)
/// @return true if the two are the same, else false
bool equal(stringslice &ss, char *str)
{
    if (length(ss) > stringlen(str)) return false;

    char *buffer = ss.start;
    while (buffer <= ss.end)
    {
        if (*buffer != *str) return false;
        buffer++;
        str++;
    }
    return true;
}

/// @brief checks if the two stringslices are the same
/// @return true if the stringlsices are the same, else false
bool equal(stringslice &ssa, stringslice &ssb)
{
    if (length(ssa) != length(ssb)) return false;

    char *buffa = ssa.start, *buffb = ssb.start;

    while ((buffa <= ssa.end) && (buffb <= ssb.end))
    {
        if (*buffa != *buffb) return false;
        buffa++;
        buffb++;
    }
    return true;
}


/// @brief checks if the given stringslice and the string portion are the same
/// @param ss the stringslice to be checked with
/// @param start the start of the string portion
/// @param end the end of the string portion
/// @return true if the two strings are the same, else false
bool equal(stringslice &ss, char *start, char *end)
{
    stringslice newss = { start, end };
    return equal(ss, newss);
}


/// @brief checks if the given stringslice has only digits
/// @param ss stringslice to be evaluated
/// @return true if it contains ONLY digits, else false
bool isdigitsonly(stringslice &ss)
{
    if ((ss.start == NULL) || (ss.end == NULL)) return false;

    char *buffer = ss.start;
    while (buffer <= ss.end)
    {
        if (!(
            ('0' <= *buffer) &&
            (*buffer <= '9')
        )) return false;
        buffer++;
    }
    return true;
}