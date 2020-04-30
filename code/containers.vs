// test containers

val tupleobj = (1, 2, 3, 3);
val listobj = [1, 2, 3, 3];
val setobj = {1, 2, 3};

print("list", listobj);
print("list to list", list(listobj));
print("list to tuple:", tuple(listobj));
print("list to set:", set(listobj));
print();
print("set", setobj);
print("set to set:", set(setobj));
print("set to tuple:", tuple(setobj));
print("set to list:", list(setobj));
print();
print("tuple", tupleobj);
print("tuple to tuple:", tuple(tupleobj));
print("tuple to list:", list(tupleobj));
print("tuple to set:", set(tupleobj));