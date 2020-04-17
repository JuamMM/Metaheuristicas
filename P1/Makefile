HOME    = .
BIN     = $(HOME)/bin
INCLUDE = $(HOME)/include
SRC     = $(HOME)/src
OBJ     = $(HOME)/obj
LIB     = $(HOME)/lib

FLAGS = --std=c++11 -O2

all: $(BIN)/greedy $(BIN)/bl $(BIN)/prueba


$(BIN)/prueba: $(OBJ)/ARP.o $(OBJ)/prueba.o $(OBJ)/Cluster.o $(OBJ)/timer.o
	g++ $(FLAGS) $(OBJ)/prueba.o $(OBJ)/ARP.o $(OBJ)/Cluster.o $(OBJ)/timer.o -o $(BIN)/prueba

$(BIN)/bl: $(OBJ)/ARP.o $(OBJ)/practica2.o $(OBJ)/Cluster.o $(OBJ)/timer.o
	g++ $(FLAGS) $(OBJ)/practica2.o $(OBJ)/ARP.o $(OBJ)/Cluster.o $(OBJ)/timer.o -o $(BIN)/bl

$(BIN)/greedy: $(OBJ)/ARP.o $(OBJ)/practica1.o $(OBJ)/Cluster.o $(OBJ)/timer.o
	g++ $(FLAGS) $(OBJ)/practica1.o $(OBJ)/ARP.o $(OBJ)/Cluster.o $(OBJ)/timer.o -o $(BIN)/greedy

$(OBJ)/prueba.o: $(SRC)/prueba.cpp $(INCLUDE)/ARP.h
	g++ -c $(FLAGS) $(SRC)/prueba.cpp -I$(INCLUDE) -o $(OBJ)/prueba.o

$(OBJ)/practica2.o: $(SRC)/practica2.cpp $(INCLUDE)/ARP.h
	g++ -c $(FLAGS) $(SRC)/practica2.cpp -I$(INCLUDE) -o $(OBJ)/practica2.o

$(OBJ)/practica1.o: $(SRC)/practica1.cpp $(INCLUDE)/ARP.h
	g++ -c $(FLAGS) $(SRC)/practica1.cpp -I$(INCLUDE) -o $(OBJ)/practica1.o

$(OBJ)/ARP.o: $(SRC)/ARP.cpp $(INCLUDE)/ARP.h $(INCLUDE)/Cluster.h
	g++ -c $(FLAGS) $(SRC)/ARP.cpp -I$(INCLUDE) -o $(OBJ)/ARP.o

$(OBJ)/Cluster.o: $(SRC)/Cluster.cpp  $(INCLUDE)/Cluster.h
	g++ -c $(FLAGS) $(SRC)/Cluster.cpp -I$(INCLUDE) -o $(OBJ)/Cluster.o

$(OBJ)/timer.o: $(SRC)/timer.c $(INCLUDE)/timer.h
	g++ -c $(FLAGS) $(SRC)/timer.c -I$(INCLUDE) -o $(OBJ)/timer.o


clean:
	-rm $(OBJ)/*.o
	-rm $(BIN)/*
