val a = (1, 2, 3);

val map = lambda(f, l) {
    var r = [];
    for (var i = 0; i < l.len(); i += 1) {
        r.append(f(l[i]));
    }
    return r;
};

var r = map(lambda(i) {return i + 1;}, a);
print(r);
