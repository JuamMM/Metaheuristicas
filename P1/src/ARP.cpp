#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <string>

#include "ARP.h"

using namespace std;

float calcularDistancia(vector<float> vec1, vector<float> vec2){
	float distancia = 0;
	for(int i=0; i< vec1.size(); i++){
		distancia += pow(abs(vec1[i] - vec2[i]),2)*1.0;
	}

	distancia /= vec2.size() *1.0;

	return distancia;
}

bool compara(float val1, float  val2){
	bool devolver = false;
	float epsilon = 0.005;
	if(epsilon >= abs(val1-val2)){
		devolver = true;
	}
	return devolver;
}

pair<float,float> leerMaximoMinimo(string nombre_fichero){
	pair<float,float> devolver;
	int minimo = 100000;
	int maximo = -100000;

	ifstream fichero;
	string dato;
	vector<float> linea;
	string cadena;

	fichero.open(nombre_fichero);

	while(!getline(fichero,cadena,'\n').eof()){
		istringstream reader(cadena);

		linea.resize(0);

		while(!getline(reader,dato,',').eof()){
			if(stof(dato)>= maximo){
				maximo = stof(dato);
			}
			if(stof(dato)<=minimo){
				minimo = stof(dato);
			}
		}
	}

	devolver.first = minimo;
	devolver.second = maximo;

	return devolver;

}

ARP::ARP(string fich_datos, string fich_restricciones, int clusters){
	pair<float,float> minmax = leerMaximoMinimo(fich_datos);
	minimo = minmax.first;
	maximo = minmax.second;
	num_clusters = clusters;

	lambda = 0;
	leerDatos(fich_datos);
	leerRestricciones(fich_restricciones);
	num_datos = datos[0].size();
	num_restricciones = (restricciones.size()/2);

	for(int x_datos = 0; x_datos < datos.size(); x_datos++){
		indices.push_back(x_datos);
	}
	for(int i=0; i<datos.size(); i++){
		for(int e=i; e<datos.size(); e++){
			if(lambda < calcularDistancia(datos[i], datos[e])){
				lambda = calcularDistancia(datos[i], datos[e]);
			}
		}
	}

	lambda /= num_restricciones;
}

void ARP::eliminarDatos(){
	actualizarCentroides();
	for(int i=0; i<num_clusters; i++){
		sol[i].vaciarDatos();
	}
}


void ARP::generarCentroides(int min, int max){
	 vector<float> datos_intro;
	 sol.clear();
	 for(int i=0; i<num_clusters; i++){
		 for(int e=0; e<num_datos; e++){
			datos_intro.push_back((rand() % int(max - min) ) + min);
		 }
		sol.push_back(datos_intro);
	   datos_intro.clear();
	 }
}

void ARP::leerRestricciones(string fich_restricciones){
	ifstream fichero;
	pair<int,int> pos;
	int val1=0, val2=0;
	string cadena, dato;

	fichero.open(fich_restricciones);

	while(!getline(fichero,cadena,'\n').eof()){
		istringstream reader(cadena);


		while(!getline(reader,dato,',').eof()){
			if(stoi(dato) != 0){
				pos = make_pair(val1,val2);
				restricciones.insert(pair<pair<int,int>,int>(pos,stod(dato)));
			}
			val2++;
		}
		val1++;
		val2 = 0;

	}
}

