#include <vector>
#include <string>
#include <map>

#include "Cluster.h"

using namespace std;

class ARP{
	private:
		vector<Cluster> sol;
		vector<vector<float>> datos;
		map<pair<int,int>,int> restricciones;
		int num_clusters;
		vector<int> indices;
		int num_datos, num_restricciones;
		int minimo, maximo;
		double lambda;

		void leerDatos(string fichero);
		void leerRestricciones(string fichero);

	public:
		ARP(string fich_datos, string fich_restricciones, int cluster);
		void Greedy(unsigned semilla);
		void imprimirClusters();
		void imprimirDatos();
		void imprimirRestricciones();
		int valoracionGreedy(vector<Cluster> clusters);
		double calcularErrorGenerado(int dato_int, int numero_cluster);
		void eliminarDatos();
		void generarCentroides(int min, int max);
		void BL(unsigned semilla);
		int buscaCluster(int dato);
		double calcularInfactivilidad();
		void intercambiaDato(int dato, int cluster_nuevo, int cluster_orig);
		void actualizarCentroides();
		float calcularDistanciaIntraCluster();
};
