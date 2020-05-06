func sum(init, *args) {
    var res = init;
    for (var i = 0; i < args.len(); i += 1) {
        res += args[i];
    }
    return res;
}

print(sum(0));
print(sum(1, 2, 3));