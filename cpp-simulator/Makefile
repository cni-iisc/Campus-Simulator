include_paths = -Ilibs/
headers = models.h initializers.h interventions.h
obj = initializers.o models.o interventions.o
opts = --std=c++14 -O3
CC = g++ -DDEBUG $(opts) $(include_paths)

interventions.o: interventions.cc $(headers)
	$(CC) -c interventions.cc -o interventions.o

models.o: models.cc $(headers)
	$(CC) -c models.cc -o models.o

initializers.o: initializers.cc $(headers)
	$(CC) -c initializers.cc -o initializers.o

simulator: simulator.cc $(obj) $(headers)
	$(CC) -o simulator simulator.cc $(obj);

all: simulator

clean:
	rm simulator $(obj)

