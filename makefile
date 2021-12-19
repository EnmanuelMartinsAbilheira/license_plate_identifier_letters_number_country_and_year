make: src/matriculas.c
	mkdir -p output
	gcc -o matriculas.out src/matriculas.c -lncurses -Wall

run: src/matriculas.c
	mkdir -p output
	gcc -o matriculas.out src/matriculas.c -lncurses -Wall
	matriculas.out

clean: 
	rm -rf output matriculas.out