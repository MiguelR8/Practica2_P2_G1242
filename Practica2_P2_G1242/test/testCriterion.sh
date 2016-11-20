make

rm -f test/ibics.csv test/isacs.csv
rm -f test/kbics.csv test/ksacs.csv

d=`python -c "from random import randrange; print randrange(0xFFFFFFFFFFFFFFFF)"`
k=`python -c "from random import randrange; print randrange(0xFFFFFFFFFFFFFFFF)"`

#SAC
for i in `python -c "print ' '.join([str(i) for i in range(1,17)])"`
do
	#Variando clave
	./desFTests -S -K -n $i -d $d -k $k >> test/ksacs.csv
	#Variando dato
	./desFTests -S -I -n $i -d $d -k $k >> test/isacs.csv
done

#BIC
for i in `python -c "print ' '.join([str(i) for i in range(1,17)])"`
do
	#Variando clave
	./desFTests -B -K -n $i -d $d -k $k >> test/kbics.csv
	#Variando dato
	./desFTests -B -I -n $i -d $d -k $k >> test/ibics.csv
done
