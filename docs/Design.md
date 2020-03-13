# 设计思路

```
    var a, b, c;

    var f = func[a, b, c](d, e, f){

    };

    var cl = class[object] {
        var ID = 0;

        val __init__ = func[self](a, b, c) {
            self.a = a;
            self.b = b;
            self.c = c;
        }

        val CLSID = func[]() {
            var old = ID;
            ID += 1;
            return old;
        }
    };
```