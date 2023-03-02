CC = g++
topart: main.cpp
	$(CC) main.cpp -o topart

clean:
	rm topart