#include <iostream>
#include <vector>
#include <set>

using namespace std;

class Cluster{
	private:
		vector<float> Centroide;
		set<int> Datos;

	public:
		Cluster(vector<float> centro){
			Centroide = centro;
		}
		void aniadeDatos(int aniade);

		vector<float> devuelveCentroide();

		int numeroDatos(){
			return Datos.size();
		}

		void eliminaDatos(int dato);

		void vaciarDatos(){
			Datos.clear();
		}

		set<int> devuelveDatos(){
			return Datos;
		}

		void asignarCentroide(vector<float> centro){
			Centroide = centro;
		}
};
