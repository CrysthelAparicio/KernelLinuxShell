main.out: main.o
	g++ main.o -o main.out
	rm *.o

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm main.out
