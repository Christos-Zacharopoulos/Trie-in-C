for i in {1..20}
do
    printf "Dynamic small\n"
    time ./ngrams -i small_dynamic/small_dynamic.init -q small_dynamic/small_dynamic.work > a;
    cmp a small_dynamic/small_dynamic.result;
    printf "Dynamic medium\n"
    time ./ngrams -i medium_dynamic/medium_dynamic.init -q medium_dynamic/medium_dynamic.work > a;
    cmp a medium_dynamic/medium_dynamic.result;
    printf "Dynamic large\n"
    time ./ngrams -i large_dynamic/large_dynamic.init -q large_dynamic/large_dynamic.work > a;
    cmp a large_dynamic/large_dynamic.result;
    printf "Static small\n"
    time ./ngrams -i small_static/small_static.init -q small_static/small_static.work > a;
    cmp a small_static/small_static.result;
    printf "Static medium\n"
    time ./ngrams -i medium_static/medium_static.init -q medium_static/medium_static.work > a;
    cmp a medium_static/medium_static.result;
    printf "Static large\n"
    time ./ngrams -i large_static/large_static.init -q large_static/large_static.work > a;
    cmp a large_static/large_static.result;
done
