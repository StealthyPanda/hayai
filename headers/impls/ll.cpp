#include "./../general/ll.h"


template <typename T>
llnode<T>::llnode()
{
    this->val = NULL;
    this->next = NULL;
    this->prev = NULL;
}

template <typename T>
llnode<T>::~llnode()
{
}

template <typename T>
llnode<T>::llnode(T *val)
{
    this->val = val;
    this->next = NULL;
    this->prev = NULL;
}


template <typename T>
ll<T>::ll()
{
    this->root = NULL;
    this->end = NULL;
    this->length = 0;
}


template <typename T>
bool ll<T>::isempty()
{
    return ((this->root == NULL) && (this->end == NULL));
}


template <typename T>
ll<T>* ll<T>::postpend(T *val)
{
    if (val == NULL) return this;

    llnode<T> *newnode = new llnode<T>(val);
    if (this->isempty())
    {
        this->root = newnode;
        this->end = newnode;
    }
    else
    {
        this->end->next = newnode;
        newnode->prev = this->end;
        this->end = newnode;
    }

    this->length++;

    return this;
}

template <typename T>
ll<T>* ll<T>::prepend(T *val)
{
    if (val == NULL) return this;

    llnode<T> *newnode = new llnode<T>(val);
    if (this->isempty())
    {
        this->root = newnode;
        this->end = newnode;
    }
    else
    {
        this->root->prev = newnode;
        newnode->next = this->root;
        this->root = newnode;
    }

    this->length++;

    return this;
}

template <typename T>
T* ll<T>::postpop()
{
    if (this->isempty()) return NULL;

    T* retter;

    if (this->root == this->end)
    {
        retter = this->root->val;
        delete this->root;
        this->root = NULL;
        this->end = NULL;
    }
    else
    {
        llnode<T> *rn = this->end;
        rn->prev->next = NULL;

        retter = rn->val;

        this->end = rn->prev;

        delete rn;
    }

    this->length--;

    return retter;
}

template <typename T>
T* ll<T>::prepop()
{
    if (this->isempty()) return NULL;
    
    T* retter;

    if (this->root == this->end)
    {
        retter = this->root->val;
        delete this->root;
        this->root = NULL;
        this->end = NULL;
    }
    else
    {
        llnode<T> *rn = this->root;
        rn->next->prev = NULL;

        retter = rn->val;

        this->root = rn->next;

        delete rn;
    }

    this->length--;

    return retter;
}

template <typename T>
ll<T>* ll<T>::append(ll<T> *other)
{
    llnode<T> *buffer = other->root;
    while (buffer != NULL)
    {
        this->postpend(buffer->val);
        buffer = buffer->next;
    }

    return this;
}

template <typename T>
ll<T>* ll<T>::prepend(ll<T> *other)
{
    llnode<T> *buffer = other->root;
    while (buffer != NULL)
    {
        this->prepend(buffer->val);
        buffer = buffer->next;
    }

    return this;
}

template <typename T>
ll<T>* ll<T>::insert(T* val, size_t index)
{
    llnode<T> *buffer = this->root;
    if (index <= (this->length - index))
    {
        while ((index --> 0) && (buffer != NULL)) buffer = buffer->next;
    }
    else
    {
        index = this->length - index - 1;
        buffer = this->end;
        while ((index --> 0) && buffer != NULL) buffer = buffer->prev;
    }

    if (buffer != NULL)
    {
        llnode<T> *newnode = new llnode<T>;
        newnode->next = buffer->next;
        newnode->prev = buffer;
        newnode->val = val;

        if (buffer->next != NULL) buffer->next->prev = newnode;
        buffer->next = newnode;

        this->length++;
    }
}

template <typename T>
T* ll<T>::remove(size_t index)
{
    llnode<T> *buffer = this->root;
    if (index <= (this->length - index))
    {
        while ((index --> 0) && (buffer != NULL)) buffer = buffer->next;
    }
    else
    {
        index = this->length - index - 1;
        buffer = this->end;
        while ((index --> 0) && buffer != NULL) buffer = buffer->prev;
    }

    if (buffer != NULL)
    {
        T* valueholder = buffer->val;

        if (buffer->next != NULL) buffer->next->prev = buffer->prev;
        if (buffer->prev != NULL) buffer->prev->next = buffer->next;

        if (buffer == this->root) this->root = this->root->next;
        if (buffer == this->end) this->end = this->end->prev;
        delete buffer;

        this->length--;
        return valueholder;
    }

    return NULL;
}

template <typename T>
bool ll<T>::remove(T *value)
{
    if (this->isempty()) return false;

    llnode<T> *buffer = this->root;
    while (buffer != NULL)
    {
        if (buffer->val == value)
        {
            if (buffer->next != NULL) buffer->next->prev = buffer->prev;
            if (buffer->prev != NULL) buffer->prev->next = buffer->next;

            if (buffer == this->root) this->root = buffer->next;
            if (buffer == this->end) this->end = buffer->prev;

            delete buffer;

            this->length--;

            return true;
        }
        else buffer = buffer->next;
    }

    return false;
}