mpicc -o prime prime.c -lm
mpicc -o integrate integrate .c -lm
mpiexec -n 2 ./a.out 1234567
