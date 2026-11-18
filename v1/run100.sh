for i in {1..100}; do
    ./mem_allocator
    #sleep to pass a different seed to the pseudorandom nuumber generator
    sleep 1
done