
// OK so basically there are two type of pointers: normal ones and unsafe ones
//Normal pointers are declared like so:
let pi = 3.141592;
let radius = 32;

ptr f32 piptr = &pi; //type inferencing rules for pointers same as variables.
ptr radiusptr = &radius;


//so, axioms for pointers:
// axiom 1: pointers cannot be assigned any random value.
// pointers can only be assigned using an expression that has an addressof operator or
// another pointer of the same type.

ptr[2] f32 aptr; //OK
ptr[2] f32 anotherone = &piptr; //OK
ptr onemore = &piptr + 1; //OK
ptr yetanuddaone = &piptr + 10000; //OK
ptr f32 piptrcopy = piptr; //OK
ptr piptrcopy2 = piptr - 69; //OK

ptr thiswontwork  = 69; //Big NO NO
ptr thiswontwork2 = 69 - 0; //Big NO NO
ptr thiswontwork2 = null; //Big NO NO


// Unsafe pointers are literally just raw C pointers, and have literally no restrictions
// So, you can declare an unsafe poniter like so:
unsafe ptr f64 somepointer;
unsafe ptr byte bruh = null;
somepointer = 69;
//you get the point, you can do anything with them.
//the only restriction is that an unsafe pointer can be dereferenced only in an unsafe block:

*somepointer = 3.14; //invalid, will lead to error;

unsafe {
	*somepointer = 3.14; //this is ok
}

//and every unsafe block has automatic checks and stuff added in the generated C code.
//this makes all unsafe pointers at least easily debuggable.
//and you should not be using unsafe pointers anyway, which are simply here for 
//backwards compatibility with C.