void ARP::leerDatos(string fich_datos){
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


void ARP::imprimirDatos(){
	vector< vector<float> >::iterator x;
	vector<float>::iterator y;
	for(x = datos.begin(); x != datos.end();x++){
		for(y = x->begin();y != x->end();y++){
			cout<<*y<<" ";
		}
		cout<<endl;
	}
}

void ARP::imprimirRestricciones(){
	for(auto i = restricciones.begin(); i != restricciones.end(); i++){
		cout<<i->first.first<<" "<<i->first.second<<"\t"<<i->second<<endl;
	}
}

void ARP::imprimirClusters(){
	set<int> centros;
	for(int i=0; i<sol.size(); i++){
		cout<<"Cluster "<<i<<": "<<sol[i].numeroDatos()<<" Datos: ";
		centros = sol[i].devuelveDatos();
		for(set<int>::iterator it = centros.begin(); it != centros.end(); it++){
			cout<<(*it)<<", ";
		}
		cout<<endl;
	}
}

double ARP::calcularErrorGenerado(int dato, int numero_cluster){
	double infac=0;
	int dato2;
	int cluster2;
	set<int> cluster = sol[numero_cluster].devuelveDatos();

	for(auto it_map = restricciones.begin(); it_map != restricciones.end(); it_map++){
		dato2 = it_map->first.second;

		if(dato < dato2 && it_map->first.first == dato){
			cluster2 = buscaCluster(dato2);

			if(cluster2 == numero_cluster && it_map->second == -1){
				infac++;
			}
			else if(cluster2 != -1 && cluster2 != numero_cluster && it_map->second == 1){
				infac++;
			}

		}

	}

	return infac;
}

void ARP::Greedy(unsigned semilla){
	vector <float>::iterator y_datos;
	vector<float>::iterator it_clusters;
	vector<int>::iterator it_indie;
	double cluster =0, dato;;
	bool parar = false;
	float desviacion_general=0, desviacion_original=1;
	int distancia_minima, restricciones_minimas;

	srand(semilla);

	generarCentroides(minimo,maximo);

	random_shuffle(indices.begin(), indices.end());

	while(!parar){
		for(it_indie = indices.begin(); it_indie != indices.end(); it_indie++){
			dato = (*it_indie);

			vector<float> centro = datos[dato];
			restricciones_minimas = 100000000;
			distancia_minima = 100000;

			double infac;
			double distancia;

			for(int i=0; i<num_clusters; i++){
				infac = calcularErrorGenerado(dato,i);
				if(infac <= restricciones_minimas){
					restricciones_minimas = infac;
					cluster = i;
				}
			}

			for(int i=0; i<num_clusters; i++){
				distancia = calcularDistancia(sol[i].devuelveCentroide(), centro);
				if(calcularErrorGenerado(dato,i) == restricciones_minimas && distancia <= distancia_minima){
					distancia_minima = distancia;
					cluster = i;
				}
			}
				sol[cluster].aniadeDatos(dato);
		}

		desviacion_general = calcularDistanciaIntraCluster();


		if(compara(desviacion_general,desviacion_original)){
			parar = true;
		}
		else{
			eliminarDatos();
			desviacion_original = desviacion_general;
			desviacion_general=0;
		}
	}

	double infact = calcularInfactivilidad();
	double dist = calcularDistanciaIntraCluster();
	double agregado = dist+lambda*infact;
	imprimirClusters();
	cout<<"Agregado: "<<agregado<<endl;
	cout<<"Infactivilidad: "<<calcularInfactivilidad()<<endl;
	cout<<"Desviacion: "<<desviacion_original<<endl;

}

double ARP::calcularInfactivilidad(){
	int errores=0;
	int cluster1, cluster2;
	int dato1, dato2;

	for(auto it_restric = restricciones.begin(); it_restric != restricciones.end(); it_restric++){
		if( it_restric->first.first < it_restric->first.second){
			dato1 = it_restric->first.first;
			dato2 = it_restric->first.second;

			cluster1 = buscaCluster(dato1);
			cluster2 = buscaCluster(dato2);

			if(cluster1 == cluster2 && it_restric->second == -1){
				errores++;
			}
			else if(cluster1 != cluster2 && it_restric->second == 1){
				errores++;
			}
		}

	}

	return errores;
}

int ARP::buscaCluster(int dato){
	set<int> datos;
	int cluster = -1;
	for(int i=0; i<num_clusters; i++){
		datos = sol[i].devuelveDatos();
		if(datos.find(dato) != datos.end()){
			cluster = i;
		}
	}
	return cluster;
}

void ARP::intercambiaDato(int dato, int cluster_nuevo, int cluster_orig){
	sol[cluster_orig].eliminaDatos(dato);
	sol[cluster_nuevo].aniadeDatos(dato);
}

void ARP::BL(unsigned semilla){
	vector<int>::iterator it_indie;
	vector <float>::iterator y_datos;
	int iteraciones = 0;
	int evaluaciones = 0;
	bool continuar = true;
	bool cont_evaluacion = true;
	double valoracion, valoracion_nueva;
	int rest_incumplidas, rest_orig;

	srand(semilla);

	generarCentroides(minimo, maximo);

	for(it_indie = indices.begin(); it_indie != indices.end(); it_indie++){
		int dato = (*it_indie);

		int numero_cluster = rand() % num_clusters;
		sol[numero_cluster].aniadeDatos(dato);
	}

	actualizarCentroides();

	rest_incumplidas = calcularInfactivilidad();
	valoracion = calcularDistanciaIntraCluster()+lambda*rest_incumplidas;

	while(continuar){
		vector<Cluster> inicial = sol;
		rest_orig = rest_incumplidas;

		cont_evaluacion = true;
		random_shuffle(indices.begin(), indices.end());

		for(auto it_indice =indices.begin(); it_indice != indices.end() && cont_evaluacion; it_indice++){
			int cluster_nuevo = rand() % num_clusters;
			int indice = (*it_indice);
			int cluster_ini = buscaCluster(indice);

			rest_incumplidas -= calcularErrorGenerado(indice, cluster_ini);
			rest_incumplidas += calcularErrorGenerado(indice, cluster_nuevo);

			intercambiaDato(indice, cluster_nuevo, cluster_ini);

			actualizarCentroides();

			valoracion_nueva = calcularDistanciaIntraCluster()+lambda*rest_incumplidas;

			if(valoracion > valoracion_nueva){
				cont_evaluacion = false;
				valoracion = valoracion_nueva;
			}
			else{
				sol = inicial;
				rest_incumplidas = rest_orig;
			}
			evaluaciones++;
		}

		if(cont_evaluacion){
			continuar = false;
		}
		iteraciones++;
	}

	imprimirClusters();
	double infact = calcularInfactivilidad();
	double dist = calcularDistanciaIntraCluster();
	double agregado = dist+lambda*infact;


	cout<<"Distancia intracluster: "<<dist<<endl;
	cout<<"Infactivilidad: "<<infact<<endl;
	cout<<"Agregado: "<<agregado<<endl;

	cout<<"Iteraciones: "<<iteraciones<<endl;
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
}

void ARP::actualizarCentroides(){
	set<int> dato;
	for(int cluster=0; cluster<num_clusters; cluster++){
		vector<float> nuevo_centro = sol[cluster].devuelveCentroide();
		dato = sol[cluster].devuelveDatos();
		for(auto it_indie = dato.begin(); it_indie != dato.end();  it_indie++){
			for(int i=0; i<num_datos; i++){
				nuevo_centro[i] = (nuevo_centro[i] + datos[(*it_indie)][i])/2;
			}
		}
		sol[cluster].asignarCentroide(nuevo_centro);
	}
}

float ARP::calcularDistanciaIntraCluster(){
	float distanciaIntraCluster = 0;
	for(int numero_cluster=0; numero_cluster<num_clusters; numero_cluster++){
		set<int> cluster = sol[numero_cluster].devuelveDatos();
		for(auto it_indie = cluster.begin(); it_indie != cluster.end(); it_indie++){
			for(int i=0; i<num_datos; i++){
				distanciaIntraCluster += abs(sol[numero_cluster].devuelveCentroide()[i] - datos[(*it_indie)][i]);
			}
		}
		distanciaIntraCluster /= cluster.size();
	}
	return distanciaIntraCluster/num_clusters;
}
