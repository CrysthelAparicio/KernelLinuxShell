main.out: main.o
	g++ main.o -o main.out

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o main.out
