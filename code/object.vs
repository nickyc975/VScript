// 定义一个构造Animal对象的函数
//　Animal对象有一个属性name和一个方法walk
func Animal(name) {
    var animal = object();
    setattr(animal, "name", name);

    func walk() {
        print(animal.name, "walking!");
    }
    setattr(animal, "walk", walk);

    return animal;
}

// 定义一个构造Dog对象的函数
// Dog继承自Animal，并且重写了Animal的walk方法
func Dog(name) {
    var dog = Animal(name);
    func walk() {
        print("Dog", dog.name, "walking!");
    }
    setattr(dog, "walk", walk);
    return dog;
}

// 定义一个构造Cat对象的函数
// Cat继承自Animal，并且重写了Animal的walk方法
func Cat(name) {
    var cat = Animal(name);
    func walk() {
        print("Cat", cat.name, "walking!");
    }
    setattr(cat, "walk", walk);
    return cat;
}

var dog = Dog("Snoopy");
var cat = Cat("Hello Kitty");
dog.walk();
cat.walk();