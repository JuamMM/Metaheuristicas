HOME    = .
BIN     = $(HOME)/bin
INCLUDE = $(HOME)/include
SRC     = $(HOME)/src
OBJ     = $(HOME)/obj
LIB     = $(HOME)/lib

FLAGS = --std=c++11 -O3

all: $(BIN)/P3


$(BIN)/P3: $(OBJ)/PAR.o $(OBJ)/P3.o $(OBJ)/Poblacion.o $(OBJ)/timer.o
	g++ $(FLAGS) $(OBJ)/P3.o $(OBJ)/PAR.o $(OBJ)/Poblacion.o $(OBJ)/timer.o -o $(BIN)/P3

$(OBJ)/P3.o: $(SRC)/P3.cpp $(INCLUDE)/PAR.h
	g++ -c $(FLAGS) $(SRC)/P3.cpp -I$(INCLUDE) -o $(OBJ)/P3.o

$(OBJ)/PAR.o: $(SRC)/PAR.cpp $(INCLUDE)/PAR.h $(INCLUDE)/Poblacion.h
	g++ -c $(FLAGS) $(SRC)/PAR.cpp -I$(INCLUDE) -o $(OBJ)/PAR.o

$(OBJ)/Poblacion.o: $(SRC)/Poblacion.cpp  $(INCLUDE)/Poblacion.h
	g++ -c $(FLAGS) $(SRC)/Poblacion.cpp -I$(INCLUDE) -o $(OBJ)/Poblacion.o

$(OBJ)/timer.o: $(SRC)/timer.c $(INCLUDE)/timer.h
	g++ -c $(FLAGS) $(SRC)/timer.c -I$(INCLUDE) -o $(OBJ)/timer.o


clean:
	-rm $(OBJ)/*.o
	-rm $(BIN)/*
