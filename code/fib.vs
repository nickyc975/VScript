// test loop and ref count memory manage

func fib(n) {
    var i = 0;
    while (i < n) {
        i += 1;
    }
    return i;
}

print(fib(100000000));