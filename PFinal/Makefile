HOME    = .
BIN     = $(HOME)/bin
INCLUDE = $(HOME)/include
SRC     = $(HOME)/src
OBJ     = $(HOME)/obj
LIB     = $(HOME)/lib
exe     = $(BIN)/PFINAL_exe

FLAGS = --std=c++11 -O3

all: $(exe)


$(exe): $(OBJ)/PAR.o $(OBJ)/PFINAL.o $(OBJ)/Poblacion.o $(OBJ)/timer.o
	g++ $(FLAGS) $(OBJ)/PFINAL.o $(OBJ)/PAR.o $(OBJ)/Poblacion.o $(OBJ)/timer.o -o $(exe)

$(OBJ)/PFINAL.o: $(SRC)/PFinal.cpp $(INCLUDE)/PAR.h
	g++ -c $(FLAGS) $(SRC)/PFinal.cpp -I$(INCLUDE) -o $(OBJ)/PFINAL.o

$(OBJ)/PAR.o: $(SRC)/PAR.cpp $(INCLUDE)/PAR.h $(INCLUDE)/Poblacion.h
	g++ -c $(FLAGS) $(SRC)/PAR.cpp -I$(INCLUDE) -o $(OBJ)/PAR.o

$(OBJ)/Poblacion.o: $(SRC)/Poblacion.cpp  $(INCLUDE)/Poblacion.h
	g++ -c $(FLAGS) $(SRC)/Poblacion.cpp -I$(INCLUDE) -o $(OBJ)/Poblacion.o

$(OBJ)/timer.o: $(SRC)/timer.c $(INCLUDE)/timer.h
	g++ -c $(FLAGS) $(SRC)/timer.c -I$(INCLUDE) -o $(OBJ)/timer.o


clean:
	-rm $(OBJ)/*.o
	-rm $(BIN)/*_exe
