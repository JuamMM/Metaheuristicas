#include <vector>
#include <algorithm>
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

	poblaciones = pob_nueva;
}


void PAR::algoritmoCruce(int padre1, int padre2){
	vector<int> cromosomas;
	Poblacion hijo(tam,tam_centro,num_clusters,minimo,maximo);
	int datos_asignados = 0;

	for(int i=0; i<tam; i++){
		cromosomas.push_back(i);
	}

	while(datos_asignados < tam/2){
		int indice = rand() % cromosomas.size();
		int dato = cromosomas[indice];
		cromosomas.erase(remove(cromosomas.begin(),cromosomas.end(),dato),cromosomas.end());
		hijo.asignaDato(dato,poblaciones[padre1].devuelveCluster(dato));
		datos_asignados++;
	}

	for(auto it_cro = cromosomas.begin(); it_cro != cromosomas.end(); it_cro++){
		int dato = (*it_cro);
		hijo.asignaDato(dato,poblaciones[padre2].devuelveCluster(dato));
	}

}

void PAR::algoritmoMutacion(int pob){
	int dato = rand()%tam;
	int valor_nuevo = poblaciones[pob].devuelveCluster(dato);
	while(valor_nuevo == poblaciones[pob].devuelveCluster(dato)){
		valor_nuevo = rand() %num_clusters;
	}

	poblaciones[pob].asignaDato(dato,valor_nuevo);
}
