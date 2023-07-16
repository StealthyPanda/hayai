#pragma once


/// @brief A Linked List node that stores a pointer of type T. So T *val;
// The Node also points to its previous and subsequent nodes via *prev and *next;
/// @tparam T -> The type of the value to be stored.
template <typename T>
class llnode
{
public:
    llnode();
    ~llnode();
    llnode(T *val);

    T *val;

    llnode<T> *prev, *next;
};

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

/// @brief Creates a linked list node and sets the pointer as provided
/// @tparam T 
/// @param val : Sets the value of this->val to val
template <typename T>
llnode<T>::llnode(T *val)
{
    this->val = val;
    this->next = NULL;
    this->prev = NULL;
}


/// @brief Creates a linked list that stores pointers of type T.
/// @tparam T 
template <typename T>
class ll
{
public:
    ll();

    llnode<T> *root, *end;
    size_t length;

    bool isempty();
    
    ll<T>* postpend(T *val);
    ll<T>* prepend(T *val);

    T* postpop();
    T* prepop();

    ll<T>* append(ll<T>* other);
    ll<T>* prepend(ll<T>* other);
    
    T* operator[] (size_t index)
    {
        if (this->isempty()) return NULL;

        llnode<T> *buffer = this->root;

        if ((this->length - index) < (index))
        {
            buffer = this->end;
            index = this->length - index - 1;
            while (index --> 0) buffer = buffer->prev;
        }
        else
        {
            while (index --> 0) buffer = buffer->next;
        }


        return buffer->val;        
    }

    ll<T>* insert(T* val, size_t index);
    T* remove(size_t index);
    bool remove(T *value);

};


template <typename T>
ll<T>::ll()
{
    this->root = NULL;
    this->end = NULL;
    this->length = 0;
}

/// @brief Returns whether the linked list is empty or not
/// @tparam T 
/// @return returns true if empty, else false
template <typename T>
bool ll<T>::isempty()
{
    return ((this->root == NULL) && (this->end == NULL));
}

/// @brief Appends a value to the end of the linked list
/// @tparam T 
/// @param val pointer to the value to be appended
/// @return a pointer to this object
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


/// @brief Appends a value to the beginning of the linked list
/// @tparam T 
/// @param val pointer to the value to be prepended
/// @return pointer to this object
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

/// @brief Removes the item at the end of the linked list
/// @tparam T 
/// @return Pointer stored in the removed node
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

/// @brief Removes an item from the beginning of the linked list
/// @tparam T 
/// @return the pointer stored at the end of the linked list
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


/// @brief Appends an entire linked list to the end of the linked list
/// @tparam T 
/// @param other pointer to the other linked list to be appended (must be of the same type as this one)
/// @return pointer to this object
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

/// @brief appends an entire linked list to the beginning of the linked list
/// @tparam T 
/// @param other pointer to the other linked list to be appended (must be of the same type as this one)
/// @return a pointer to this object
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

/// @brief inserts an item in the linked list
/// @tparam T 
/// @param val the value to be inserted
/// @param index the index to be inserted at (the item is inserted between node and node + 1 indices)
/// @return pointer to this
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


/// @brief Removes an item from the linked list
/// @tparam T 
/// @param index the index at which item is to be removed (item is removed at the indexth position)
/// @return the value stored at the node removed
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


/// @brief removes the node containing the given pointer value
/// @tparam T 
/// @param value 
/// @return true if the node was found (and removed), else false
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