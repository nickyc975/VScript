// test nested list.

var a = [1, [2, 3, [4, 5]], 6];
print(a[0]);
print(a[1]);
print(a[1][0]);
print(a[1][2]);
print(a[1][2][0]);

a[1][2] = 0;

print(a[1][2]);

a[1] = 3;

print(a);