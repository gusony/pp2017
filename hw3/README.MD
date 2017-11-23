mpicc -o integrate integrate .c -lm
mpiexec -n 2 ./a.out
