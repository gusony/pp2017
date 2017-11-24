mpicc -o prime prime.c -lm
mpicc -o integrate integrate .c -lm
mpiexec -n 1 ./prime 1234567
