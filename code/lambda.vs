val a = [1, 2, 3];

val b = lambda(f, l, n) {
    var r = [];
    for (var i = 0; i < n; i += 1) {
        r.append(f(l[i]));
    }
    return r;
};

b(lambda(i) {return i + 1;}, a, 3);
