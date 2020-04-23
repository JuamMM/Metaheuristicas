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

	public:
		Poblacion(int max_datos, int datos_centro, int clusters, int min, int max);

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

		int calcularErrorGenerado(list<tuple<int,int,double>> rest);

		int calcularErrorParcial(int dato, int cluster1, list<tuple<int,int,double>> restricciones);

		void asignacionAleatoria();

		void actualizarCentroides(vector<vector<float>> centros);

		float distanciaIntraCluster(vector<vector<float>> centros, int cluster);

		float desviacionGeneral(vector<vector<float>> datos);

		void vaciarClusters(){
			vector<int> vacio(num_datos,-1);
			datos = datos;
		}

		bool operator== (Poblacion comp){
			return comp.devuelveDatos() == datos;
		}
};
