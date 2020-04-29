func incr(n) {
    return n + 1;
}

func decr(n) {
    return n - 1;
}

func call_func(f, n) {
    return f(n);
}

print(call_func(incr, 2));
print(call_func(decr, 3));