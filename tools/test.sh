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

cat ../alt21.txt | ./../day21a.o

for i in {22..22};
do
    cat ../alt${i}.txt | ./../day${i}a.o
    cat ../alt${i}.txt | ./../day${i}b.o
done

for i in {24..24};
do
    cat ../alt${i}.txt | ./../day${i}a.o
    cat ../alt${i}.txt | ./../day${i}b.o
done

cat ../alt25.txt | ./../day25z.o
