#include <vector>
#include <math.h>
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

void PAR::reparacion(){
	for(int i=0; i<num_poblaciones; i++){
		for(int cluster=0; cluster<num_clusters; cluster++){
			if(poblaciones[i].tamCluster(cluster) == 0){
				int dato = rand()%tam;
				poblaciones[i].asignaDato(dato,cluster);
			}
		}
	}
}


int PAR::mejorCromosoma(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> res){
	float mejor_valoracion = 10000.0;
	int devolver = 0;

	for(int i=0; i<num_poblaciones;i++){
		float val = poblaciones[i].devolverValoracion();

		if(val < mejor_valoracion && -1!=val){
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
		float val = poblaciones[i].devolverValoracion();

		if(val > mejor_valoracion && -1!=val){
			mejor_valoracion = val;
			devolver = i;
		}
	}

	return devolver;
}


void PAR::algoritmoMutacion(int pob, int prob_muta, int rango_muta){
	for(int dato=0; dato<tam;dato++){
		int muta = rand() % rango_muta;
		if(muta <= prob_muta){
			int valor_nuevo = poblaciones[pob].devuelveCluster(dato);
			while(valor_nuevo == poblaciones[pob].devuelveCluster(dato)){
				valor_nuevo = rand() %num_clusters;
			}
			poblaciones[pob].asignaDato(dato,valor_nuevo);
		}
	}
}


void PAR::BLsuaveMejores(double lambda, list<tuple<int,int,double>> restricciones, vector<vector<float>> datos, int num_mejores){
	int errores_max = 0.1*tam;
	vector<int> indices;

	for(int i=0;i <tam; i++){
		indices.push_back(i);
	}

	vector<int> mejores_cro = mejoresPadres(lambda,datos,restricciones,num_mejores);

	for(auto it = mejores_cro.begin(); it != mejores_cro.end(); it++){
		int i = (*it);
		random_shuffle(indices.begin(), indices.end());
		int errores = 0;

		for(auto it = indices.begin(); it != indices.end() && errores<errores_max;it++){
			int e = (*it);
			int cluster_orig = poblaciones[i].devuelveCluster(e);

			float val_orig = poblaciones[i].devolverValoracion();

			for(int cluster_nuevo = 0; cluster_nuevo<num_clusters && errores<errores_max;cluster_nuevo++){
				poblaciones[i].asignaDato(e,cluster_nuevo);
				poblaciones[i].actualizarCentroides();
				poblaciones[i].calcularValoracion();

				float val_nuevo = poblaciones[i].devolverValoracion();

				if(val_orig > val_nuevo){
					val_orig = val_nuevo;
					poblaciones[i].asignarValoracion(val_nuevo);
				}
				else{
					poblaciones[i].asignaDato(e,cluster_orig);
					poblaciones[i].actualizarCentroides();
					errores++;
					poblaciones[i].asignarValoracion(val_orig);
				}
			}
		}
	}
}


vector<int> PAR::mejoresPadres(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> res, int num_padres){
	float mejor_val;

	vector<int>devolver(num_padres,-1);

	for(int mejor = 0; mejor <num_padres; mejor++){
		mejor_val = 1000000;
		for(int i=0; i<num_poblaciones;i++){
			float val = poblaciones[i].devolverValoracion();
			if(val < mejor_val && find(devolver.begin(),devolver.end(),i) == devolver.end() && -1!=val){
				mejor_val = val;
				devolver[mejor] = i;
			}
		}
	}

	return devolver;

}

vector<int> PAR::peoresPadres(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> res, int num_padres){
	float peor_val;

	vector<int>devolver(num_padres,-1);

	for(int mejor = 0; mejor <num_padres; mejor++){
		peor_val = 0;
		for(int i=0; i<num_poblaciones;i++){
			float val = poblaciones[i].devolverValoracion();
			if(val > peor_val && find(devolver.begin(),devolver.end(),i) == devolver.end() && -1!=val){
				peor_val = val;
				devolver[mejor] = i;
			}
		}
	}

	return devolver;

}
