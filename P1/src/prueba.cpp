#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <random>
#include "ARP.h"
#include "timer.h"

using namespace std;


int main(int argc, char **argv){
	ARP datos(argv[1], argv[2], stoi(argv[3]));

	if(string(argv[5]) == "greedy"){
		start_timers();
		datos.Greedy(stoul(argv[4]));
		cout<<"Tiempo: "<<elapsed_time()<<endl;
	}
	else{
		start_timers();
		datos.BL(stoul(argv[3]));
		cout<<"Tiempo: "<<elapsed_time()<<endl;
	}

}
