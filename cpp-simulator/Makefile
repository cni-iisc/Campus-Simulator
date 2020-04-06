include_paths = -Ilibs/
headers = models.h initializers.h
obj = initializers.o models.o
opts = --std=c++14
CC = g++ $(opts) $(include_paths)

models.o: models.cc $(headers)

initializers.o: initializers.cc $(headers)
	$(CC) -c initializers.cc -o initializers.o

simulator: simulator.cc $(obj) $(headers)
	$(CC) -o simulator simulator.cc $(obj);

all: simulator

clean:
	rm simulator $(obj)

