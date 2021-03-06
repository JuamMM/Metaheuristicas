#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <random>
#include "ARP.h"
#include "timer.h"

using namespace std;


 int main(){
	 unsigned semillas[5] = {774611256, 52661822, 4726488, 77192004, 43321123};

	 ARP iris_10("iris_set.dat", "iris_set_const_10.const",  3);
	 ARP ecoli_10("ecoli_set.dat", "ecoli_set_const_10.const", 8);
	 ARP rand_10("rand_set.dat", "rand_set_const_10.const", 3);

	 ARP iris_20("iris_set.dat", "iris_set_const_20.const", 3);
	 ARP ecoli_20("ecoli_set.dat", "ecoli_set_const_20.const", 8);
	 ARP rand_20("rand_set.dat", "rand_set_const_20.const", 3);

	 cout<<"Restricciones 10"<<endl;
	 for(int i=0; i<5; i++){
		cout<<"Iteracion: "<<i<<endl;
		cout<<"Iris"<<endl;
		start_timers();
	   iris_10.Greedy(semillas[i]);
		cout<<"Tiempo: "<<elapsed_time()<<endl;
		cout<<"Ecoli"<<endl;
		start_timers();
		ecoli_10.Greedy(semillas[i]);
		cout<<"Tiempo: "<<elapsed_time()<<endl;
		cout<<"Rand"<<endl;
		start_timers();
		rand_10.Greedy(semillas[i]);
		cout<<"Tiempo: "<<elapsed_time()<<endl;
		iris_10.eliminarDatos();
		ecoli_10.eliminarDatos();
		rand_10.eliminarDatos();
	 }

	 cout<<"Restricciones 20"<<endl;
	 for(int i=0; i<5; i++){
		cout<<"Iteracion: "<<i<<endl;
		cout<<"Iris"<<endl;
		start_timers();
	   iris_20.Greedy(semillas[i]);
		cout<<"Tiempo: "<<elapsed_time()<<endl;
		cout<<"Ecoli"<<endl;
		start_timers();
		ecoli_20.Greedy(semillas[i]);
		cout<<"Tiempo: "<<elapsed_time()<<endl;
		cout<<"Rand"<<endl;
		start_timers();
		rand_20.Greedy(semillas[i]);
		cout<<"Tiempo: "<<elapsed_time()<<endl;
		iris_20.eliminarDatos();
		ecoli_20.eliminarDatos();
		rand_20.eliminarDatos();
	 }

 }
