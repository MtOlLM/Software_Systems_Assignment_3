for live in {560..610}; do
    gcc -O2 -DLIVE=$live c_allocation_stress_test.c allocator.c freelist.c -o stress_test
    fails=0
    for seed in {1..100}; do
        if ! SEED=$seed ./stress_test | grep -q "No failure occurred"; then
            fails=$((fails + 1))
        fi
    done
    echo "LIVE=$live  failed $fails / 100" >> logs/first_fit.txt
done

rm -f stress_test