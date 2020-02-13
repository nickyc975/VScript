func main(argc, argv) {
    print("Hello World!\n");
    var x = argc, y = 5, z = argv;
    for (var i = 0; i < y; i += x) {
        print(i);
        z += z;
    }
    print(z);
    return 0;
}

func fib(n) {
    if (n > 2) { 
        return fib(n - 2) + fib(n - 1);
    } else {
        return n;
    }
}

val none_val = none;
// none_val += 10;
val bool_val = !true | false & false | !(3 < 4);
val char_val = 5 / (9 - (10 - 8) * (9 / 3));
val int_val = 92834087;
val float_val = 1.24;
// int_val = int_val + float_val;
val str_val = "hello" + " " + "world" + "!\n";
val empty_str = "";
var a1 = 1, a2 = int_val, a3, a4 = 4 * 8;
val array_val = [1, 2, 3, 4, a1 + a2 + a4];
var avl = [];
var test = array_val[a1 + a4 / 16];

// call main and print;

var a = 0;
while (a < 10)
{
    if (a < 3) {
        a += 1;
    } elif (a < 7) {
        a += 2;
    } else {
        break;
    }
}
print(a, "\n");
print(main(2, [1, 2, 3]), "\n");
print(fib(10), "\n");