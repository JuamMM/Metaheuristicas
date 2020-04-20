#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Poblacion.h"

using namespace std;

Poblacion::Poblacion(int max_datos, int datos_centro, int clusters, int min, int max){
	num_datos = max_datos;
	vector<int> tamanio(clusters,0);
	tam = tamanio;

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
	if(datos[dato]!=-1){
		tam[datos[dato]]--;
	}

	datos[dato] = cluster;
	if(cluster!=-1){
		tam[cluster]++;
	}
}

void Poblacion::asignacionAleatoria(){
	for(int e=0;e<num_datos;e++){
		int cluster_nuevo = (rand()%num_clusters);
		asignaDato(e,cluster_nuevo);
	}
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

float Poblacion::distanciaIntraCluster(vector<vector<float>> centros, int cluster){
	int indice = 0;
	float distancia = 0.0;

	for(auto it = centros.begin(); it != centros.end(); it++){
		if(cluster == datos[indice]){
			int i=0;
			for(auto it_datos = it->begin(); it_datos != it->end(); it_datos++){
				distancia += abs(centroides[cluster][i]-(*it_datos))*1.0;
				i++;
			}
			distancia /=centroides[cluster].size();
		}
		indice++;
	}

	distancia = distancia/tam[cluster]*1.0;

	return distancia;
}

float Poblacion::desviacionGeneral(vector<vector<float>> datos){
	float distancia = 0.0;

	for(int i=0; i<num_clusters; i++){
		distancia+=distanciaIntraCluster(datos,i);
	}

	return distancia/(num_clusters*1.0);
}

int Poblacion::calcularErrorGenerado(int num_datos, list<tuple<int,int,int>> rest){
	int errores = 0;
	for(int i=0; i< num_datos; i++){
		errores += calcularErrorParcial(i,datos[i]);
	}
	return errores;
}
