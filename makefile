TARGET = comp
OBJECTS = compile.o scanner.o driver.o fsa.o parser.o node.o semantics.o codeGen.o

$(TARGET): $(OBJECTS)
	g++ -g -o $(TARGET) $(OBJECTS)

compile.o: compile.cpp scanner.h lib.h token.h parser.h semantics.h node.h codeGen.h
	g++ -g -c compile.cpp

scanner.o: scanner.cpp scanner.h driver.h token.h
	g++ -g -c scanner.cpp

driver.o: driver.cpp driver.h fsa.h scanner.h token.h
	g++ -g -c driver.cpp

fsa.o: fsa.cpp fsa.h
	g++ -g -c fsa.cpp

parser.o: parser.cpp parser.h token.h scanner.h node.h
	g++ -g -c parser.cpp

node.o: node.cpp node.h token.h
	g++ -g -c node.cpp

semantics.o: semantics.cpp semantics.h node.h
	g++ -g -c semantics.cpp

codeGen.o: codeGen.cpp codeGen.h
	g++ -g -c codeGen.cpp

.PHONY: clean
clean:
	/bin/rm -f $(OBJECTS) $(TARGET) *.gch
