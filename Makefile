CXX=g++

BIN_DIR=build/bin
OBJ_DIR=build/obj

SRC_CP_DIR=src/compiler
SRC_RT_DIR=src/runtime
SRC_UTL_DIR=src/utils

VSC=vsc

vsc: File.o vslexer.o vsc.o
	$(CXX) $(OBJ_DIR)/File.o $(OBJ_DIR)/vslexer.o $(OBJ_DIR)/vsc.o -o $(BIN_DIR)/vsc

File.o: $(SRC_UTL_DIR)/File.cpp
	$(CXX) -c $(SRC_UTL_DIR)/File.cpp -o $(OBJ_DIR)/File.o

vslexer.o: File.o $(SRC_CP_DIR)/vslexer.cpp
	$(CXX) -c $(SRC_CP_DIR)/vslexer.cpp -o $(OBJ_DIR)/vslexer.o

vsc.o: File.o vslexer.o $(SRC_CP_DIR)/vsc.cpp
	$(CXX) -c $(SRC_CP_DIR)/vsc.cpp -o $(OBJ_DIR)/vsc.o

test: vsc
	$(BIN_DIR)/$(VSC) -l -p test/hello.vs

test-lex: vsc
	$(BIN_DIR)/$(VSC) -l test/hello.vs

test-parse: vsc
	$(BIN_DIR)/$(VSC) -p test/hello.vs

clean:
	rm $(OBJ_DIR)/* $(BIN_DIR)/*

