func if_elif_else(n) {
    if (n < 0) {
        print(n, " is negative!");
    } elif (n == 0) {
        print(n, " is zero!");
    } else {
        print(n, " is positive!");
    }
}

if_elif_else(-1);
if_elif_else(0);
if_elif_else(1);