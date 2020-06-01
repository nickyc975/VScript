# VScript使用说明

### 变量常量声明与定义

VScript使用`var`和`val`关键字声明和定义变量和常量。

VScript中有六种基本数据类型，可使用如下方式声明相应的变量并赋值：

```
    var a = none, b = true, c = '1';
    var d = 1, d = 1.0, e = "1";
```

VScript中还有四种容器数据类型，同样可以用于给变量赋值：

```
    // 声明元组
    var a = (1, 2, 3);
    // 声明列表
    var b = [1, 2, 3];
    // 声明集合
    var c = {1, 2, 3};
    //　声明字典
    var d = {"1": 1, "2": 2, "3": 3};
```

VScript支持基本的文本IO，所以你可以使用`open`函数打开文本文件并赋值给变量：

```
    var file = open("/path/to/text/file.txt", "r");
```

VScript还支持函数式编程，所以你还可以将匿名函数赋值给变量：

```
    var inc_one = lambda (n) {return n + 1;};
```

你也可以直接使用`func`关键字声明并定义具名函数，例如：

```
    func inc_one(n) {
        return n + 1;
    }
```

将上述示例代码中的`var`替换成`val`就是VScript中声明并定义常量的方法。注意，变量声明时可以不赋值，但常量声明时必须赋值。

### 流程控制结构

VScript中有三种流程控制结构：`if-elif-else`，`while`和`for`。

`if-elif-else`结构用于实现条件分支，例如如下代码：

```
    var a = int(input("input a: "));
    if (a > 0) {
        print("a is positive!");
    } elif (a == 0) {
        print("a is zero!");
    } else {
        print("a is negative!");
    }
```

实现了根据输入的数字是正、负或零来输出相应的信息。

`while`和`for`结构用于实现循环，它们的用法跟许多主流语言，如C++, Java等相同，即：

```
    var a = 0;
    while (a < 100) {
        a += 1;
    }

    for (var i = 0; i < a; i += 1) {
        print(i);
    }
```

### 类型转换

VScript是强类型的，这意味着当你将整数与浮点数相加时，需要先统一它们的类型，例如：

```
    var a = 1, b = 1.0;
    var c = a + int(b);
```

VScript提供了一系列进行这种类型转换的函数，关于它们的详细介绍请查阅VScript文档。

### 使用函数

在变量声明与定义中已经简单介绍过了函数的两种定义方式，但需要补充的一点是，在VScript中支持可变参数个数的函数，其使用方法如下：

```
    func sum(init, *args) {
        var res = init;
        for (var i = 0; i < args.len(); i += 1) {
            res += args[i];
        }
        return res;
    }

    print(sum(0));
    print(sum(1, 2, 3));
```

其中，可变参数元组的名字`args`是可以任意指定的。

之前说过VScript是支持函数式编程的，也就意味着VScript中的函数像普通数据类型一样可以动态生成，可以用于赋值，可以作为参数也可以作为返回值，可以用于赋值前文已经介绍过了，这里介绍一下函数动态生成、作为参数以及返回值：

```
    // 返回一个将参数加上n的函数
    func inc_n_func(n) {
        func inc_n(v) {
            return v + n;
        }
        return inc_n;
    }

    // 接收一个列表和一个函数，对列表中的每一个元素调用接收的函数
    func map(lst, f) {
        var res = [];
        for (var i = 0; i < lst.len(); i += 1) {
            var mapped = f(lst[i]);
            res.append(mapped);
        }
        return res;
    }

    //　生成一个将参数加一的函数
    var inc_one = inc_n_func(1);

    // 将列表的每一项加一
    print(map([1, 2, 3], inc_one));
```

### 面向对象编程

VScript部分支持面向对象编程范式，你可以使用`object`函数创建一个没有任何属性和方法的空对象，然后使用`hasattr`、`getattr`、`setattr`和`removeattr`对对象进行修饰，即可获得想要的对象。例如：

```
    // 定义一个构造Person对象的函数
    // Person对象拥有两个属性：name和age，以及两个方法：getName和getAge，此外它还重写了父类的__str__方法
    func Person(name, age) {
        var person = object();

        func getName() {
            return person.name;
        }

        func getAge() {
            return person.age;
        }

        func __str__() {
            return person.name + " " + str(person.age);
        }

        setattr(person, "name", name);
        setattr(person, "age", age);
        setattr(person, "getName", getName);
        setattr(person, "getAge", getAge);
        setattr(person, "__str__", __str__);

        return person;
    }

    // 构造一个Person对象并调用它的方法
    var person = Person("Nicky", 22);
    print(person);
```

### 输入输出

VScript支持基本的文本IO，你可以使用`print`和`input`直接对标准输入输出文件进行操作，如：

```
    // 打印"input a: "作为提示并从标准输入中读取一行
    var a = input("input a: ");

    // 向标准输出打印一系列字符串
    print("you input:", a);
```

如果你想操作其他的文本文件，可以使用`open`函数打开文件，然后调用文件对象的相应方法进行读写：

```
    var a = open("/path/to/text/file.txt", "w");
    a.writeline("Hello VScript!");
    a.close();
```
