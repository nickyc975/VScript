// test nested list.

var a = [1, [2, 3, [4, 5]], 6];
print(a[0], "\n");
print(a[1], "\n");
print(a[1][0], "\n");
print(a[1][2], "\n");
print(a[1][2][0], "\n");

a[1][2] = 0;

print(a[1][2], "\n");

a[1] = 3;

print(a);