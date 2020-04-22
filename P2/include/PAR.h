#include <vector>
#include <list>
#include <tuple>
#include <string>
#include <map>

#include <Poblacion.h>

using namespace std;

class PAR{
	private:
		vector<Poblacion> poblaciones;
		int num_poblaciones;
		int tam;
		int num_clusters;
		int tam_centro;
		int minimo;
		int maximo;

	public:
		PAR(int poblaciones, int clusters, int centros, int num_datos, int min, int max){
			tam = num_datos;
			num_poblaciones = poblaciones;
			num_clusters = clusters;
			minimo = min;
			maximo = max;
			tam_centro = centros;
			for(int i=0; i<num_poblaciones; i++){
				Poblacion pob(num_datos, centros, clusters, min, max);
				this->poblaciones.push_back(pob);
			}
		}

		Poblacion devuelvePoblacion(int indice){
			return poblaciones[indice];
		}

		void generarPoblacionesAleatorias(int num_datos);

		void imprimirPoblaciones();

		void algoritmoSeleccionador(double lambda, vector<vector<float>> datos, list<tuple<int,int,double>> rest);

		void algoritmoCruce(int padre1, int padre2);

		void algoritmoMutacion(int pob);

		void BLsuave(double lambda, list<tuple<int,int,double>> restricciones, vector<vector<float>> datos);
};
