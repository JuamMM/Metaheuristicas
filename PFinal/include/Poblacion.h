#include <iostream>
#include <list>
#include <tuple>
#include <stdlib.h>
#include <vector>

using namespace std;

class Poblacion{
	private:
		vector<vector<float>> centroides;
		vector<int> datos;
		int num_datos;
		int num_clusters;
		vector<int> tam;
		float valoracion;
		list<tuple<int,int,double>> restricciones;
		vector<vector<float>> centros;
		float lambda;


	public:
		Poblacion(int max_datos, int datos_centro, int clusters, int min, int max, list<tuple<int,int,double>> rest, vector<vector<float>> dat, float l);

		void asignaDato(int dato, int cluster);

		void imprimePoblacion();

		vector<float> devuelveCentroide(int indice){
			return centroides[indice];
		}

		int tamCluster(int indice){
			return tam[indice];
		}

		vector<int> devuelveDatos(){
			return datos;
		}

		int devuelveCluster(int indice){
			return datos[indice];
		}

		int tamanio(){
			return datos.size();
		}

		int calcularErrorGenerado();

		int calcularErrorParcial(int dato, int cluster1);

		void asignacionAleatoria();

		void actualizarCentroides();

		void Reparacion();

		float distanciaIntraCluster(int cluster);

		float desviacionGeneral();

		void vaciarClusters(){
			vector<int> vacio(num_datos,-1);
			datos = datos;
		}

		bool operator== (Poblacion comp){
			return comp.devuelveDatos() == datos;
		}

		void mutacionFuerte();

		void calcularValoracion();

		void asignarValoracion(float val){
			valoracion=val;
		}

		float devolverValoracion(){
			return valoracion;
		}
};
