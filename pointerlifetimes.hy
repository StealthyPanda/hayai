struct human {
	ui8 age;
	str name;
	ptr human parent1, parent2;
}


fun dangler(ptr human holder)
{
	//depth 3
	let guy = human {
		age = 30,
		name = "SomeGuyTm",
	};

	//depth <=2
	holder = &guy; // this assignment is invalid(outside pointer, local address)
}

fun set(ptr human a, ptr human b)
{
	//both unknown depths, at compiletime if depth(a) >= depth(b)
	//then this will be fine, else not.
	a = b; // valid assignment
}


fun factory()
{
	let me = human();
	ptr something = &me; //valid with everything
	
	return fun(ptr temp) {
		
		//depth of temp is unknown but something is local
		temp = something;
	
	};

}


fun main()
{
	ptr human a, b, c; //depth 1

	{//inside this scope, a human is instantiated.
		let me = human {
			age = 20,
			name = "StealthyPanda",
		}; // depth 2

		//me is not a valid var after scope ends, but it wasn't created using 
		//new or anything, so `a` does not clear memory at scope end.
		a = &me; //this assignment is invalid. (outside pointer, local address)
	
		//last case to solve. I think.
		//depth 2
		ptr localptr = &me; //valid assignment
		set(c, localptr); // valid function
	}


	//at this point `a` is a dangling pointer.

	//another case:
	dangler(b);

	//`b` is now dangling.

	//possible solution:
	//add an additional check during pointer assignment,
	//checking if the address being assigned is a local variable.
	//so basically don't allow an outsider pointer to be assigned 
	//a local address.
	//IMP: DO not allow clone for pointers in func params.
	//it will basically mess everything up, and isn't required anyway.
	//NOTE this is a compiler level check.
	//the only exception to this rule is if the
	//local variable is created using `new` or `malloc`.


	//case `c` still not solved.
	//`c` is a dangling pointer at this point.


	// OK final solution, using pointer depths
	// basically every pointer has an associated depth, which the compiler
	// can find statically at compile time. So, where ever there is a pointer
	// assignment, the compiler basically has to ensure the depths match up.
	// So, for an assignment of the type:
	
	// x = y;

	// where x is a pointer and y is a pointer or an address,
	// check if depth(x) >= depth(y)
	// This will work inside functions as well, basically everywhere I think.

	//depth of a pointer at global level is 0,
	//and everytime you entera scope, it increases by 1, 
	//and everytime you exit a scope, it decreases by 1.


	//none of this BS is required for heap pointers, 
	//which will take care of themselves at runtime.

	//the only thing im thinking is left now is how closures will work
	//with all this.


	let functor = fun(ptr temp) {
		
		//depth of temp is now dependent on where this function will be called
		//i think this can still be statically determined at compiletime.
		temp = a;
	};

	//TODO: figure out how tf closures work


	return 0;
}