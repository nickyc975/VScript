var in = open(input("input file: "), "r");
var out = open(input("output file: "), "w");
out.write(in.read());
out.close();
in.close();
