// Define a Person object constructor.
// The constructor creates an object and binds Person attributes to it.
func Person(name, age) {
    var person = object();

    func getName() {
        return person.name;
    }

    func getAge() {
        return person.age;
    }

    setattr(person, "name", name);
    setattr(person, "age", age);
    setattr(person, "getName", getName);
    setattr(person, "getAge", getAge);

    return person;
}

// Construct a Person object.
var person = Person("Nicky", 22);

// Call methods.
print("name:", person.getName());
print("age:", person.getAge());
