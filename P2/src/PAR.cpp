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

int PAR::mejorCromosoma(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> res){
	float mejor_valoracion = 10000.0;
	int devolver = 0;

	for(int i=0; i<num_poblaciones;i++){
		float val = poblaciones[i].desviacionGeneral(datos)+lambda*poblaciones[i].calcularErrorGenerado(res);

		if(val < mejor_valoracion){
			mejor_valoracion = val;
			devolver = i;
		}
	}

	return devolver;
}

int PAR::peorCromosoma(double lambda, list<tuple<int,int,double>> res, vector<vector<float>> datos){
	float mejor_valoracion = -1;
	int devolver = 0;

	for(int i=0; i<num_poblaciones;i++){
		float val = poblaciones[i].desviacionGeneral(datos)+lambda*poblaciones[i].calcularErrorGenerado(res);

		if(val > mejor_valoracion){
			mejor_valoracion = val;
			devolver = i;
		}
	}

	return devolver;
}

Poblacion PAR::algoritmoCruce(int padre1, int padre2){
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

	return hijo;
}

void PAR::algoritmoMutacion(int pob, int prob_muta, int rango_muta){
	int muta = rand() % rango_muta;
	if(muta <= prob_muta){
		int dato = rand()%tam;
		int valor_nuevo = poblaciones[pob].devuelveCluster(dato);
		while(valor_nuevo == poblaciones[pob].devuelveCluster(dato)){
			valor_nuevo = rand() %num_clusters;
		}

		poblaciones[pob].asignaDato(dato,valor_nuevo);
	}
}

void PAR::BLsuave(double lambda, list<tuple<int,int,double>> restricciones, vector<vector<float>> datos){
	int errores_max = 0.1*tam;
	vector<int> indices;

	for(int i=0;i <tam; i++){
		indices.push_back(i);
	}

	for(int i=0;i<num_poblaciones;i++){
		random_shuffle(indices.begin(), indices.end());
		int errores = 0;

		for(auto it = indices.begin(); it != indices.end() && errores<errores_max;it++){
			int e = (*it);
			int cluster_orig = poblaciones[i].devuelveCluster(e);
			int error_orig = poblaciones[i].calcularErrorGenerado(restricciones);

			float val_orig = poblaciones[i].desviacionGeneral(datos)+lambda*error_orig;

			for(int cluster_nuevo = 0; cluster_nuevo<num_clusters && errores<errores_max;cluster_nuevo++){
				poblaciones[i].asignaDato(e,cluster_nuevo);
				poblaciones[i].actualizarCentroides(datos);

				int error_nuevo = poblaciones[i].calcularErrorGenerado(restricciones);
				float val_nuevo = poblaciones[i].desviacionGeneral(datos)+lambda*error_nuevo;

				if(val_orig > val_nuevo){
					val_orig = val_nuevo;
				}
				else{
					poblaciones[i].asignaDato(e,cluster_orig);
					poblaciones[i].actualizarCentroides(datos);
					errores++;
				}
			}
		}
	}

}
