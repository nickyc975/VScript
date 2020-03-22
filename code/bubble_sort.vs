// implement bubble sort

// Begin temporary built-in definitions.
// These are temporary definitions for built-in names, added to pass the compilation.
var len = lambda(cont) {
    return 0;
};

var int = lambda(lit) {
    return 0;
};

var input = lambda(msg) {
    return "0";
};

var append = lambda(cont, v) {
    return none;
};

var print = lambda(a, b, c) {
    return none;
};
// End temporary built-in definitions.

func bubble_sort(array) {
    var n = len(array);
    for (var i = 0; i < n; i += 1) {
        for (var j = 0; j < n - 1; j += 1) {
            if (array[j] > array[j + 1]) {
                var temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

var array = [];
var a = int(input("input next: "));
while (a != -1) {
    append(array, a);
    a = int(input("input next: "));
}
bubble_sort(array);
print(array, "\n");