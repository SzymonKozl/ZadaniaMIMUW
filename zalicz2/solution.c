#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


// finds first (or last) indexes of different elements in array (eg: t=[1, 2, 1, 2, 3], step=1 -> [0, 1, 4])
// negative step used for last elements
// n: max number of elements to iterate over
// how_many_found: pointer to variable that will be set to number of initialized values in output (accessing with indexes greater than it causes UB)
int ** first3_diff_indexes(int * t, int n, int step, int * how_many_found);


bool is_different(int a, int b, int c);


int main(){
    int N;
    assert(scanf("%i", &N));
    int * chains = (int *) malloc((unsigned)N * sizeof(int));
    unsigned * dists = (unsigned *) malloc((unsigned)N * sizeof(int));
    for (int i = 0; i < N; i++) assert(scanf("%i %u", &chains[i], &dists[i]));
    int begin_found, end_found;
    int ** begin_indexes = first3_diff_indexes(chains, N, 1, &begin_found); // first three indexes of hotels of different chains
    int ** end_indexes = first3_diff_indexes(chains + N - 1, N, -1, &end_found); // last three indexes of hotels of different chains
    if (begin_found < 3) printf("0 0");
    else {
        unsigned max = 0;
        for (unsigned i = 0; i < 3; i ++){
            for (unsigned j = 0; j < 3; j++){
                if (* begin_indexes[i] != * end_indexes[j]){
                    for (int * k = begin_indexes[i] + 1; k < end_indexes[j]; k ++){
                        if (* begin_indexes[i] != * k && * end_indexes[j] != * k){
                            unsigned d1 = dists[(int) (begin_indexes[i] - chains)];
                            unsigned d2 = dists[(int) (k - chains)];
                            unsigned d3 = dists[(int) (end_indexes[j] - chains)];
                            max = MAX(max, MIN(d2 - d1, d3 - d2));
                        }
                    }
                }
            }
        }
        unsigned min = __INT32_MAX__;
        int first_cands[] = {0, -1, -1}; // candidates for first hotel (indexes)
        int last_cands[] = {2, 3, 4}; // candidates for las hotel (indexes)
        for (int mid = 1; mid < N - 1; mid ++){
            // updating elems_before
            if (chains[first_cands[0]] != chains[mid - 1]){
                int t = first_cands[1];
                first_cands[1] = first_cands[0];
                if (chains[first_cands[1]] != chains[mid - 1]){
                    first_cands[2] = t;
                }
            }
            first_cands[0] = mid - 1;
            // updating elements after
            while (last_cands[1] < N && (chains[last_cands[1]] == chains[last_cands[0]] || last_cands[0] == last_cands[1])) last_cands[1] ++;
            while (last_cands[2] < N && (
                last_cands[2] == last_cands[1] || chains[last_cands[1]] == chains[last_cands[2]] || chains[last_cands[0]] == chains[last_cands[2]]
            )) last_cands[2] ++;
            for (unsigned i = 0; i < 3; i++){
                if (first_cands[i] > -1){
                    for (unsigned j = 0; j < 3; j++){
                        if (last_cands[j] > -1 && last_cands[j] < N && is_different(chains[first_cands[i]], chains[last_cands[j]], chains[mid])){
                            min = MIN(min, MAX(dists[last_cands[j]] - dists[mid], dists[mid] - dists[first_cands[i]]));
                        }
                    }
                }
            }
            last_cands[0] ++;
        }
        printf("%i %i", min, max);
    }
    free(begin_indexes);
    free(end_indexes);
    free(chains);
    free(dists);
    return 0;
}


int ** first3_diff_indexes(int * t, int n, int step, int * how_many_found){
    int ** res = (int **) malloc(3 * sizeof(int*));
    int passed = 0;
    int found = 0;
    for (int* ptr = t; found < 3 && passed < n; ptr = ptr + step){
        switch (found)
        {
        case 0:
            res[0] = ptr;
            found ++;
            break;
        case 1:
            if (*res[0] != *ptr){
                res[1] = ptr;
                found ++;
            }
            break;
        case 2:
            if (*res[0] != *ptr && *res[1] != *ptr){
                res[2] = ptr;
                found ++;
            }
            break;
        default:
            break;
        }
        passed ++;
    }
    * how_many_found = found;
    return res;
}


bool is_different(int a, int b, int c){ return (a != b && b != c && c!= a);}