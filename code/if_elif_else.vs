func if_elif_else(n) {
    if (n < 0) {
        print(n, " is negative!\n");
    } elif (n == 0) {
        print(n, " is zero!\n");
    } else {
        print(n, " is positive!\n");
    }
}

if_elif_else(-1);
if_elif_else(0);
if_elif_else(1);