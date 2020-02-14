// implement bubble sort

func bubble_sort(array, len) {
    for (var i = 0; i < len; i += 1) {
        for (var j = 0; j < len - 1; j += 1) {
            if (array[j] > array[j + 1]) {
                var temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

var len = 0, array = [];
var a = int(input("input next: "));
while (a != -1) {
    len += 1;
    array += [a];
    a = int(input("input next: "));
}
bubble_sort(array, len);
print(array);