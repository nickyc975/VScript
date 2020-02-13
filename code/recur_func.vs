// test recursive function

func fib(n) {
    if (n > 2) {
        return fib(n - 1) + fib(n - 2);
    } else {
        return n;
    }
}

print(fib(10), "\n");