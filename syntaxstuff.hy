
//compiler quirk for now:
fun print(str format, void value) {}

struct vector3
{
	f32 x, y, z;
	str name;
}






fun main()
{
	//no empty/null variables; everything is initialized to some 0 value always

	let position = vector3(
		x = -1.0,
		y = 2.2,
		z = 3.3,
		name = "position"
	);

	ptr ptopos = &position;

	ptr f32 newone = new f32;

	




	return 0;
}