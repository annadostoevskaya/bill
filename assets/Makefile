CC = clang++

ifeq ($(OS), Windows_NT)
	TARGET = genbundle.exe
else
	TARGET = genbundle
endif

all: $(TARGET)

$(TARGET): genbundle.cpp
	$(CC) -Wall -std=c++17 genbundle.cpp -o $(TARGET)

clean:
	rm $(TARGET) *.o

