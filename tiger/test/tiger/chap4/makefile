# a.out: parsetest.o y.tab.o lex.yy.o errormsg.o util.o
	# cc -g parsetest.o y.tab.o lex.yy.o errormsg.o util.o

# parsetest.o: parsetest.c errormsg.h util.h
	# cc -g -c parsetest.c

# y.tab.o: y.tab.c
	# cc -g -c y.tab.c

# y.tab.c: tiger.grm
	# yacc -dv tiger.grm

# y.tab.h: y.tab.c
	# echo "y.tab.h was created at the same time as y.tab.c"



# lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	# cc -g -c lex.yy.c

#lex.yy.c: tiger.lex
#	lex tiger.lex

a.out: absyn.o x86frame.o semant.o symbol.o table.o util.o testCase.o env.o types.o errormsg.o translate.o temp.o main.c y.tab.o lex.yy.o parse.o
	cc -g absyn.o x86frame.o semant.o symbol.o table.o util.o testCase.o env.o types.o errormsg.o translate.o temp.o main.c y.tab.o lex.yy.o parse.o

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: tiger.grm
	yacc -dv tiger.grm

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	flex tiger.lex

absyn.o: absyn.c absyn.h symbol.h util.h
	cc -g -c absyn.c

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

table.o: table.c table.h util.h
	cc -g -c table.c

x86frame.o: ../chap6/x86frame.c 
	cc -g -c ../chap6/x86frame.c

translate.o: ../chap6/translate.c
	cc -g -c ../chap6/translate.c

temp.o: ../chap6/temp.c
	cc -g -c ../chap6/temp.c

util.o: util.c util.h
	cc -g -c util.c
	
testCase.o: symbol.o
	cc -g -c ../../testCase.c symbol.o

symbol.o: symbol.c table.h util.h
	cc -g -c symbol.c

types.o: ../chap5/types.c util.o symbol.o
	cc -g -c ../chap5/types.c

semant.o: semant.c
	cc -g -c semant.c

parse.o: parse.c
	cc -g -c parse.c

env.o: env.c
	cc -g -c env.c

clean: 
	rm -f a.out *.o
