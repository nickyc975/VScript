vpath %.hpp inc
vpath %.cpp src src/compiler src/runtime src/tools src/types

CXX=g++
CXXFLAGS=-I inc -g -Wall -Wextra -Wno-write-strings

SRCS=vslexer.cpp vsparser.cpp gencode.cpp printers.cpp error.cpp File.cpp VSObject.cpp \
	 VSObjectList.cpp VSCodeObject.cpp VSValue.cpp VSCallStackFrame.cpp execute.cpp vs.cpp
OBJECTS=$(SRCS:.cpp=.o)

OUTPUT_DIR=build

VS=vs

%.o: %.cpp
	$(if $(shell ls | grep -w $(OUTPUT_DIR)), , $(shell mkdir $(OUTPUT_DIR)))
	$(CXX) $(CXXFLAGS) -c $< -o $(OUTPUT_DIR)/$@

vs: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(foreach obj, $(OBJECTS), $(OUTPUT_DIR)/$(obj)) -o $(OUTPUT_DIR)/vs

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

