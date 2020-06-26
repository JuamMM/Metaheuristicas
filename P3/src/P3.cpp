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
float temp_ini;
float temp_fin = 0.001;

float beta;
float mu = 0.3;
float phi = 0.3;
int max_vecinos;
int enfriamientos;
int max_evaluaciones = 100000;
float max_exitos;

void calcularTemperaturaIni(Poblacion pob){
	temp_ini = (mu*pob.calcularErrorGenerado(restricciones))/(-1*log(phi));
}

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

	while(!getline(fichero,cadena,'\n').eof() && val1 < datos.size()){
		istringstream reader(cadena);

		while(!getline(reader,dato,',').eof() && val2 < datos.size()){
			if(stod(dato) != 0 && val2 > val1){
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
	int error = pob.calcularErrorGenerado(restricciones);
	cout<<"Desviacion General: "<<pob.desviacionGeneral(datos)<<endl;
	cout<<"Error: "<<error<<endl;
	cout<<"Valoracion: "<<pob.desviacionGeneral(datos)+lambda*error<<endl;
	pob.imprimePoblacion();

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

Poblacion BLSobrePoblacion(int num_datos, int num_clusters, int min, int max, Poblacion pob){
	int iteraciones = 0, evaluaciones = 0;
	int error;

	error = pob.calcularErrorGenerado(restricciones);
	pob.actualizarCentroides(datos);
	float valoracion = pob.desviacionGeneral(datos)+lambda*error;

	vector<int> indices;
	for(int i=0; i<num_datos; i++){
		indices.push_back(i);
	}

	bool continuar = true;

	while(continuar && evaluaciones < 100000){
		bool cont_evaluacion = true;

		random_shuffle(indices.begin(), indices.end());

		for(auto it = indices.begin(); it != indices.end() && cont_evaluacion; it++){
			int dato = (*it);
			int clust_orig = pob.devuelveCluster(dato);

			for(int clust_nuevo=0; clust_nuevo<num_clusters && cont_evaluacion; clust_nuevo++){
				int rest_nueva = error;

				pob.asignaDato(dato,-1);

				rest_nueva -= pob.calcularErrorParcial(dato,clust_orig,restricciones);
				rest_nueva += pob.calcularErrorParcial(dato,clust_nuevo,restricciones);

				pob.asignaDato(dato,clust_nuevo);

				pob.actualizarCentroides(datos);

				float valoracion_nueva = pob.desviacionGeneral(datos)+lambda*rest_nueva;

				if(valoracion > valoracion_nueva){
					valoracion = valoracion_nueva;
					error = pob.calcularErrorGenerado(restricciones);
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
	return pob;
}

Poblacion BL(int num_datos, int num_clusters, int min, int max){
	Poblacion pob(num_datos, datos_centro, num_clusters, min, max);
	int iteraciones = 0, evaluaciones = 0;
	int error;

	pob.asignacionAleatoria();

	error = pob.calcularErrorGenerado(restricciones);
	pob.actualizarCentroides(datos);
	float valoracion = pob.desviacionGeneral(datos)+lambda*error;

	vector<int> indices;
	for(int i=0; i<num_datos; i++){
		indices.push_back(i);
	}

	bool continuar = true;

	while(continuar && evaluaciones < 100000){
		bool cont_evaluacion = true;

		random_shuffle(indices.begin(), indices.end());

		for(auto it = indices.begin(); it != indices.end() && cont_evaluacion; it++){
			int dato = (*it);
			int clust_orig = pob.devuelveCluster(dato);

			for(int clust_nuevo=0; clust_nuevo<num_clusters && cont_evaluacion; clust_nuevo++){
				int rest_nueva = error;

				pob.asignaDato(dato,-1);

				rest_nueva -= pob.calcularErrorParcial(dato,clust_orig,restricciones);
				rest_nueva += pob.calcularErrorParcial(dato,clust_nuevo,restricciones);

				pob.asignaDato(dato,clust_nuevo);

				pob.actualizarCentroides(datos);

				float valoracion_nueva = pob.desviacionGeneral(datos)+lambda*rest_nueva;

				if(valoracion > valoracion_nueva){
					valoracion = valoracion_nueva;
					error = pob.calcularErrorGenerado(restricciones);
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

	int error_max = pob.calcularErrorGenerado(restricciones);
	cout<<"Desviacion General: "<<pob.desviacionGeneral(datos)<<endl;
	cout<<"Error: "<<error_max<<endl;
	cout<<"Valoracion: "<<pob.desviacionGeneral(datos)+lambda*error_max<<endl;
	pob.imprimePoblacion();

	cout<<"Iteraciones: "<<iteraciones<<endl;
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	return pob;
}

void Memetico(PAR estado, int num_datos, int datos_centro, int num_clusters, int poblaciones, int min, int max){
	float prob_cruce = 0.7;
	int prob_muta = 1, rango_muta = 1000;
	float valoracion = 10000000;
	int criterio_parada = 100000, evaluaciones = 0, generaciones = 0;

	int numero_cruces = (poblaciones/2)*prob_cruce;

	Poblacion mejor_solucion(num_datos, datos_centro, num_clusters, min, max);

	while(evaluaciones<criterio_parada){
		bool tiene_mejor_sol = false;

		mejor_solucion = estado.devuelvePoblacion(estado.mejorCromosoma(lambda,datos,restricciones));

		estado.algoritmoSeleccionador(lambda,datos,restricciones);

		for(int i=0; i<numero_cruces; i++){
			int padre1 = rand() % poblaciones;
			int padre2 = padre1;

			while(padre2 == padre1){
				padre2 = rand() % poblaciones;
			}

			Poblacion hijo1(num_datos, datos_centro, num_clusters, min, max);
			hijo1 = estado.algoritmoCruceUN(padre1,padre2);

			Poblacion hijo2(num_datos, datos_centro, num_clusters, min, max);
			hijo2= estado.algoritmoCruceUN(padre2,padre1);

			estado.sustituyePoblacion(padre1,hijo1);
			estado.sustituyePoblacion(padre2,hijo2);
			evaluaciones++;
		}

		for(int i=0;i<poblaciones;i++){
			estado.algoritmoMutacion(i,prob_muta,rango_muta);
			evaluaciones++;
		}

		for(int i=0; i<poblaciones; i++){
			if(mejor_solucion == estado.devuelvePoblacion(i)){
				tiene_mejor_sol = true;
			}
		}

		estado.reparacion();

		if(!tiene_mejor_sol){
			int peor = estado.peorCromosoma(lambda,restricciones,datos);

			estado.sustituyePoblacion(peor,mejor_solucion);
		}

		for(int i=0; i<poblaciones;i++){
			int error = estado.devuelvePoblacion(i).calcularErrorGenerado(restricciones);
			float valoracion_nueva = estado.devuelvePoblacion(i).desviacionGeneral(datos)+lambda*error;
			if(valoracion > valoracion_nueva){
				valoracion = valoracion_nueva;
			}
		}

		if(generaciones % 10 == 0){
			estado.BLsuaveMejores(lambda,restricciones,datos,5);
		}

		generaciones++;
	}

	int mejor = estado.mejorCromosoma(lambda,datos,restricciones);
	cout<<"Mejor solucion Memetico"<<endl;
	int error = estado.devuelvePoblacion(mejor).calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)+lambda*error<<endl;
	estado.devuelvePoblacion(mejor).imprimePoblacion();
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	cout<<"Generaciones: "<<generaciones<<endl;
}

void AGG_UN(PAR estado, int num_datos, int datos_centro, int num_clusters, int poblaciones, int min, int max){
	float prob_cruce = 0.7;
	int prob_muta = 1, rango_muta = 1000;
	float valoracion = 10000000;
	int criterio_parada = 100000, evaluaciones = 0, generaciones = 0;

	int numero_cruces = (poblaciones/2)*prob_cruce;

	Poblacion mejor_solucion(num_datos, datos_centro, num_clusters, min, max);

	while(evaluaciones<criterio_parada){
		bool tiene_mejor_sol = false;

		mejor_solucion = estado.devuelvePoblacion(estado.mejorCromosoma(lambda,datos,restricciones));

		estado.algoritmoSeleccionador(lambda,datos,restricciones);

		for(int i=0; i<numero_cruces; i++){
			int padre1 = rand() % poblaciones;
			int padre2 = padre1;

			while(padre2 == padre1){
				padre2 = rand() % poblaciones;
			}

			Poblacion hijo1(num_datos, datos_centro, num_clusters, min, max);
			hijo1 = estado.algoritmoCruceUN(padre1,padre2);

			Poblacion hijo2(num_datos, datos_centro, num_clusters, min, max);
			hijo2= estado.algoritmoCruceUN(padre2,padre1);

			estado.sustituyePoblacion(padre1,hijo1);
			estado.sustituyePoblacion(padre2,hijo2);
			evaluaciones++;
		}

		for(int i=0;i<poblaciones;i++){
			estado.algoritmoMutacion(i,prob_muta,rango_muta);
			evaluaciones++;
		}

		for(int i=0; i<poblaciones; i++){
			if(mejor_solucion == estado.devuelvePoblacion(i)){
				tiene_mejor_sol = true;
			}
		}

		estado.reparacion();

		if(!tiene_mejor_sol){
			int peor = estado.peorCromosoma(lambda,restricciones,datos);

			estado.sustituyePoblacion(peor,mejor_solucion);
		}

		for(int i=0; i<poblaciones;i++){
			int error = estado.devuelvePoblacion(i).calcularErrorGenerado(restricciones);
			float valoracion_nueva = estado.devuelvePoblacion(i).desviacionGeneral(datos)+lambda*error;
			if(valoracion > valoracion_nueva){
				valoracion = valoracion_nueva;
			}
		}

		generaciones++;
	}

	int mejor = estado.mejorCromosoma(lambda,datos,restricciones);
	cout<<"Mejor solucion AGG-UN"<<endl;
	int error = estado.devuelvePoblacion(mejor).calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)+lambda*error<<endl;
	estado.devuelvePoblacion(mejor).imprimePoblacion();
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	cout<<"Generaciones: "<<generaciones<<endl;
}

void AGG_SF(PAR estado, int num_datos, int datos_centro, int num_clusters, int poblaciones, int min, int max){
	float prob_cruce = 0.7;
	int prob_muta = 1, rango_muta = 1000;
	float valoracion = 10000000;
	int criterio_parada = 100000, evaluaciones = 0, generaciones = 0;

	int numero_cruces = (poblaciones/2)*prob_cruce;

	Poblacion mejor_solucion(num_datos, datos_centro, num_clusters, min, max);

	while(evaluaciones<criterio_parada){
		bool tiene_mejor_sol = false;

		mejor_solucion = estado.devuelvePoblacion(estado.mejorCromosoma(lambda,datos,restricciones));

		estado.algoritmoSeleccionador(lambda,datos,restricciones);

		for(int i=0; i<numero_cruces; i++){
			int padre1 = rand() % poblaciones;
			int padre2 = padre1;

			while(padre2 == padre1){
				padre2 = rand() % poblaciones;
			}

			Poblacion hijo1(num_datos, datos_centro, num_clusters, min, max);
			hijo1 = estado.algoritmoCruceSF(padre1,padre2);

			Poblacion hijo2(num_datos, datos_centro, num_clusters, min, max);
			hijo2= estado.algoritmoCruceSF(padre2,padre1);

			estado.sustituyePoblacion(padre1,hijo1);
			estado.sustituyePoblacion(padre2,hijo2);
			evaluaciones++;
		}

		for(int i=0;i<poblaciones;i++){
			estado.algoritmoMutacion(i,prob_muta,rango_muta);
			evaluaciones++;
		}

		for(int i=0; i<poblaciones; i++){
			if(mejor_solucion == estado.devuelvePoblacion(i)){
				tiene_mejor_sol = true;
			}
		}

		estado.reparacion();

		if(!tiene_mejor_sol){
			int peor = estado.peorCromosoma(lambda,restricciones,datos);

			estado.sustituyePoblacion(peor,mejor_solucion);
		}

		for(int i=0; i<poblaciones;i++){
			int error = estado.devuelvePoblacion(i).calcularErrorGenerado(restricciones);
			float valoracion_nueva = estado.devuelvePoblacion(i).desviacionGeneral(datos)+lambda*error;
			if(valoracion > valoracion_nueva){
				valoracion = valoracion_nueva;
			}
		}

		generaciones++;
	}

	int mejor = estado.mejorCromosoma(lambda,datos,restricciones);
	cout<<"Mejor solucion AGG-SF"<<endl;
	int error = estado.devuelvePoblacion(mejor).calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)+lambda*error<<endl;
	estado.devuelvePoblacion(mejor).imprimePoblacion();
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	cout<<"Generaciones: "<<generaciones<<endl;
}

void AGE_SF(PAR estado, int num_datos, int datos_centro, int num_clusters, int poblaciones, int min, int max){
	int prob_muta = 1, rango_muta = 1000;
	float valoracion = 10000000;
	int criterio_parada = 100000, evaluaciones = 0, generaciones = 0;

	Poblacion mejor_solucion(num_datos, datos_centro, num_clusters, min, max);

	while(evaluaciones<criterio_parada){
		bool tiene_mejor_sol = false;

		mejor_solucion = estado.devuelvePoblacion(estado.mejorCromosoma(lambda,datos,restricciones));

		estado.algoritmoSeleccionador(lambda,datos,restricciones);

		vector<int> padres = estado.mejoresPadres(lambda,datos,restricciones,2);

		Poblacion hijo1(num_datos, datos_centro, num_clusters, min, max);
		hijo1 = estado.algoritmoCruceSF(padres[0],padres[1]);

		Poblacion hijo2(num_datos, datos_centro, num_clusters, min, max);
		hijo2= estado.algoritmoCruceSF(padres[1],padres[0]);

		estado.sustituirPeoresPadres(lambda,datos,restricciones,hijo1,hijo2);

		evaluaciones++;

		for(int i=0;i<poblaciones;i++){
			estado.algoritmoMutacion(i,prob_muta,rango_muta);
			evaluaciones++;
		}

		for(int i=0; i<poblaciones; i++){
			if(mejor_solucion == estado.devuelvePoblacion(i)){
				tiene_mejor_sol = true;
			}
		}

		estado.reparacion();

		if(!tiene_mejor_sol){
			int peor = estado.peorCromosoma(lambda,restricciones,datos);

			estado.sustituyePoblacion(peor,mejor_solucion);
		}

		for(int i=0; i<poblaciones;i++){
			int error = estado.devuelvePoblacion(i).calcularErrorGenerado(restricciones);
			float valoracion_nueva = estado.devuelvePoblacion(i).desviacionGeneral(datos)+lambda*error;
			if(valoracion > valoracion_nueva){
				valoracion = valoracion_nueva;
			}
		}

		generaciones++;
	}

	int mejor = estado.mejorCromosoma(lambda,datos,restricciones);
	cout<<"Mejor solucion AGE-SF"<<endl;
	int error = estado.devuelvePoblacion(mejor).calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)+lambda*error<<endl;
	estado.devuelvePoblacion(mejor).imprimePoblacion();
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	cout<<"Generaciones: "<<generaciones<<endl;
}

void AGE_UN(PAR estado, int num_datos, int datos_centro, int num_clusters, int poblaciones, int min, int max){
	int prob_muta = 1, rango_muta = 1000;
	float valoracion = 10000000;
	int criterio_parada = 100000, evaluaciones = 0, generaciones = 0;

	Poblacion mejor_solucion(num_datos, datos_centro, num_clusters, min, max);

	while(evaluaciones<criterio_parada){
		bool tiene_mejor_sol = false;

		mejor_solucion = estado.devuelvePoblacion(estado.mejorCromosoma(lambda,datos,restricciones));

		estado.algoritmoSeleccionador(lambda,datos,restricciones);

		vector<int> padres = estado.mejoresPadres(lambda,datos,restricciones,2);

		Poblacion hijo1(num_datos, datos_centro, num_clusters, min, max);
		hijo1 = estado.algoritmoCruceUN(padres[0],padres[1]);

		Poblacion hijo2(num_datos, datos_centro, num_clusters, min, max);
		hijo2= estado.algoritmoCruceUN(padres[1],padres[0]);

		estado.sustituirPeoresPadres(lambda,datos,restricciones,hijo1,hijo2);

		evaluaciones++;

		for(int i=0;i<poblaciones;i++){
			estado.algoritmoMutacion(i,prob_muta,rango_muta);
			evaluaciones++;
		}

		for(int i=0; i<poblaciones; i++){
			if(mejor_solucion == estado.devuelvePoblacion(i)){
				tiene_mejor_sol = true;
			}
		}

		estado.reparacion();

		if(!tiene_mejor_sol){
			int peor = estado.peorCromosoma(lambda,restricciones,datos);

			estado.sustituyePoblacion(peor,mejor_solucion);
		}

		for(int i=0; i<poblaciones;i++){
			int error = estado.devuelvePoblacion(i).calcularErrorGenerado(restricciones);
			float valoracion_nueva = estado.devuelvePoblacion(i).desviacionGeneral(datos)+lambda*error;
			if(valoracion > valoracion_nueva){
				valoracion = valoracion_nueva;
			}
		}

		generaciones++;
	}

	int mejor = estado.mejorCromosoma(lambda,datos,restricciones);
	cout<<"Mejor solucion AGE-UN"<<endl;
	int error = estado.devuelvePoblacion(mejor).calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<estado.devuelvePoblacion(mejor).desviacionGeneral(datos)+lambda*error<<endl;
	estado.devuelvePoblacion(mejor).imprimePoblacion();
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	cout<<"Generaciones: "<<generaciones<<endl;
}

Poblacion ES(int num_datos, int num_clusters, int min, int max){
	Poblacion pob(num_datos, datos_centro, num_clusters, min, max);

	pob.asignacionAleatoria();
	pob.actualizarCentroides(datos);
	calcularTemperaturaIni(pob);
	beta = (temp_ini - temp_fin)/(enfriamientos*temp_ini*temp_fin);
	int sol_aceptadas=1;
	float temperatura=temp_ini;

	while(temperatura >= temp_fin && sol_aceptadas>0){
		sol_aceptadas=0;
		int errores = pob.calcularErrorGenerado(restricciones);
		double valoracion_ini = pob.desviacionGeneral(datos)+lambda*errores;

		for(int vecino=0; vecino<max_vecinos && sol_aceptadas<max_exitos;vecino++){
			int error_nuevo = errores;
			double valoracion_nueva;
			int dato = rand() % num_datos;
			int cluster_orig = pob.devuelveCluster(dato);
			int cluster_nuevo = rand() % num_clusters;

			pob.asignaDato(dato,-1);

			error_nuevo -= pob.calcularErrorParcial(dato,cluster_orig,restricciones);
			error_nuevo += pob.calcularErrorParcial(dato,cluster_nuevo,restricciones);

			pob.asignaDato(dato,cluster_nuevo);
			pob.actualizarCentroides(datos);

			valoracion_nueva = pob.desviacionGeneral(datos)+lambda*error_nuevo;

			double val = valoracion_nueva - valoracion_ini;

			pob.Reparacion();

			float U = rand() % 2;

			if(val < 0 || U <= -1*exp(val*temperatura)){
				valoracion_ini = valoracion_nueva;
				sol_aceptadas++;
			}
			else{
				pob.asignaDato(dato,cluster_orig);
				pob.actualizarCentroides(datos);
			}
		}
		temperatura = temperatura/(1+beta*temperatura);
	}
	cout<<"Mejor solucion ES"<<endl;
	int error = pob.calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<pob.desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<pob.desviacionGeneral(datos)+lambda*error<<endl;
	pob.imprimePoblacion();

	return pob;
}

Poblacion ESSobrePoblacion(int num_datos, int num_clusters, Poblacion pob){

	pob.actualizarCentroides(datos);
	calcularTemperaturaIni(pob);
	beta = (temp_ini - temp_fin)/(enfriamientos*temp_ini*temp_fin);
	int sol_aceptadas=1;
	float temperatura=temp_ini;

	while(temperatura >= temp_fin && sol_aceptadas>0){
		sol_aceptadas=0;
		int errores = pob.calcularErrorGenerado(restricciones);
		double valoracion_ini = pob.desviacionGeneral(datos)+lambda*errores;

		for(int vecino=0; vecino<max_vecinos && sol_aceptadas<max_exitos;vecino++){
			int error_nuevo = errores;
			double valoracion_nueva;
			int dato = rand() % num_datos;
			int cluster_orig = pob.devuelveCluster(dato);
			int cluster_nuevo = rand() % num_clusters;

			pob.asignaDato(dato,-1);

			error_nuevo -= pob.calcularErrorParcial(dato,cluster_orig,restricciones);
			error_nuevo += pob.calcularErrorParcial(dato,cluster_nuevo,restricciones);

			pob.asignaDato(dato,cluster_nuevo);
			pob.actualizarCentroides(datos);

			valoracion_nueva = pob.desviacionGeneral(datos)+lambda*error_nuevo;

			double val = valoracion_nueva - valoracion_ini;

			pob.Reparacion();

			float U = rand() % 2;

			if(val < 0 || U <= -1*exp(val*temperatura)){
				valoracion_ini = valoracion_nueva;
				sol_aceptadas++;
			}
			else{
				pob.asignaDato(dato,cluster_orig);
				pob.actualizarCentroides(datos);
			}
		}
		temperatura = temperatura/(1+beta*temperatura);
	}

	return pob;
}

Poblacion ILS(int num_datos, int num_clusters, int min, int max){
	Poblacion pob_mejor(num_datos, datos_centro, num_clusters, min, max);
	double val_mejor, val_nueva;

	pob_mejor.asignacionAleatoria();
	pob_mejor = BLSobrePoblacion(num_datos, num_clusters, min, max, pob_mejor);
	pob_mejor.actualizarCentroides(datos);

	val_mejor = pob_mejor.desviacionGeneral(datos)+lambda*pob_mejor.calcularErrorGenerado(restricciones);
	for(int i=0; i<9; i++){
		Poblacion pob_nueva(num_datos, datos_centro, num_clusters, min, max);

		pob_nueva = pob_mejor;

		pob_nueva.mutacionFuerte();

		pob_nueva = BLSobrePoblacion(num_datos,num_clusters,min,max, pob_nueva);
		pob_nueva.actualizarCentroides(datos);

		val_nueva = pob_nueva.desviacionGeneral(datos)+lambda*pob_nueva.calcularErrorGenerado(restricciones);

		if(val_mejor >= val_nueva){
			pob_mejor = pob_nueva;
		}
	}

	cout<<"Mejor solucion ILS"<<endl;
	int error = pob_mejor.calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<pob_mejor.desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<pob_mejor.desviacionGeneral(datos)+lambda*error<<endl;
	pob_mejor.imprimePoblacion();

	return pob_mejor;
}

Poblacion ILS_ES(int num_datos, int num_clusters, int min, int max){
	Poblacion pob_mejor(num_datos, datos_centro, num_clusters, min, max);
	double val_mejor, val_nueva;
	int evaluaciones=0;

	pob_mejor.asignacionAleatoria();
	pob_mejor = ESSobrePoblacion(num_datos, num_clusters, pob_mejor);
	pob_mejor.actualizarCentroides(datos);

	val_mejor = pob_mejor.desviacionGeneral(datos)+lambda*pob_mejor.calcularErrorGenerado(restricciones);
	for(int i=0; i<9 && evaluaciones < 10000; i++){
		evaluaciones++;
		Poblacion pob_nueva(num_datos, datos_centro, num_clusters, min, max);

		pob_nueva = pob_mejor;

		pob_nueva.mutacionFuerte();
		//ES

		pob_nueva.actualizarCentroides(datos);
		calcularTemperaturaIni(pob_nueva);
		beta = (temp_ini - temp_fin)/(enfriamientos*temp_ini*temp_fin);
		int sol_aceptadas=1;
		float temperatura=temp_ini;

		while(temperatura >= temp_fin && sol_aceptadas>0 && evaluaciones <10000){
			sol_aceptadas=0;
			int errores = pob_nueva.calcularErrorGenerado(restricciones);
			double valoracion_ini = pob_nueva.desviacionGeneral(datos)+lambda*errores;

			for(int vecino=0; vecino<max_vecinos && sol_aceptadas<max_exitos;vecino++){
				int error_nuevo = errores;
				double valoracion_nueva;
				int dato = rand() % num_datos;
				int cluster_orig = pob_nueva.devuelveCluster(dato);
				int cluster_nuevo = rand() % num_clusters;

				pob_nueva.asignaDato(dato,-1);

				error_nuevo -= pob_nueva.calcularErrorParcial(dato,cluster_orig,restricciones);
				error_nuevo += pob_nueva.calcularErrorParcial(dato,cluster_nuevo,restricciones);

				pob_nueva.asignaDato(dato,cluster_nuevo);
				pob_nueva.actualizarCentroides(datos);

				valoracion_nueva = pob_nueva.desviacionGeneral(datos)+lambda*error_nuevo;

				double val = valoracion_nueva - valoracion_ini;

				pob_nueva.Reparacion();

				float U = rand() % 2;

				if(val < 0 || U <= -1*exp(val*temperatura)){
					valoracion_ini = valoracion_nueva;
					sol_aceptadas++;
				}
				else{
					pob_nueva.asignaDato(dato,cluster_orig);
					pob_nueva.actualizarCentroides(datos);
				}
				evaluaciones++;
			}
			temperatura = temperatura/(1+beta*temperatura);
		}

		//NO ES

		val_nueva = pob_nueva.desviacionGeneral(datos)+lambda*pob_nueva.calcularErrorGenerado(restricciones);

		if(val_mejor >= val_nueva){
			pob_mejor = pob_nueva;
		}
	}

	cout<<"Mejor solucion ILS-ES"<<endl;
	int error = pob_mejor.calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<pob_mejor.desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<pob_mejor.desviacionGeneral(datos)+lambda*error<<endl;
	cout<<"Evaluaciones: "<<evaluaciones<<endl;
	pob_mejor.imprimePoblacion();

	return pob_mejor;
}

Poblacion BMB(int num_datos, int num_clusters, int min, int max, int numero_poblaciones){
	Poblacion pob_mejor(num_datos, datos_centro, num_clusters, min, max);
	Poblacion pob(num_datos, datos_centro, num_clusters, min, max);
	double val_mejor = 10000, val_nueva;

	for(int i=0;i<numero_poblaciones;i++){
		pob.asignacionAleatoria();
		pob = BLSobrePoblacion(num_datos, num_clusters, min, max, pob);
		pob.actualizarCentroides(datos);

		val_nueva = pob.desviacionGeneral(datos)+lambda*pob.calcularErrorGenerado(restricciones);

		if(val_mejor > val_nueva){
			pob_mejor = pob;
			val_mejor = val_nueva;
		}
	}

	cout<<"Mejor solucion BMB"<<endl;
	int error = pob_mejor.calcularErrorGenerado(restricciones);
	cout<<"Error: "<<error<<endl;
	cout<<"Desviacion General: "<<pob_mejor.desviacionGeneral(datos)<<endl;
	cout<<"Valoracion: "<<pob_mejor.desviacionGeneral(datos)+lambda*error<<endl;
	pob_mejor.imprimePoblacion();

	return pob_mejor;
}

int main(int argc, char **argv){
	if(argc < 5){
		cout<<"La forma de uso de este programa es: ./P3 <numero de clusters> <path a los datos> <path a las resctricciones> <semilla>"<<endl;
	}
	else{
		int clusters = stoi(argv[1]);
		leerDatos(argv[2]);
		leerRestricciones(argv[3]);
		unsigned semilla = stoul(argv[4]);

		srand(semilla);

		int minimo = leerMaximoMinimo().first;
		int maximo = leerMaximoMinimo().second;
		int numero_datos = datos.size();

		max_vecinos = 10*numero_datos;
		max_exitos = 0.1*max_vecinos;
		enfriamientos = pow(max_evaluaciones/max_vecinos,2);

		datos_centro = datos[0].size();
		calculaLambda();

		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;

		start_timers();
		BMB(numero_datos,clusters,minimo,maximo,10);
		cout<<"Tiempo: "<<elapsed_time()<<endl;

		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;

		start_timers();
		ILS_ES(numero_datos,clusters,minimo,maximo);
		cout<<"Tiempo: "<<elapsed_time()<<endl;

		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;

		start_timers();
		ILS(numero_datos,clusters,minimo,maximo);
		cout<<"Tiempo ILS: "<<elapsed_time()<<endl;

		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;

		start_timers();
		ES(numero_datos,clusters,minimo,maximo);
		cout<<"Tiempo: "<<elapsed_time()<<endl;

		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;
	}


}
