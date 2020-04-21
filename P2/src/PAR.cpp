#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>

#include "PAR.h"

using namespace std;

void PAR::generarPoblacionesAleatorias(int num_datos){
	for(int i=0; i<num_poblaciones; i++){
		poblaciones[i].asignacionAleatoria();
	}
}

void PAR::imprimirPoblaciones(){
	for(int i=0; i<num_poblaciones;i++){
		cout<<"Poblacion: "<<i<<endl;
		poblaciones[i].imprimePoblacion();
	}
	cout<<endl;
}

void PAR::algoritmoSeleccionador(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> rest){
	vector<Poblacion> pob_nueva;
	for(int i=0; i<num_poblaciones; i++){
		int pob1 = (rand()%num_poblaciones);
		int pob2 = (rand()%num_poblaciones);
		while(pob1 == pob2){
			pob2 = (rand()%num_poblaciones);
		}
		int error1 = poblaciones[pob1].calcularErrorGenerado(rest);
		int error2 = poblaciones[pob2].calcularErrorGenerado(rest);

		float val1 = poblaciones[pob1].desviacionGeneral(datos)+lambda*error1;
		float val2 = poblaciones[pob2].desviacionGeneral(datos)+lambda*error2;

		if(val1 < val2){
			pob_nueva.push_back(poblaciones[pob1]);
		}
		else{
			pob_nueva.push_back(poblaciones[pob2]);
		}
	}
}
