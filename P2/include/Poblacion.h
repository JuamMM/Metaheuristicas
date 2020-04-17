#include <iostream>
#include <vector>
#include <set>

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

		int devuelveCluster(int indice){
			return datos[indice];
		}
		int tamanio(){
			return datos.size();
		}

		void actualizarCentroides(vector<vector<float>> centros);

		float distanciaIntraCluster(vector<vector<float>> centros, int cluster);

		float desviacionGeneral(vector<vector<float>> datos);

		void vaciarClusters(){
			vector<int> vacio(num_datos,-1);
			datos = datos;
		}
};
