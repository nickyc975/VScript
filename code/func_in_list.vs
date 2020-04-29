// test function in list

func f(n) {
    return n + 1;
}

var a = [1, 2, 3, f];

print(a[0]);
print(a[3](a[2]));