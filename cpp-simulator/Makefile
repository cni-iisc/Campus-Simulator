include_paths = -Ilibs/
headers = models.h initializers.h interventions.h updates.h simulator.h outputs.h
obj = initializers.o models.o interventions.o updates.o simulator.o outputs.o
opts = --std=c++14 -O3 -fopenmp
CC = g++ $(opts) $(include_paths)

outputs.o: outputs.cc $(headers)
	$(CC) -c outputs.cc -o outputs.o

simulator.o: simulator.cc $(headers)
	$(CC) -c simulator.cc -o simulator.o

updates.o: updates.cc $(headers)
	$(CC) -c updates.cc -o updates.o

interventions.o: interventions.cc $(headers)
	$(CC) -c interventions.cc -o interventions.o

models.o: models.cc $(headers)
	$(CC) -c models.cc -o models.o

initializers.o: initializers.cc $(headers)
	$(CC) -c initializers.cc -o initializers.o

drive_simulator: drive_simulator.cc $(obj) $(headers)
	$(CC) -o drive_simulator drive_simulator.cc $(obj)

all: drive_simulator

clean:
	rm drive_simulator $(obj)

