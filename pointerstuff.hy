
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

ptr[2] f32 aptr; //OK (internally, a single piece of memory is made on stack that is always empty)
				// that this aptr will point to. (see line 63 for more details)
ptr[2] f32 anotherone = &piptr; //OK
ptr onemore = &piptr + 1; //OK
ptr yetanuddaone = &piptr + 10000; //OK
ptr f32 piptrcopy = piptr; //OK
ptr piptrcopy2 = piptr - 69; //OK

ptr thiswontwork  = 69; //Big NO NO
ptr thiswontwork2 = 69 - 0; //Big NO NO
ptr thiswontwork3 = null; //Big NO NO

// axiom 2: pointer arithmetic is allowed, provided assignment rules as decribed
// above are allowed. (And ofc the pointer types match and everything)

onemore += 1; //OK
onemore += 100000; //OK
onemore -= 100000; //OK
onemore = yetanuddaone - 2; //OK

onemore = 2; //Big NO NO (simply constant)
onemore = piptr; //Big NO NO (types dont match)




//internally, a normal pointer is actually represented by 3 raw pointers in C.
struct smartpointer {
	void *left, *right, *value;
};

//whenever an assignment occurs, the following check is added in C:

onemore = yetanuddaone - 2;
//above line leads to the following bounds check (after making sure pointer types are the same):
auto temp = yetanuddaone;
if ((yetanuddaone.left < yetanuddaone - 2) && ((yetanuddaone - 2 + sizeof(f32)) < yetanuddaone.right))
	onemore.value = yetanuddaone - 2;
else if ((yetanuddaone.left > yetanuddaone - 2))
	onemore.value = yetanuddaone.left;
else if ((yetanuddaone.right < yetanuddaone - 2))
	onemore.value = yetanuddaone.right;
onemore.left = yetanuddaone.left;
onemore.right = yetanuddaone.right;



//about uninitialized normal pointers, basically you go through the entire program,
//check for the datatype with largest size, lets say X. In the C code, make a variable of X,
X _placeholder;
*add code here to set all bytes of _placeholder to 0*
//then, for any uninitialized pointers, just point them to _placeholder.


//to check if a normal pointer is initialized or not, you can use this check:
somepointer == null; // null is equivalent to &_placeholder.

//-------------------------------------------------------------
// these axioms ensure a normal pointer is always valid.
//-------------------------------------------------------------



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

//this makes all unsafe pointers at least easily debuggable.
//and you should not be using unsafe pointers anyway, which are simply here for 
//backwards compatibility with C.



//Finally, arrays are the same thing as pointers.

let rad = 3.14;

//So essentially all the following are the same:
ptr           bruh = &rad;
ptr[1] f32[1] bruh = &rad;

//to declare arrays of dimensions N with sizes x1, x2, ... xN along each of those dimensions:
ptr[N] datatype[x1][x2][x3]...[xN] name;