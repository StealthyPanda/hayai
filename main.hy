 
//leave these here for now; i havent yet dealt with C headers yet.
fun print(str format, void value) : void {}
//fun print(str format, str value) {}

//C:\Users\touse\AppData\Roaming\Sublime Text\Packages\User
fun nthfib(i32 n)
{
    let a = 1;
    let b = 1;
    let c = a + b;

    if n < 2 {
        return 1;
    }
    elif n == 3 {
        return 2;
    }

    n = n - 3;

    while n > 0 {
        a = b;
        b = c;
        c = a + b;

        n = n - 1;
    }

    return c;
}

fun main() {
    let f64 i = 0.0;

    ptr pointer = new i32;

    while i < 10 {
        
        // *(pointer + i);

        print("%d ", i);
        print("%s\n", "Hajimemashite");
    
        i = i + 1;
    }

    print("%d\n", nthfib(1));
    print("%d\n", nthfib(2));
    print("%d\n", nthfib(3));
    print("%d\n", nthfib(4));
    print("%d\n", nthfib(5));
    print("%d\n", nthfib(6));


    return 0;
}