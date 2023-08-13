# hayai💨

A low level systems programming language with a focus on ease of use by the programmer.

<i>This is still a WIP, and the nebulus of features is still far from being fully formed.</i>

Hayai is a strongly-typed, functional, imperative programming language that can interop with C code seamlessly. Hayai gives you low level control with strong memory safety rules.
Syntactically, hayai aims to be a clearer and concise version of C. The language draws inspiration from rust, C++ and elixir languages.

## Syntax

The syntax is C-like, but more concise and explicit.
- Whitespace is ignored.
- Statements end with semi-colons.
- Scopes are bound with braces.
- Single line comments start with `//`.

### Variables
Variables can be declared using `let` keyword, optionally followed by the type of the variable. Type inferencing can deduce variable types if possible.

```rust
let u8 age = 69; //valid

let id = 69; //type inferred (integers default to i32)

let f32 radius; //valid; all uninitialized variables are set to a default `0` value.
//radius = 0.0

let bool flag; // flag = false

let name; //invalid; type cannot be inferred.

```

### functions

Functions are first class citizens, and supports anonymous functions. Functions are declared with the `fun` keyword, the identifier, the parameters list, followed by the return type.

```rust
//param list must contain param type
fun square(i32 number) : u32 {
    return number * number;
}
//note return type is optional
```

Functions are first class citizens:

```rust
//another way to define functions
let ((i32, i32) : i32) combine = fun(i32 x, i32 y) {
    let inter = (x + y);
    inter *= y;
    return inter;
};
```


Note type inferencing allows us to skip specifying the function type while declaring the variable,
which is `(i32, i32) : i32`. Also, any expression at the end of a scope is automatically returned. So, the above definition can be stripped down to:


```rust
//another way to define functions
let combine = fun(i32 x, i32 y) {
    let inter = (x + y);
    inter * y
};
```

The inline syntax is specific to defining functions that return a value:

```rust
let sum = (i32 x, i32 y) => x + y;
```


Function calls:

```rust
let s = sum(3, 5); //8

let combination = combine(11, 10); //210
```


Functions can be passed as arguments to other functions:

```rust

fun showcombination((i32, i32) : i32 combiner, i32 x, i32 y)
{
    let c = combiner(x, y);
    print("The combination of ", x, " and ", y, " is ", c);
}

let fancysum = (i32 x, i32 y) => 2 * (x + y);

showcombination(fancysum, 5, 6); // The combination of 5 and 6 is 22

```


### pointers

Hayai supports pointers and pointer arithmetic, with additional statically-verifiable rules for pointer manipulation that make them as safe as possible.

Pointers are declared with the `ptr` keyword, and the most general syntax for pointers is:
```rust
ptr[N] data_type[x0][x1]...[xN-1] identifier = value;
```

where `N` is the dimension of the pointer, `xi` is the size along the `i`th dimension.
These can be omitted wherever the type can be inferenced.


While any positive integer is a valid pointer value, in Hayai, the value assigned to a pointer MUST be in relation to the address of a variable. Examples:

```rust
let radius = 22;
let pi = 3.14;

//omitting pointer dimension defaults to ptr[1]

ptr i32 radius_ptr; //valid; more on null pointers later.
ptr i32 radius_ptr = null; //invalid; assignment cant use null.

ptr i32 radius_ptr = &radius; //valid
radius_ptr = &radius + 1; //valid
radius_ptr = &radius + 1000; //valid
radius_ptr += 69; //valid

ptr i32 radius_ptr = 69; //NOT valid (rvalue must have & in expression)
ptr f32 radius_ptr = &radius; //NOT valid (pointer type and variable type don't match)
ptr i32[5] radius_ptr = &radius; //NOT valid (pointer type and variable type don't match)

```

Pointer arithmetic is possible just as in C. Under the hood, runtime checks ensure the actual value of the pointer never goes outside the assigned memory block. So, in the block above, `&radius + 1` or `&radius + 1000` both end up having the same effect as `&radius`, as radius is a single memory block of 4 bytes corresponding to `i32`.

Arrays are the same thing as pointers, declared using the same syntax as pointers.

```rust

//2d array of size 4x5, with all values being initialized to 0.0
ptr[2] f32[4][5] matrix(0.0); 

```

Pointers are dereferenced using the `[]` operator:
```rust

ptr radius_ptr = &radius;

//empty brackets are the same as [0]
radius_ptr[] = 30;

radius_ptr[5] = 30; //compile time error, as the index is out of bounds of the pointer.

```

Pointers can be taken as function arguments by simply specifying `ptr` keyword in argument list:

```rust
fun swap(ptr i32 x, ptr i32 y) {
    let temp = x[];
    x[] = y[];
    y[] = temp;
}
```


### `structs` and `traits`

Structs are basically the same as in C:

```rust
struct human {
    u8 age;
    str name;
    ptr human p1, p2;
}
```

To make variables of structs:

```rust
let mom = human {
    age = 50,
    name = "Mother panda",
};

let dad = human {
    age = 50,
    name = "Father panda",
};

let panda = human(20, "Panda", &mom, &dad);

```

Any function that takes a pointer to a type as its first argument is a method on that type.
So:

```rust

fun greet(ptr human h, str dialogue)
{
    print("Hi, I'm ", h[].name, "! ");
    print(dialogue);
}

panda.greet("Nice to meet you!"); //Hi, I'm Panda! Nice to meet you!
//equivalent to:
greet(&panda, "Nice to meet you!");
```


Traits are similar to the concept in rust, and similar to interface in Go, which allows for polymorphic code. A trait is simply a set of method signatures. Any struct that implements a trait must define explicit definitions for each of these functions, which is done inside an `impls` (implements) block:

```rust

struct human {
    str name;
    u8 age;
}

struct robot {
    str id;
    u8 batterypercent;
}

trait greeter {
    fun greet(ptr greeter g) {}
}

human impls greeter {
    fun greet(ptr human h) {
        print("Hi, I'm ", h[].name, "!");
    }
}

robot impls greeter {
    fun greet(ptr robot r) {
        print("Beep boop, I'm", r[].id, ", and I'm at ", r[].batterypercent, "%!");
    }
}

let panda = human("panda", 20);
let r2d2  = robot("R2D2", 69);

panda.greet(); //works
r2d2.greet(); //works

```

Traits can be used in place of types in function params:

```rust

fun saydialogue(greeter g, str dialogue)
{
    g.greet();
    print(dialogue);
}

saydialogue(panda, "Pleasure to meet you!");
saydialogue(r2d2, "Pleasure to meet you!");
```

## General

Hayai is meant to be a layer over C, removing the tedious parts and retaining as much control as possible while ensuring memory safety. To interop with C, Hayai compiles to C, which is then compiled by a C compiler to machine code. As of yet, the compiler is still a WIP, and aims to deal with C code with a custom parser.