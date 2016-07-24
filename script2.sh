echo "###########"
echo "keller6"
echo "==========="
echo "SEQUENCIAL"
echo "==========="

for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time mpirun -n 1 ./acoMPI -i keller6.clq.b -a 2 -b 1 -f 120 -c 100
done
