# 设计思路

### 基本设定

* 带有部分面向对象特性的函数式编程语言

* 部分面向对象特性包括：
  
  + 对象构造

```
    func Test(a, b, c) {
        var obj = Object();

        setattr(obj, "a", a);
        setattr(obj, "b", b);
        setattr(obj, "c", c);

        func inc_a() {
            obj.a += 1;
        }
        setattr(obj, "inc_a", inc_a);

        return obj;
    }

    var test = Test(1, 2, 3);
```