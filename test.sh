for i in {01..25};
do
    cat alt${i}.txt | ./day${i}a.o
    cat alt${i}.txt | ./day${i}b.o
done
