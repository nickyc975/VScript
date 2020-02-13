// test loop and ref count memory manage

func while_loop(n) {
    var i = 0;
    while (i < n) {
        i += 1;
    }
    return i;
}

print(while_loop(100000), "\n");