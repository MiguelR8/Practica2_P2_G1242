make
rm outi.txt outp.txt
for f in 250.txt 500.txt 1000.txt 2000.txt
do
	./seg-perf -P -i $f >> outp.txt
	./seg-perf -I -i $f >> outi.txt
done
