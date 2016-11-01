#awk script by http://stackoverflow.com/questions/1729824/transpose-a-file-in-bash#answer-1729980

make
rm -f test/outi.txt test/outp.txt test/outpt.csv test/testit.csv
for f in 50.txt 100.txt 250.txt 500.txt 1000.txt 2000.txt 4000.txt
do
	./seg-perf -P -i test/$f >> test/outp.txt
	awk -f transpose.awk test/outp.txt > test/outpt.csv
	./seg-perf -I -i test/$f >> test/outi.txt
	awk -f transpose.awk test/outi.txt > test/outit.csv
done
