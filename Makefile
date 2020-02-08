CXX=g++ -g -Wall

BIN_DIR=build/bin
OBJ_DIR=build/obj

SRC_CP_DIR=src/compiler
SRC_RT_DIR=src/runtime
SRC_CP_TY_DIR=$(SRC_CP_DIR)/types

VSC=vsc

vsc: File.o vslexer.o vsparser.o Value.o Token.o SymTable.o ASTNode.o printers.o vsc.o
	$(CXX) $(OBJ_DIR)/File.o $(OBJ_DIR)/Value.o $(OBJ_DIR)/Token.o $(OBJ_DIR)/SymTable.o \
	 $(OBJ_DIR)/ASTNode.o $(OBJ_DIR)/vslexer.o $(OBJ_DIR)/vsparser.o $(OBJ_DIR)/printers.o \
	 $(OBJ_DIR)/vsc.o -o $(BIN_DIR)/vsc

File.o: $(SRC_CP_TY_DIR)/File.cpp
	$(CXX) -c $(SRC_CP_TY_DIR)/File.cpp -o $(OBJ_DIR)/File.o

Token.o: $(SRC_CP_TY_DIR)/Token.cpp
	$(CXX) -c $(SRC_CP_TY_DIR)/Token.cpp -o $(OBJ_DIR)/Token.o

SymTable.o: $(SRC_CP_TY_DIR)/SymTable.cpp
	$(CXX) -c $(SRC_CP_TY_DIR)/SymTable.cpp -o $(OBJ_DIR)/SymTable.o

Value.o: $(SRC_CP_TY_DIR)/Value.cpp
	$(CXX) -c $(SRC_CP_TY_DIR)/Value.cpp -o $(OBJ_DIR)/Value.o

ASTNode.o: $(SRC_CP_TY_DIR)/ASTNode.cpp
	$(CXX) -c $(SRC_CP_TY_DIR)/ASTNode.cpp -o $(OBJ_DIR)/ASTNode.o

vslexer.o: $(SRC_CP_DIR)/vslexer.cpp
	$(CXX) -c $(SRC_CP_DIR)/vslexer.cpp -o $(OBJ_DIR)/vslexer.o

vsparser.o: $(SRC_CP_DIR)/vsparser.cpp
	$(CXX) -c $(SRC_CP_DIR)/vsparser.cpp -o $(OBJ_DIR)/vsparser.o

printers.o: $(SRC_CP_DIR)/printers.cpp
	$(CXX) -c $(SRC_CP_DIR)/printers.cpp -o $(OBJ_DIR)/printers.o

vsc.o: $(SRC_CP_DIR)/vsc.cpp
	$(CXX) -c $(SRC_CP_DIR)/vsc.cpp -o $(OBJ_DIR)/vsc.o

test: vsc
	$(BIN_DIR)/$(VSC) -l -p test/hello.vs

test-lex: vsc
	$(BIN_DIR)/$(VSC) -l test/hello.vs

test-parse: vsc
	$(BIN_DIR)/$(VSC) -p test/hello.vs

clean:
	rm $(OBJ_DIR)/* $(BIN_DIR)/*

