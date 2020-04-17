#include <vector>
#include <string>
#include <map>
#include <stdlib.h>

#include "PAR.h"

using namespace std;

void PAR::generarPoblacionesAleatorias(int num_datos){
	for(int i=0; i<num_poblaciones; i++){
		for(int e=0;e<num_datos;e++){
			int cluster = rand() %num_clusters;
			poblaciones[i].asignaDato(e,cluster);
		}
	}
}

void PAR::imprimirPoblaciones(){
	for(int i=0; i<num_poblaciones;i++){
		cout<<"Poblacion: "<<i<<endl;
		poblaciones[i].imprimePoblacion();
	}
	cout<<endl;
}
