
target:
	gcc -g -Wall -o es01a es01a.c 
	gcc -g -Wall -o es01b es01b.c 
	gcc -g -Wall -o es02 es02.c -lpthread
	gcc -g -Wall -o mkarrays mkarrays.c 
	gcc -g -Wall -o es03_seq es03_seq.c
	gcc -g -Wall -o es03A es03A.c -lpthread
	gcc -g -Wall -o es03B es03B.c -lpthread
	

generate_file:
	mkdir tmp
	./mkarrays 5 tmp


clean:
	rm -rf es01a es01b es02 es03_seq es03A es03B fifo tmp

