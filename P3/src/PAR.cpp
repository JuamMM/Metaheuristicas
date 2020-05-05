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

Poblacion PAR::algoritmoCruceUN(int padre1, int padre2){
	vector<int> cromosomas;
	Poblacion hijo(tam,tam_centro,num_clusters,minimo,maximo);
	int datos_asignados = 0;

	for(int i=0; i<tam; i++){
		cromosomas.push_back(i);
	}

	random_shuffle(cromosomas.begin(), cromosomas.end());

	for(auto it_cro = cromosomas.begin(); it_cro != cromosomas.end(); it_cro++){
		int dato = cromosomas[(*it_cro)];
		if(datos_asignados<cromosomas.size()/2){
			hijo.asignaDato(dato,poblaciones[padre1].devuelveCluster(dato));
		}
		else{
			hijo.asignaDato(dato,poblaciones[padre2].devuelveCluster(dato));
		}
		datos_asignados++;
	}

	return hijo;
}

Poblacion PAR::algoritmoCruceSF(int padre1, int padre2){
	vector<int> cromosomas;
	int inicio_segmento = rand() % tam;
	int longitud_segmento = rand() % tam;
	int fin_segmento = inicio_segmento + longitud_segmento;
	Poblacion hijo(tam,tam_centro,num_clusters,minimo,maximo);
	int datos_asignados = 0;

	for(int i=0; i<tam; i++){
		cromosomas.push_back(i);
	}

	for(auto it_cro = cromosomas.begin(); it_cro != cromosomas.end(); it_cro++){
		int dato = cromosomas[(*it_cro)];
		if(dato >= inicio_segmento && dato<=fin_segmento){
			hijo.asignaDato(dato,poblaciones[padre1].devuelveCluster(dato));
		}
		else{
			hijo.asignaDato(dato,poblaciones[padre2].devuelveCluster(dato));
		}
	}

	return hijo;
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

void PAR::BLsuaveAleatoria(double lambda, list<tuple<int,int,double>> restricciones, vector<vector<float>> datos){
	int errores_max = 0.1*tam;
	vector<int> indices;

	for(int i=0;i <tam; i++){
		indices.push_back(i);
	}

	for(int i=0;i<num_poblaciones;i++){
		int aplicarbl = rand() %100;
		if(aplicarbl <= 10){
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

void PAR::BLsuaveCompleta(double lambda, list<tuple<int,int,double>> restricciones, vector<vector<float>> datos){
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

void PAR::sustituirPeoresPadres(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> res, Poblacion pob1, Poblacion pob2){
	float mejor_val1 = 0, mejor_val2 = 0;

	vector<int>peores(2,0);

	for(int i=0; i<num_poblaciones;i++){
		float val = poblaciones[i].desviacionGeneral(datos)+lambda*poblaciones[i].calcularErrorGenerado(res);

		if(val > mejor_val1){
			mejor_val1 = val;
			peores[0] = i;
		}
	}

	for(int i=0; i<num_poblaciones;i++){
		float val = poblaciones[i].desviacionGeneral(datos)+lambda*poblaciones[i].calcularErrorGenerado(res);
		if(val > mejor_val2 && i != peores[0]){
			 mejor_val2 = val;
			 peores[1] = i;
		}
	}

	float val1 = pob1.desviacionGeneral(datos)+lambda*pob1.calcularErrorGenerado(res);
	float val2 = pob2.desviacionGeneral(datos)+lambda*pob2.calcularErrorGenerado(res);

	if(mejor_val1 > val1){
		sustituyePoblacion(peores[0],pob1);
	}

	if(mejor_val2 > val2){
		sustituyePoblacion(peores[1],pob2);
	}
}

vector<int> PAR::mejoresPadres(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> res, int num_padres){
	float mejor_val;

	vector<int>devolver(num_padres,-1);

	for(int mejor = 0; mejor <num_padres; mejor++){
		mejor_val = 1000000;
		for(int i=0; i<num_poblaciones;i++){
			float val = poblaciones[i].desviacionGeneral(datos)+lambda*poblaciones[i].calcularErrorGenerado(res);
			if(val < mejor_val && find(devolver.begin(),devolver.end(),i) == devolver.end()){
				mejor_val = val;
				devolver[mejor] = i;
			}
		}
	}

	return devolver;

}
