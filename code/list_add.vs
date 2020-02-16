// perform add on list to test memory management

var a = [], b = [1];
for (var i = 0; i < 20000; i += 1)
{
    a += b;
}
print(len(a), "\n");