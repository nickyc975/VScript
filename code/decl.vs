val a = [1, 2, 3];
val b = (1, 2, 3);
val c = {"1": 1, "2": 2, "3": 3};
val d = {1, 2, 3};
val e = lambda (a, b, f) {
    var res = a + b + f;
    c["res"] = res;
};

e(1, 2, 3);
print(c);