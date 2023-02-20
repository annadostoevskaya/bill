ifeq ($(OS), Windows_NT)
	CC = clang-cl
	TARGET = genbundle.exe
else
	CC = clang
	TARGET = genbundle
endif

all: $(TARGET)
	
$(TARGET): genbundle.o
	$(CC) genbundle.o -o $(TARGET)

genbundle.o: genbundle.cpp
	$(CC) genbundle.cpp -c -o genbundle.o

clean:
	rm $(TARGET) *.o

