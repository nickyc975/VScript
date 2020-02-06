CXX=g++ -g -Wall

BIN_DIR=build/bin
OBJ_DIR=build/obj

SRC_CP_DIR=src/compiler
SRC_RT_DIR=src/runtime
SRC_UTL_DIR=src/utils

VSC=vsc

vsc: File.o vslexer.o vsparser.o SymTable.o printers.o vsc.o
	$(CXX) $(OBJ_DIR)/File.o $(OBJ_DIR)/vslexer.o $(OBJ_DIR)/vsparser.o $(OBJ_DIR)/SymTable.o \
	 $(OBJ_DIR)/printers.o $(OBJ_DIR)/vsc.o -o $(BIN_DIR)/vsc

File.o: $(SRC_UTL_DIR)/File.cpp
	$(CXX) -c $(SRC_UTL_DIR)/File.cpp -o $(OBJ_DIR)/File.o

vslexer.o: File.o $(SRC_CP_DIR)/vslexer.cpp
	$(CXX) -c $(SRC_CP_DIR)/vslexer.cpp -o $(OBJ_DIR)/vslexer.o

vsparser.o: $(SRC_CP_DIR)/vsparser.cpp
	$(CXX) -c $(SRC_CP_DIR)/vsparser.cpp -o $(OBJ_DIR)/vsparser.o

printers.o: vsparser.o $(SRC_CP_DIR)/printers.cpp
	$(CXX) -c $(SRC_CP_DIR)/printers.cpp -o $(OBJ_DIR)/printers.o

SymTable.o: $(SRC_CP_DIR)/SymTable.cpp
	$(CXX) -c $(SRC_CP_DIR)/SymTable.cpp -o $(OBJ_DIR)/SymTable.o

vsc.o: File.o vslexer.o vsparser.o SymTable.o printers.o $(SRC_CP_DIR)/vsc.cpp
	$(CXX) -c $(SRC_CP_DIR)/vsc.cpp -o $(OBJ_DIR)/vsc.o

test: vsc
	$(BIN_DIR)/$(VSC) -l -p test/hello.vs

test-lex: vsc
	$(BIN_DIR)/$(VSC) -l test/hello.vs

test-parse: vsc
	$(BIN_DIR)/$(VSC) -p test/hello.vs

clean:
	rm $(OBJ_DIR)/* $(BIN_DIR)/*

