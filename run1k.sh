for i in {1..1000}; do
    ./mem_allocator
    #sleep to pass a different seed to the pseudorandom nuumber generator
    sleep 0.1
done