vpath %.hpp inc
vpath %.cpp src src/compiler src/runtime src/tools src/objects

CXX=g++
CXXFLAGS=-I inc -g -Wall -Wextra -Wno-write-strings

SRCS=error.cpp VSBoolObject.cpp VSCharObject.cpp VSFloatObject.cpp VSIntObject.cpp \
	 VSMapObject.cpp VSNoneObject.cpp VSObject.cpp VSStringObject.cpp VSTypeObject.cpp \
	 VSTupleObject.cpp VSListObject.cpp VSSetObject.cpp VSCodeObject.cpp Symtable.cpp \
	 VSTokenizer.cpp VSParser.cpp VSCompiler.cpp printers.cpp vs.cpp

OBJECTS=$(SRCS:.cpp=.o)

OUTPUT_DIR=build

VS=vs

%.o: %.cpp
	$(if $(shell ls | grep -w $(OUTPUT_DIR)), , $(shell mkdir $(OUTPUT_DIR)))
	$(CXX) $(CXXFLAGS) -c $< -o $(OUTPUT_DIR)/$@

vs: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(foreach obj, $(OBJECTS), $(OUTPUT_DIR)/$(obj)) -o $(OUTPUT_DIR)/vs

objects:$(OBJECTS)

test: vs
	$(OUTPUT_DIR)/$(VS) -l -p -s test/hello.vs

test-lex: vs
	$(OUTPUT_DIR)/$(VS) -l test/hello.vs

test-parse: vs
	$(OUTPUT_DIR)/$(VS) -p test/hello.vs

test-gencode: vs
	$(OUTPUT_DIR)/$(VS) -s test/hello.vs

clean:
	rm -rf $(OUTPUT_DIR)/* *.o

