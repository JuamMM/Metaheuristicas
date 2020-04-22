#include <cmath>
#include <algorithm>
#include <vector>
#include <list>
#include <tuple>
#include <map>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "PAR.h"
#include "timer.h"

using namespace std;

vector<vector<float>> datos;
list<tuple<int,int,double>>restricciones;
int datos_centro;
float epsilon= 0.0005;
float lambda = 0;

void leerDatos(string fich_datos){
	ifstream fichero;
	string dato;
	vector<float> linea;
	string cadena;

	fichero.open(fich_datos);

	while(!getline(fichero,cadena,'\n').eof()){
		istringstream reader(cadena);

		linea.resize(0);

		while(!getline(reader,dato,',').eof()){
			linea.push_back(stof(dato));
		}

		datos.push_back(linea);
	}
}

void leerRestricciones(string fichero_restricciones){
	ifstream fichero;
	pair<int,int> pos;
	int val1=0, val2=0;
	string cadena, dato;

	fichero.open(fichero_restricciones);

	while(!getline(fichero,cadena,'\n').eof()){
		istringstream reader(cadena);


		while(!getline(reader,dato,',').eof()){
			if(stod(dato) != 0){
				tuple<int,int,double> tupla = make_tuple(val1,val2,stod(dato));
				restricciones.push_back(tupla);
			}
			val2++;
		}
		val1++;
		val2 = 0;

	}

}

double calcularDistancia(vector<float> el1, vector<float> el2){
	int c1=0, c2=0;
	for(int i=0; i<el1.size();i++){
		c1 += el1[i];
		c2 += el2[i];
	}
	c1 /= el1.size();
	c2 /= el1.size();

	return abs(c1 - c2);
}

bool compara(float dist1, float dist2){
	return epsilon>(abs(dist1-dist2));
}

Poblacion Greedy(int num_datos, int num_clusters, int min, int max){
	Poblacion pob(num_datos, datos_centro, num_clusters, min, max);
	vector<int> indices;
	bool parar = false;
	float distancia_orig =10000;

	for(int i=0;i <num_datos; i++){
		indices.push_back(i);
	}

	while(!parar){

		random_shuffle(indices.begin(), indices.end());

		for(auto it_indie = indices.begin(); it_indie != indices.end(); it_indie++){
			int dato = (*it_indie);

			int cluster_correcto;
			double restricciones_minimas = 1000000;
			float distancia_minima = 100000;

			for(int cluster=0; cluster < num_clusters; cluster++){
				double infac = pob.calcularErrorParcial(dato,cluster,restricciones);

				if(infac < restricciones_minimas){
					restricciones_minimas = infac;
					cluster_correcto = cluster;
				}
			}

			for(int cluster=0; cluster<num_clusters;cluster++){
				float distancia = calcularDistancia(datos[dato],pob.devuelveCentroide(cluster));
				if(distancia_minima > distancia && pob.calcularErrorParcial(dato,cluster,restricciones) == restricciones_minimas){
					distancia_minima = distancia;
					cluster_correcto = cluster;
				}
			}

			pob.asignaDato(dato,cluster_correcto);
		}
		pob.actualizarCentroides(datos);
		float distancia_nueva = pob.desviacionGeneral(datos);

		if(compara(distancia_orig,distancia_nueva)){
			parar = true;
		}
		else{
			pob.vaciarClusters();
			distancia_orig = distancia_nueva;
		}
	}

	return pob;
}

void calculaLambda(){
	for(int i=0; i<datos.size(); i++){
		for(int e=i; e<datos.size(); e++){
			if(lambda < calcularDistancia(datos[i], datos[e])){
				lambda = calcularDistancia(datos[i], datos[e]);
			}
		}
	}

	lambda /= datos.size();
}

pair<float,float> leerMaximoMinimo(){
	pair<float,float> devolver;
	int minimo = 100000;
	int maximo = -100000;

	for(auto it_d = datos.begin(); it_d != datos.end(); it_d++){
		for(auto it_val = it_d->begin(); it_val != it_d->end(); it_val++){
			if((*it_val)>=maximo){
				maximo = (*it_val);
			}

			if((*it_val)<=minimo){
				minimo = (*it_val);
			}
		}
	}

	devolver.first = minimo;
	devolver.second = maximo;

	return devolver;

}

Poblacion BL(int num_datos, int num_clusters, int min, int max){
	Poblacion pob(num_datos, datos_centro, num_clusters, min, max);
	int iteraciones = 0, evaluaciones = 0;
	int error;

	pob.asignacionAleatoria();

	error = pob.calcularErrorGenerado(restricciones);
	pob.actualizarCentroides(datos);
	float valoracion = pob.desviacionGeneral(datos)+lambda*error;

	bool continuar = true;

	while(continuar && evaluaciones < 100000){
		bool cont_evaluacion = true;

		for(int dato = 0; dato<num_datos && cont_evaluacion; dato++){
			int clust_orig = pob.devuelveCluster(dato);

			for(int clust_nuevo=0; clust_nuevo<num_clusters && cont_evaluacion; clust_nuevo++){
				int rest_nueva;

				pob.asignaDato(dato,clust_nuevo);
				rest_nueva = pob.calcularErrorGenerado(restricciones);

				pob.actualizarCentroides(datos);

				float valoracion_nueva = pob.desviacionGeneral(datos)+lambda*rest_nueva;

				if(valoracion > valoracion_nueva){
					valoracion = valoracion_nueva;
					error = rest_nueva;
					cont_evaluacion = false;
				}
				else{
					pob.asignaDato(dato,clust_orig);
					pob.actualizarCentroides(datos);
				}
				evaluaciones++;

			}
		}

		if(cont_evaluacion){
			continuar=false;
		}
		iteraciones++;
	}

	cout<<"Iteraciones: "<<iteraciones<<endl;
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	return pob;
}

int main(int argc, char **argv){
	if(argc < 5){
		cout<<"La forma de uso de este programa es: ./P2 <numero de clusters> <numero de poblaciones> <path a los datos> <path a las resctricciones>"<<endl;
	}
	else{
		int clusters = stoi(argv[1]);
		int poblaciones = stoi(argv[2]);
		leerDatos(argv[3]);
		leerRestricciones(argv[4]);
		int minimo = leerMaximoMinimo().first;
		int maximo = leerMaximoMinimo().second;
		int numero_datos = datos.size();
		datos_centro = datos[0].size();
		calculaLambda();
		PAR problema(poblaciones, clusters, datos_centro, numero_datos, minimo, maximo);
		srand(time( NULL ));

		problema.generarPoblacionesAleatorias(numero_datos);
		problema.imprimirPoblaciones();
		problema.algoritmoMutacion(0);
		problema.imprimirPoblaciones();
	}
}
