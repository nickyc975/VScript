// test loop and ref count memory management

func for_loop(n) {
    for (var i = 0; i < n; i += 1) {
        i += 1;
    }
    return n;
}

print(for_loop(10000));