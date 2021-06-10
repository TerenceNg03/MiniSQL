CC = g++
FLAGS = -IException -IBuffer_Manager -ICatalog_Manager -IIndex_Manager -IIndex_Manager -IInterpreter -IRecord_Manager -IOBJ -Wall -std=c++14 -g
CFLAGS = -IException -IBuffer_Manager -ICatalog_Manager -IIndex_Manager -IIndex_Manager -IInterpreter -IRecord_Manager -Wall -g

All:buf rec exc cat ind int lex
	$(CC) $(FLAGS) -c main.cpp -o OBJ/main.o
	$(CC) OBJ/*.o -o a.out
	[ -d DB_Data ] || mkdir DB_Data
buf: dir
	$(CC) $(FLAGS) -c Buffer_Manager/Buffer_Manager.cpp -o OBJ/buf.o
rec: dir
	$(CC) $(FLAGS) -c Record_Manager/Record_Manager.cpp -o OBJ/rec.o
exc: dir
	$(CC) $(FLAGS) -c Exception/Exception.cpp -o OBJ/exc.o
cat: dir
	$(CC) $(FLAGS) -c Catalog_Manager/Catalog_Manager.cpp -o OBJ/cat.o
ind: dir
	$(CC) $(FLAGS) -c Index_Manager/B_Plus_Tree.cpp -o OBJ/bpt.o
int: dir
	$(CC) $(FLAGS) -c Interpreter/C_warpper.cpp -o OBJ/int_cw.o
	$(CC) $(FLAGS) -c Interpreter/Interpreter.cpp -o OBJ/int_int.o

lex: dir
	yacc -d -Wno-yacc ./Interpreter/parser.y -o OBJ/y.tab.c
	lex -o OBJ/lex.yy.c ./Interpreter/token.l 
	cc $(CFLAGS) -c OBJ/y.tab.c  -o OBJ/y.tab.o
	cc $(CFLAGS) -c OBJ/lex.yy.c  -o OBJ/lex.o


dir:
	[ -d OBJ ] || mkdir OBJ
clean:
	[ ! -d OBJ ] || rm -rf OBJ/
	[ ! -d DB_Data ] || rm -rf DB_Data/
	[ ! -e a.out ] || rm a.out
