for i in {01..09};
do
    cat ../alt${i}.txt | ./../day${i}a.o
    cat ../alt${i}.txt | ./../day${i}b.o
done

cat ../alt10.txt | ./../day10a.o

for i in {11..20};
do
    cat ../alt${i}.txt | ./../day${i}a.o
    cat ../alt${i}.txt | ./../day${i}b.o
done
