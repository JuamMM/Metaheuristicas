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

int numero_corales;
int minimo;
int maximo;

int numero_datos;

int max_evaluaciones = 100000;
int n_clu;

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

Poblacion algoritmoCruceUN(Poblacion padre1, Poblacion padre2){
	vector<int> cromosomas;
	Poblacion hijo = padre1;
	int datos_asignados = 0;

	for(int i=0; i<datos.size(); i++){
		cromosomas.push_back(i);
	}

	random_shuffle(cromosomas.begin(), cromosomas.end());

	for(auto it_cro = cromosomas.begin(); it_cro != cromosomas.end(); it_cro++){
		int dato = cromosomas[(*it_cro)];
		if(datos_asignados<cromosomas.size()/2){
			hijo.asignaDato(dato,padre1.devuelveCluster(dato));
		}
		else{
			hijo.asignaDato(dato,padre2.devuelveCluster(dato));
		}
		datos_asignados++;
	}

	hijo.calcularValoracion();

	return hijo;
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

Poblacion CRO(PAR estado, int num_datos, int datos_centro, int num_clusters, int poblaciones, int min, int max){
	int rep_sexual;
	int criterio_parada = 100000, evaluaciones = 0, generaciones = 0;
	vector<int> corales;
	vector<int> indices;
	bool parar = false;
	int repeticion=0;
	float valoracion_ant = 0;
	vector<int> broadcast;
	vector<int> brooding;
	vector<Poblacion> hijos;

	for(int i =0; i<poblaciones;i++){
		indices.push_back(i);
	}

	//Calculamos el número de espacios ocupados en este caso siempre vamos a generar el mismo número de corales, un 60% de las casillas están ocupadas al empezar por corales
	int casillas_ocupadas = poblaciones*0.6;
	//Para indicar si una casilla está inicializada simplemente comprobamos si alguno de sus datos está en el cluster -1.
	for(int i=0; i<casillas_ocupadas;i++){
		estado.asignacionAleatoria(i);
		corales.push_back(i);
	}


	while(evaluaciones<criterio_parada && !parar){
		rep_sexual = corales.size()*0.9;

		//Seleccionamos los corales que generaran hijos de forma sexual
		while(broadcast.size()<rep_sexual){
			int dato_introducir = rand() % corales.size();
			broadcast.push_back(corales[dato_introducir]);
			corales.erase(corales.begin()+dato_introducir);
		}
		//y guardamos los que no para reproducción asexual
		for(int i=0; i< corales.size(); i++){
			brooding.push_back(corales[i]);
		}

		//seleccionamos las parejas de padres y creamos nuevos hijos
		while(broadcast.size()>1){
			int p1 = rand() % broadcast.size();
			Poblacion padre1 = estado.devuelvePoblacion(broadcast[p1]);
			broadcast.erase(broadcast.begin()+p1);

			int p2 = rand() % broadcast.size();
			Poblacion padre2 = estado.devuelvePoblacion(broadcast[p1]);
			broadcast.erase(broadcast.begin()+p2);

			Poblacion h1 = algoritmoCruceUN(padre1,padre2);
			Poblacion h2 = algoritmoCruceUN(padre2,padre1);

			hijos.push_back(h1);
			hijos.push_back(h2);
		}


		while(brooding.size()>0){
			int p = rand() % brooding.size();
			Poblacion h2 = estado.devuelvePoblacion(brooding[p]);
			h2.mutacionFuerte();
			hijos.push_back(h2);
			brooding.erase(brooding.begin()+p);
		}

		int lugar, intentos;
		bool asentado;
		float val_orig, val_larva;

		//Ahora con los hijos tratamos de introducirlos y guardamos el número máximo de intentos, en este caso 5.
		for(auto it = hijos.begin(); it != hijos.end(); it++){
			intentos = 5+generaciones*0.1;
			asentado = false;
			Poblacion larva = (*it);
			while(intentos>0 && !asentado){
				lugar = rand() % poblaciones;
				val_orig = estado.devuelvePoblacion(lugar).devolverValoracion();
				val_larva = larva.devolverValoracion();
				if(val_larva<val_orig || -1==val_orig){
					estado.sustituyePoblacion(lugar,larva);
					asentado = true;
					evaluaciones++;
				}
				else{
					intentos--;
				}
			}
		}


		//Se selecciona las mejores soluciones para que se reproduzcan

		vector<int> m_corales = estado.mejoresPadres(lambda,datos,restricciones,2);
		for(auto it_m = m_corales.begin(); it_m != m_corales.end(); it_m++){
			intentos = 5+generaciones*0.1;
			asentado = false;
			while(intentos>0 && !asentado){
				lugar = rand() % poblaciones;
				val_orig = estado.devuelvePoblacion(lugar).devolverValoracion();
				val_larva = estado.devuelvePoblacion((*it_m)).devolverValoracion();
				if(val_larva<val_orig || -1==val_orig){
					estado.sustituyePoblacion(lugar,estado.devuelvePoblacion((*it_m)));
					asentado = true;
					evaluaciones++;
				}
				else{
					intentos--;
				}
			}
		}

		//Introducimos la depredación en este caso se toma la mejor solución y cuando mayo sea la diferencia mayor probabilidad de morir
		int mejor_sol = estado.mejorCromosoma(lambda, datos, restricciones);
		float valoracion_mejor = estado.devuelvePoblacion(mejor_sol).devolverValoracion();

		Poblacion vacio(num_datos, datos_centro, num_clusters, min, max,restricciones,datos,lambda);
		//Recorremos la población eliminando poblaciones en función de su salud
		vector<int> p_sol = estado.peoresPadres(lambda,datos,restricciones,poblaciones*0.1);
		for(auto it_i=p_sol.begin(); it_i!=p_sol.end();it_i++){
			int i= (*it_i);
			float val_p = 0.5*generaciones;
			int p_muerte = rand() % 100;
			if(val_p > p_muerte){
				estado.sustituyePoblacion(i,vacio);
				evaluaciones++;
			}
		}

		//Ahora comprobamos que casillas son corales y las añadimos al vector
		corales.clear();
		for(int i=0; i<poblaciones;i++){
			if(0<(estado.devuelvePoblacion(i).devolverValoracion())){
				corales.push_back(i);
			}
		}

		if(compara(valoracion_mejor,valoracion_ant)){
			repeticion++;
		}
		else{
			repeticion = 0;
		}
		valoracion_ant = valoracion_mejor;

		if(repeticion == 30){
			parar=true;
		}
		cout<<"Mejor sol: "<<valoracion_mejor<<endl;

		evaluaciones++;
		broadcast.clear();
		brooding.clear();
		p_sol.clear();
		m_corales.clear();
		hijos.clear();
	}

	int mejor_sol = estado.mejorCromosoma(lambda,datos,restricciones);
	Poblacion pob_mejor = estado.devuelvePoblacion(mejor_sol);
	cout<<"Mejor solucion CRO"<<endl;
	int error = pob_mejor.calcularErrorGenerado();
	cout<<pob_mejor.desviacionGeneral()<<"\t";
	cout<<error<<"\t";
	cout<<pob_mejor.devolverValoracion()<<"\t";
	return pob_mejor;
}

Poblacion CRO_BL(PAR estado, int num_datos, int datos_centro, int num_clusters, int poblaciones, int min, int max){
	int rep_sexual;
	int criterio_parada = 100000, evaluaciones = 0, generaciones = 0;
	vector<int> corales;
	vector<int> indices;
	bool parar = false;
	int repeticion=0;
	float valoracion_ant = 0;
	vector<int> broadcast;
	vector<int> brooding;
	vector<Poblacion> hijos;

	for(int i =0; i<poblaciones;i++){
		indices.push_back(i);
	}

	//Calculamos el número de espacios ocupados en este caso siempre vamos a generar el mismo número de corales, un 60% de las casillas están ocupadas al empezar por corales
	int casillas_ocupadas = poblaciones*0.6;
	//Para indicar si una casilla está inicializada simplemente comprobamos si alguno de sus datos está en el cluster -1.
	for(int i=0; i<casillas_ocupadas;i++){
		estado.asignacionAleatoria(i);
		corales.push_back(i);
	}


	while(evaluaciones<criterio_parada && !parar){
		rep_sexual = corales.size()*0.9;

		//Seleccionamos los corales que generaran hijos de forma sexual
		while(broadcast.size()<rep_sexual){
			int dato_introducir = rand() % corales.size();
			broadcast.push_back(corales[dato_introducir]);
			corales.erase(corales.begin()+dato_introducir);
		}
		//y guardamos los que no para reproducción asexual
		for(int i=0; i< corales.size(); i++){
			brooding.push_back(corales[i]);
		}

		//seleccionamos las parejas de padres y creamos nuevos hijos
		while(broadcast.size()>1){
			int p1 = rand() % broadcast.size();
			Poblacion padre1 = estado.devuelvePoblacion(broadcast[p1]);
			broadcast.erase(broadcast.begin()+p1);

			int p2 = rand() % broadcast.size();
			Poblacion padre2 = estado.devuelvePoblacion(broadcast[p1]);
			broadcast.erase(broadcast.begin()+p2);

			Poblacion h1 = algoritmoCruceUN(padre1,padre2);
			Poblacion h2 = algoritmoCruceUN(padre2,padre1);

			hijos.push_back(h1);
			hijos.push_back(h2);
		}


		while(brooding.size()>0){
			int p = rand() % brooding.size();
			Poblacion h2 = estado.devuelvePoblacion(brooding[p]);
			h2.mutacionFuerte();
			hijos.push_back(h2);
			brooding.erase(brooding.begin()+p);
		}

		int lugar, intentos;
		bool asentado;
		float val_orig, val_larva;

		//Ahora con los hijos tratamos de introducirlos y guardamos el número máximo de intentos, en este caso 5.
		for(auto it = hijos.begin(); it != hijos.end(); it++){
			intentos = 5+generaciones*0.1;
			asentado = false;
			Poblacion larva = (*it);
			while(intentos>0 && !asentado){
				lugar = rand() % poblaciones;
				val_orig = estado.devuelvePoblacion(lugar).devolverValoracion();
				val_larva = larva.devolverValoracion();
				if(val_larva<val_orig || -1==val_orig){
					estado.sustituyePoblacion(lugar,larva);
					asentado = true;
					evaluaciones++;
				}
				else{
					intentos--;
				}
			}
		}


		//Se selecciona las mejores soluciones para que se reproduzcan

		vector<int> m_corales = estado.mejoresPadres(lambda,datos,restricciones,10);
		for(auto it_m = m_corales.begin(); it_m != m_corales.end(); it_m++){
			intentos = 5+generaciones*0.1;
			asentado = false;
			while(intentos>0 && !asentado){
				lugar = rand() % poblaciones;
				val_orig = estado.devuelvePoblacion(lugar).devolverValoracion();
				val_larva = estado.devuelvePoblacion((*it_m)).devolverValoracion();
				if(val_larva<val_orig || -1==val_orig){
					estado.sustituyePoblacion(lugar,estado.devuelvePoblacion((*it_m)));
					asentado = true;
					evaluaciones++;
				}
				else{
					intentos--;
				}
			}
		}

		//Introducimos la depredación en este caso se toma la mejor solución y cuando mayo sea la diferencia mayor probabilidad de morir

		Poblacion vacio(num_datos, datos_centro, num_clusters, min, max,restricciones,datos,lambda);
		//Recorremos la población eliminando poblaciones en función de su salud
		vector<int> p_sol = estado.peoresPadres(lambda,datos,restricciones,poblaciones*0.1);
		for(auto it_i=p_sol.begin(); it_i!=p_sol.end();it_i++){
			int i= (*it_i);
			float val_p = 0.1*generaciones;
			int p_muerte = rand() % 100;
			if(val_p > p_muerte){
				estado.sustituyePoblacion(i,vacio);
				evaluaciones++;
			}
		}

		//Ahora comprobamos que casillas son corales y las añadimos al vector
		corales.clear();
		for(int i=0; i<poblaciones;i++){
			if(0<(estado.devuelvePoblacion(i).devolverValoracion())){
				corales.push_back(i);
			}
		}

		if(generaciones % 10 == 0){
			estado.BLsuaveMejores(lambda,restricciones,datos,poblaciones*0.1);
		}

		int mejor_sol = estado.mejorCromosoma(lambda, datos, restricciones);
		float valoracion_mejor = estado.devuelvePoblacion(mejor_sol).devolverValoracion();
		cout<<"Mejor sol: "<<valoracion_mejor<<endl;

		if(compara(valoracion_mejor,valoracion_ant)){
			repeticion++;
		}
		else{
			repeticion = 0;
		}
		valoracion_ant = valoracion_mejor;

		if(repeticion == 30){
			parar=true;
		}

		evaluaciones++;
		broadcast.clear();
		brooding.clear();
		p_sol.clear();
		m_corales.clear();
		hijos.clear();
	}

	int mejor_sol = estado.mejorCromosoma(lambda,datos,restricciones);
	Poblacion pob_mejor = estado.devuelvePoblacion(mejor_sol);
	cout<<"Mejor solucion CRO-BL"<<endl;
	int error = pob_mejor.calcularErrorGenerado();
	cout<<pob_mejor.desviacionGeneral()<<"\t";
	cout<<error<<"\t";
	cout<<pob_mejor.devolverValoracion()<<"\t";
	return pob_mejor;
}

int main(int argc, char **argv){
	if(argc < 5){
		cout<<"La forma de uso de este programa es: ./P3 <numero de clusters> <poblaciones> <path a los datos> <path a las restricciones> <semilla>"<<endl;
	}
	else{
		int clusters = stoi(argv[1]);
		n_clu = clusters;
		leerDatos(argv[3]);
		leerRestricciones(argv[4]);
		numero_corales = stoi(argv[2]);
		unsigned semilla = stoul(argv[5]);

		srand(semilla);

		minimo = leerMaximoMinimo().first;
		maximo = leerMaximoMinimo().second;
		numero_datos = datos.size();


		datos_centro = datos[0].size();
		calculaLambda();
		PAR problema1(numero_corales, clusters, datos_centro, numero_datos, minimo, maximo, restricciones, datos, lambda);
		PAR problema2(numero_corales, clusters, datos_centro, numero_datos, minimo, maximo, restricciones, datos, lambda);

		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;

		start_timers();
		CRO(problema1, numero_datos, datos_centro, clusters, numero_corales, minimo, maximo);
		cout<<elapsed_time()<<endl;

		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;

		start_timers();
		CRO_BL(problema2, numero_datos, datos_centro, clusters, numero_corales, minimo, maximo);
		cout<<elapsed_time()<<endl;
		cout<<"<--------------------------------------------------------------------------------------------->"<<endl;

	}
}
