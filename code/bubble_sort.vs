// implement bubble sort

func bubble_sort(array) {
    var n = array.len();
    for (var i = 0; i < n; i += 1) {
        for (var j = 0; j < n - 1; j += 1) {
            if (array[j] > array[j + 1]) {
                var temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

var array = [];
var a = int(input("input next: "));
while (a != -1) {
    array.append(a);
    a = int(input("input next: "));
}
bubble_sort(array);
print(array);