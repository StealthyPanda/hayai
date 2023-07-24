#pragma once

#include "basics.h"
#include "strings.h"
#include "stderrs.h"
#include <iostream>


/// @brief represents an error. contains an errorcode, and an errormessage
class error
{
public:

    byte errorcode = 0;
    string errormessage;

    error()
    {
        this->errormessage = getstring_nt((char*) "");
    }

    error(byte errorcode)
    {
        this->errorcode = errorcode;
        this->errormessage = getstring_nt((char*) "");
    }

    error(byte errorcode, string message)
    {
        this->errorcode = errorcode;
        this->errormessage = message;
    }

    error(byte errorcode, char* message)
    {
        this->errorcode = errorcode;
        this->errormessage = getstring_nt(message);
    }

    void print()
    {
        std::cout << "\n[Error#" << (int)this->errorcode << "]:\n";
        ::print(&this->errormessage);
        std::cout << "\n";
    }
};


/// @brief Similar to options in rust. Basically it will either be 'ok' and have a value of type T, or
/// not 'ok' and contain an error with additional info.
template <typename T>
class result
{
public:
    T *value;
    error err;
    bool ok;

    result();
    result(T *value);
    result(error err);
    result(byte code, char *emsg);
    result(byte code, string emsg);
    ~result();

    void print();
};

template <typename T>
result<T>::~result()
{
}

template <typename T>
result<T>::result()
{
    this->value = NULL;
    this->ok = false;
}

template <typename T>
result<T>::result(T *value)
{
    this->value = value;
    this->ok = true;
}

template <typename T>
result<T>::result(error err)
{
    this->err = err;
    this->ok = false;
}

template <typename T>
result<T>::result(byte code, char *emsg)
{
    this->err = error(code, emsg);
    this->ok = false;
}

template <typename T>
result<T>::result(byte code, string emsg)
{
    this->err = error(code, emsg);
    this->ok = false;
}

template <typename T>
void result<T>::print()
{
    if (this->ok) std::cout << "Result OK";
    else
    {
        std::cout << "Result Bad: ";
        this->err.print();
    }
}