ifeq ($(OS), Windows_NT)
	CC = clang-cl
	TARGET = genbundle.exe
else
	CC = clang++
	TARGET = genbundle
endif

all: $(TARGET)

$(TARGET): genbundle.o scandir.o
	$(CC) -Wall -std=c++17 genbundle.o scandir.o -o $(TARGET)

genbundle.o: genbundle.cpp
	$(CC) genbundle.cpp -std=c++17 -c -Wall -o genbundle.o

scandir.o: scandir.cpp
	$(CC) scandir.cpp -std=c++17 -c -Wall -o scandir.o

clean:
	rm $(TARGET) *.o

