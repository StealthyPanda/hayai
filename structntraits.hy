//Ok so about structs and traits

//no classes and all that nonsense.
//there are only structs and traits. Yes I yoinked basically
//the exact same thing from rust.

//struct syntax that we all know and love

struct coords {
	f32 x, y;
	str name;
}

//notice no ending ; required when defining structs; already better than C.

//instantiating structs:
//basically the same as normal vars:

let position = coords(0, 0, "init position"); //here all fields need to be filled in
//positional arguments are inferred to the corresponding fields in struct.
let position = coords(); //also valid as all fields always have default 0 values
let position = coords {
	name = "curr position",
	x = 22,
}; //also valid





//Now, defining methods on structs is the same as defining a normal function,
//but with a param of that struct type at the beginning. So:

fun move(coords position, f32 dx, f32 dy) {
	position.x += dx;
	position.y += dy;
	return position;
}
// move is now a method on coords struct

position.move(1, 2); //valid
move(position, 1, 2); //also correct

position.move(1, 2).move(3, 4); // chaining possible cuz coords is
							 //returned be ref (cuz it is a param)

//remember all params are pass by reference by default for functions.

//So, traits are basically a set of method signatures.
// For eg, let human and robot be two structs:

struct human {
	ui8 age;
	str firstname, lastname;
	f32 height, weight;
}

struct robot {
	ui16 id;
	f32 batterypercent;
}


//traits simply contain function signatures (in full, no type inferencing here)
trait talkers {
	//returns a greeting string
	fun getgreeting(talkers t) : str {}
	
	//directly greets to std output
	fun greet(talkers t) : void {}
}

//to implement traits, you have to use impl blocks (yes exactly as rust)
impl talkers for human
{
	fun getgreeting(human h) {
		("👋Hello, my name is " + h.firstname + " " + h.lastname + "!")
	}

	fun greet(human h) {
		print(h.getgreeting());
	}
}

impl talkers for robot
{
	fun getgreeting(robot r) {
		("🤖beep boop, I am " + (r.id as str) + "!")
	}

	fun greet(robot r) {
		print(r.getgreeting());
	}
}

//impl blocks must implement all the function signatures
//defined in the trait block. one impl block can implement multiple traits too.
//now both human and robot are talkers.

let touseef = human {
	age = 20,
	firstname = "Stealthy",
	lastname = "Panda",
};

let r2d2 = robot {
	id = 69,
	batterypercent = 100,
};

touseef.greet();

print(r2d2.getgreeting());


//Now, traits can be used exactly like a type in function params. So:

fun say5times(talkers t)
{
	let i = 0;
	while i < 5 {
		t.greet();
		i += 1;
	}
}

say5times(touseef);
touseef.say5times();

//is now possible.

//todo: figure out C implementation of these traits
//also todo: figure out enums.

// pointer fields are specified using pointer.
// so a linked list node would be

struct node {

	i32 value;
	ptr node next;

}