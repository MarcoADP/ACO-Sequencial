echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "keller6"
echo "==========="
echo "SEQUENCIAL"
echo "==========="

for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i keller6.clq.b -a 2 -b 1 -f 120 -c 100
done
for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i keller6.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "keller5"
echo "==========="
echo "SEQUENCIAL"
echo "==========="

for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i keller5.clq.b -a 2 -b 1 -f 120 -c 100
done
for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i keller5.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "keller4"
echo "==========="
echo "SEQUENCIAL"
echo "==========="

for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i keller4.clq.b -a 2 -b 1 -f 120 -c 100
done
for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i keller4.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "MANN_a81.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i MANN_a81.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i MANN_a81.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "MANN_a45.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i MANN_a45.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i MANN_a45.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "p_hat1500-3.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i p_hat1500-3.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i p_hat1500-3.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "p_hat1500-2.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i p_hat1500-2.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i p_hat1500-2.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "p_hat1500-1.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i p_hat1500-1.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i p_hat1500-1.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "p_hat1000-3.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i p_hat1000-3.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i p_hat1000-3.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "p_hat1000-2.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i p_hat1000-2.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i p_hat1000-2.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "p_hat1000-1.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i p_hat1000-1.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i p_hat1000-1.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "hamming10-4.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i hamming10-4.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i hamming10-4.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "hamming10-2.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i hamming10-2.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i hamming10-2.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done


echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "san1000.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i san1000.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i san1000.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "brock800_4.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i brock800_4.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i brock800_4.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "brock800_3.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i brock800_3.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i brock800_3.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "brock800_2.clq.b"
echo "==========="
echo "SEQUENCIAL"
echo "==========="
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
   time ./aco -i brock800_2.clq.b -a 2 -b 1 -f 120 -c 100
done

for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i brock800_2.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done

echo "###########"
echo "###########"
echo "###########"
echo "###########"
echo "brock800_1.clq.b"
echo "==========="
for t in 1 2 4 8
do
	echo "==========="
	echo "thread $t"
	echo "==========="
	for i in 1 2 3 4 5 6 7 8 9 10 11 12
	do
	   time ./acoP -i brock800_1.clq.b -a 2 -b 1 -f 120 -c 100 -t $t
	done
done