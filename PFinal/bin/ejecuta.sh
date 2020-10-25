#!/bin/bash

semillas=(1345378882 572718921 34737829 28388992 9987479)

make

for sem in ${semillas[*]}
do
	echo "Ejecutando iris con 10 restricciones y semilla $sem"
	./bin/PFINAL_exe 3 300 ./bin/ConjuntosNuevosPAR/iris_set.dat ./bin/ConjuntosNuevosPAR/iris_set_const_10.const $sem

	echo "Ejecutando ecoli con 10 restricciones y semilla $sem"
	./bin/PFINAL_exe 8 300 ./bin/ConjuntosNuevosPAR/ecoli_set.dat ./bin/ConjuntosNuevosPAR/ecoli_set_const_10.const $sem

	echo "Ejecutando rand con 10 restricciones y semilla $sem"
	./bin/PFINAL_exe 3 300 ./bin/ConjuntosNuevosPAR/rand_set.dat ./bin/ConjuntosNuevosPAR/rand_set_const_10.const $sem

	echo "Ejecutando newthyroid con 10 restricciones y semilla $sem"
	./bin/PFINAL_exe 3 300 ./bin/ConjuntosNuevosPAR/newthyroid_set.dat ./bin/ConjuntosNuevosPAR/newthyroid_set_const_10.const $sem

	echo "Ejecutando iris con 20 restricciones y semilla $sem"
	./bin/PFINAL_exe 3 300 ./bin/ConjuntosNuevosPAR/iris_set.dat ./bin/ConjuntosNuevosPAR/iris_set_const_20.const $sem

	echo "Ejecutando ecoli con 20 restricciones y semilla $sem"
	./bin/PFINAL_exe 8 300 ./bin/ConjuntosNuevosPAR/ecoli_set.dat ./bin/ConjuntosNuevosPAR/ecoli_set_const_20.const $sem

	echo "Ejecutando rand con 20 restricciones y semilla $sem"
	./bin/PFINAL_exe 3 300 ./bin/ConjuntosNuevosPAR/rand_set.dat ./bin/ConjuntosNuevosPAR/rand_set_const_20.const $sem

	echo "Ejecutando newthryroid con 20 restricciones y semilla $sem"
	./bin/PFINAL_exe 3 300 ./bin/ConjuntosNuevosPAR/newthyroid_set.dat ./bin/ConjuntosNuevosPAR/newthyroid_set_const_20.const $sem

done
