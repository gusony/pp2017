gcc -pthread -std=gnu99 -O2 -s pi.c -o pi
taskset -c 1,2 ./pi 2 100000

