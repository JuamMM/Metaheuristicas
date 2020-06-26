#include <algorithm>
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
		PAR(int poblaciones, int clusters, int centros, int num_datos, int min, int max, list<tuple<int,int,double>> rest, vector<vector<float>> dat, float lam){
			tam = num_datos;
			num_poblaciones = poblaciones;
			num_clusters = clusters;
			minimo = min;
			maximo = max;
			tam_centro = centros;
			for(int i=0; i<num_poblaciones; i++){
				Poblacion pob(num_datos, centros, clusters, min, max, rest, dat, lam);
				this->poblaciones.push_back(pob);
			}
		}

		Poblacion devuelvePoblacion(int indice){
			return poblaciones[indice];
		}

		void asignacionAleatoria(int indice){
			poblaciones[indice].asignacionAleatoria();
		}

		void reparacion();

		void generarPoblacionesAleatorias(int num_datos);

		void imprimirPoblaciones();

		vector<int> mejoresPadres(double lambda,vector<vector<float>> datos, list<tuple<int,int,double>> res, int num_padres);

		vector<int> peoresPadres(double lambda,vector<vector<float>> datos, list<tuple<int,int,double>> res, int num_padres);

		void algoritmoMutacion(int pob, int prob_muta, int rangp_muta);

		int mejorCromosoma(double lambda,  vector<vector<float>> datos, list<tuple<int,int,double>> restricciones);

		void BLsuaveMejores(double lambda, list<tuple<int,int,double>> restricciones, vector<vector<float>> datos, int num_mejores);

		void sustituyePoblacion(int indice, Poblacion pob){
			poblaciones[indice] = pob;
		}

		int peorCromosoma(double lambda, list<tuple<int,int,double>> restricciones, vector<vector<float>> datos);

};
