#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>

#include "PAR.h"

using namespace std;

vector<vector<float>> datos;
map<pair<int,int>,int> restricciones;
int datos_centro;

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
			if(stoi(dato) != 0 && val1>=val2){
				pos = make_pair(val1,val2);
				restricciones.insert(pair<pair<int,int>,int>(pos,stod(dato)));
			}
			val2++;
		}
		val1++;
		val2 = 0;

	}

}

int calcularErrorParcial(int dato, int cluster1, Poblacion pob){
	int errores = 0;
	for(auto it_res = restricciones.begin(); it_res != restricciones.end(); it_res++){
		if( it_res->first.first == dato ){
			int cluster2 = pob.devuelveCluster(it_res->first.second);
			if(cluster2 != -1){
				if(cluster1 == cluster2 && it_res->second == -1){
					errores++;
				}
				else if(cluster1 != cluster2 && it_res->second == 1){
					errores++;
				}
			}
		}
	}
	return errores;
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

Poblacion Greedy(int num_datos, int num_clusters, int min, int max){
	Poblacion pob(num_datos, datos_centro, num_clusters, min, max);
	vector<int> indices;
	bool parar = false;

	for(int i=0;i <num_datos; i++){
		indices.push_back(i);
	}

	random_shuffle(indices.begin(), indices.end());

	while(!parar){
		for(auto it_indie = indices.begin(); it_indie != indices.end(); it_indie++){
			int dato = (*it_indie);

			int cluster_correcto;
			double restricciones_minimas = 1000000;
			double distancia_minima = 100000;

			for(int cluster=0; cluster < num_clusters; cluster++){
				double infac = calcularErrorParcial(dato, cluster, pob);
				if(infac < restricciones_minimas){
					restricciones_minimas = infac;
					cluster_correcto = cluster;
				}
			}

			for(int cluster=0; cluster<num_clusters;cluster++){
				double distancia = calcularDistancia(datos[dato],pob.devuelveCentroide(cluster));
				if(distancia_minima > distancia && calcularErrorParcial(dato,cluster,pob) == restricciones_minimas){
					distancia_minima = distancia;
					cluster_correcto = cluster;
				}
			}

			pob.asignaDato(dato,cluster_correcto);
		}
		pob.actualizarCentroides(datos);
		parar = true;
	}

	return pob;
}

int calcularErrorGenerado(Poblacion pob){
	int errores = 0;
	for(int i =0; i< pob.tamanio(); i++){
		int cluster1 = pob.devuelveCluster(i);
		for(auto it_res = restricciones.begin(); it_res != restricciones.end(); it_res++){
			if( it_res->first.first == i ){
				int cluster2 = pob.devuelveCluster(it_res->first.second);
				if(cluster1 == cluster2 && it_res->second == -1){
					errores++;
				}
				else if(cluster1 != cluster2 && it_res->second == 1){
					errores++;
				}
			}
		}
	}
	return errores;
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
		PAR problema(poblaciones, datos_centro, clusters, numero_datos, minimo, maximo);

		Poblacion pob = Greedy(numero_datos, clusters, minimo, maximo);
		pob.imprimePoblacion();
		cout<<"Error: "<<calcularErrorGenerado(pob)<<endl;

		problema.imprimirPoblaciones();
		for(int i=0; i<poblaciones; i++){
			cout<<"Error poblacion "<<i<<": "<<endl;
			cout<<calcularErrorGenerado(problema.devuelvePoblacion(i));
			cout<<endl;
		}
	}
}
