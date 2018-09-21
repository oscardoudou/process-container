all: benchmark 

#validate

benchmark: benchmark.c 
	$(CC) -g -O0 benchmark.c -o benchmark -I/usr/local/include -lpcontainer -lpthread
	
clean:
	rm -f benchmark 
