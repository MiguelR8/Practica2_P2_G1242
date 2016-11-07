make

d=`python -c "from random import randrange; print randrange(0xFFFFFFFFFFFFFFFF)"`
k=`python -c "from random import randrange; print randrange(0xFFFFFFFFFFFFFFFF)"`

for i in `python -c "print ' '.join([str(i) for i in range(1,17)])"`
do
	printf "Variando clave $k\n"
	./desAvTest -K -n $i -d $d -k $k
	printf "Variando dato $d\n"
	./desAvTest -I -n $i -d $d -k $k
done
