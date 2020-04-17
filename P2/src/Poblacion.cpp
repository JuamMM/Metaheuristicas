#include <vector>
#include <math.h>

#include "Poblacion.h"

using namespace std;

Poblacion::Poblacion(int max_datos, int datos_centro, int clusters, int min, int max){
	num_datos = max_datos;
	for(int i=0; i <clusters; i++){
		vector<float> centros;
		for(int e=0; e<datos_centro;e++){
			centros.push_back(float(rand()%(max-min) +min));
		}
		centroides.push_back(centros);
	}
	num_clusters = clusters;
	vector<int> aux(max_datos,-1);
	datos = aux;
}

void Poblacion::asignaDato(int dato, int cluster){
	datos[dato] = cluster;
}

void Poblacion::actualizarCentroides(vector<vector<float>> centros){
	int indice=0;
	int cluster;
	for(auto it_dato = centros.begin(); it_dato != centros.end(); it_dato++){
		cluster = datos[indice];
		int i=0;
		for(auto it = it_dato->begin(); it != it_dato->end(); it++){
			centroides[cluster][i] += (*it);
			centroides[cluster][i] /= 2;
			i++;
		}
	}
}

void Poblacion::imprimePoblacion(){
	for(int i=0; i<num_datos; i++){
		cout<<datos[i]<<" ";
	}
	cout<<endl;
}

float Poblacion::distanciaIntraCluster(vector<vector<float>> centros){
	int indice = 0;
	int cluster;
	vector<float> dist(num_clusters,0.0);

	for(auto it = centros.begin(); it != centros.end(); it++){
		cluster = datos[indice];
		int i=0;
		for(auto it_datos = it->begin(); it_datos != it->end(); it_datos++){
			dist[cluster] += abs(centroides[cluster][i]-(*it_datos));
			i++;
		}
		indice++;
	}
	for(int i=0; i<num_clusters;i++){
		dist[i]
	}
	dist = dist/num_datos;
	return dist/num_clusters;
}
