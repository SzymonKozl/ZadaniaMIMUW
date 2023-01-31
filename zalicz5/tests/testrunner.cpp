#include "next.h"

#include <vector>
#include <random>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include<fstream>

using namespace std;

namespace brute
{
    int nextInRange(int i, int a, int b, const vector<int> &x)
    {
        int s = (int)x.size();
        for (int j = i; j < s; j++)
            if (x[j] >= a && x[j] <= b)
                return j;
        return -1;
    }
}


void vec_to_file(const vector<int> &x, ofstream &f){
    f.open("tests/last_vec.txt");
    for (int el: x) f << el << " ";
    f.close();
}


void req_to_file(int i, int a, int b, ofstream &f){
    f.open("tests/last_req.txt");
    f << i << " " << a << " " << b;
    f.close();
}


int randrange(int a, int b)
{
    return a + rand() % (min(b - a, __INT_MAX__ - 1) + 1);
}

pair<vector<int>, pair<int, int>> gen_seq(int length, int mode)
{
    vector<int> v(length), vals;
    int min_val = __INT_MAX__, max_val = -__INT_MAX__ - 1, a;
    switch (mode)
    {
    case 0:
    { // homogenous
        a = rand() % (4 * length) - 2 * length;
        for (int i = 0; i < length; i++)
            v[i] = a;
        min_val = max_val = a;
        break;
    }
    case 1:
    { // many repeated values
        int sample_size = randrange(1, (length / 3) + 1);
        vals = gen_seq(sample_size, 2).first;
        for (int i = 0; i < length; i++)
        {
            v[i] = vals[rand() % sample_size];
            min_val = min(min_val, v[i]);
            max_val = max(max_val, v[i]);
        }
        break;
    }
    case 2:
    { // random values from range [0;length)
        for (int i = 0; i < length; i++)
        {
            v[i] = rand() % length;
            min_val = min(min_val, v[i]);
            max_val = max(max_val, v[i]);
        }
        break;
    }
    case 3:
    { // random values from range [-10 * length;10 * length)
        for (int i = 0; i < length; i++)
        {
            v[i] = rand() % (20 * length) - 10 * length;
            min_val = min(min_val, v[i]);
            max_val = max(max_val, v[i]);
        }
        break;
    }
    case 4:
    { // random values up to RAND_MAX
        for (int i = 0; i < length; i++)
        {
            v[i] = rand();
            min_val = min(min_val, v[i]);
            max_val = max(max_val, v[i]);
        }
        break;
    }
    default:
        break;
    }
    return make_pair(v, make_pair(min_val, max_val));
}

int gen_len()
{
    switch (rand() % 4)
    {
    case 0:
        return 1;
    case 1:
        return randrange(2, 10);
    case 2:
        return randrange(11, 100);
    case 3:
        return randrange(101, 100000);
    default:
        return 20;
    }
}

pair<int, int> gen_req_range(int min_val, int max_val)
{
    if (max_val >= __INT_MAX__ - 1 || min_val <= -__INT_MAX__)
        return make_pair(7, 13); // for safety
    int a, b;
    switch (rand() % 5)
    {
    case 0:
    { // range not intersecting with values of x
        if (rand() % 2)
        { // range greater than larges val in x
            a = randrange(max(max_val + 1, 0), __INT_MAX__);
            b = randrange(a, __INT_MAX__);
        }
        else
        {
            a = randrange(-__INT_MAX__, min(min_val - 1, 0));
            b = randrange(a, (a >= 0) ? min_val - 1 : min(a + __INT_MAX__, min_val - 1));
        }
        break;
    }
    case 1:
    { // all values of x are in request range
        a = randrange(max(max_val, 0), __INT_MAX__);
        b = randrange(-__INT_MAX__, min(min_val, 0));
        break;
    }
    case 2:
    { // request range is inside range of values of x
        a = randrange(min_val, max_val);
        b = randrange(a, (a >= 0) ? max_val : a + __INT_MAX__);
        break;
    }
    case 3:
    { // req range intersects with range of values of x
        if (rand() % 2)
        {
            a = randrange(min_val, max_val);
            b = randrange(a, (a < 0) ? a + __INT_MAX__ : __INT_MAX__);
        }
        else
        {
            b = randrange(min_val, max_val);
            a = randrange((b > 0) ? b - __INT_MAX__ : -__INT_MAX__, b);
        }
        break;
    }
    case 4:
    { // req range is in format [a;a]
        a = b = rand() - (RAND_MAX / 2);
        break;
    }
    default:
        break;
    }
    return make_pair(a, b);
}

int main(int argc, char **argv)
{
    assert(argc == 3);
    srand((unsigned)time(NULL));
    int sequences = atoi(argv[1]);
    int requests_per_seq = atoi(argv[2]);
    pair<vector<int>, pair<int, int>> gen_out;
    pair<int, int> req_data;
    vector<int> x;
    int seq_type, seq_len, min_val, max_val, i, a, b, resp1, resp2;
    ofstream f_vec, f_req;
    for (int k = 0; k < sequences; k++)
    {
        seq_type = rand() % 5;
        seq_len = gen_len();
        gen_out = gen_seq(seq_len, seq_type);
        x = gen_out.first;
        vec_to_file(x, f_vec);
        min_val = gen_out.second.first;
        max_val = gen_out.second.second;
        init(x);
        for (int r = 0; r < requests_per_seq; r++)
        {
            i = rand() % seq_len;
            req_data = gen_req_range(min_val, max_val);
            a = req_data.first;
            b = req_data.second;
            req_to_file(i, a, b, f_req);
            resp1 = brute::nextInRange(i, a, b, x);
            resp2 = nextInRange(i, a, b);
            if (resp1 != resp2){
                cout << "SEQ: " << k << " LEN: " << seq_len << " REQ: " << r << ": " << i << " " << a << " " << b;
                cout << " GOT: " << resp2 << "EXPECTED: " << resp1 << endl;
            }
            assert(resp1 == resp2);
        }
        cout << "SEQ: " << k << " LEN: " << seq_len << " ALL OK" << endl;
        done();
    }
}