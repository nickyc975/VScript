// perform add on string to test memory management

var a = "", b = "a";
for (var i = 0; i < 500000; i += 1)
{
    a += b;
}
print(a.len());