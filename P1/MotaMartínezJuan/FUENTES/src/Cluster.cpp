#include <vector>
#include <math.h>

#include "Cluster.h"

using namespace std;

void Cluster::aniadeDatos(int aniade){
	Datos.insert(Datos.begin(), aniade);
}

void Cluster::eliminaDatos(int dato){
	set<int>::iterator it_dato = Datos.find(dato);
	if(it_dato == Datos.end()){
	}
	else
		Datos.erase(it_dato);
}

vector<float> Cluster::devuelveCentroide(){
	return Centroide;
}
