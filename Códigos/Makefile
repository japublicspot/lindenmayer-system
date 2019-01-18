CC = gcc
CFLAGS = -Wall -Wextra -Wconversion -pedantic  
OBJ = lsystem.o GenericList/GenericList.o VariableTree/VariableTree.o ParserHandler/ParserHandler.o TreeHandler/TreeHandler.o Stack/Stack.o psinterface/psinterface.o
LIBS = -lm

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 
	
lsystem: $(OBJ)
	$(CC) -o $@ $^ $(LIBS) 
	$(MAKE) clean
	
clean:
	rm -f *.o
	rm -f GenericList/*.o
	rm -f VariableTree/*.o
	rm -f ParserHandler/*.o
	rm -f TreeHandler/*.o
	rm -f Stack/*.o
	rm -f psinterface/*.o
