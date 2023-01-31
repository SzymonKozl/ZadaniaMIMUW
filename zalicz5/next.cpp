#include "next.h"

#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

using namespace std;

namespace
{

    // rounds number to closest smaller or equal power of 2 (n have to be positive)
    int clst_pwr(int n)
    {
        int p;
        for (p = 1; 2 * p <= n; p *= 2);
        return p;
    }

    struct node
    {
        shared_ptr<node> left, right; // subtrees
        long long val; // lowest index that has value between range_a and range_b
        int range_a, range_b;
    };

    shared_ptr<node> *roots;

    // auxillary function for recursive tree generation
    shared_ptr<node> make_tree(const vector<pair<int, int>> &sorted, int first_index, int last_index)
    {
        shared_ptr<node> ptr = make_shared<node>();
        ptr->val = __LONG_LONG_MAX__;
        ptr->range_a = sorted[first_index].first;
        ptr->range_b = sorted[last_index].first;
        if (first_index == last_index)
        {
            ptr->left = ptr->right = NULL;
        }
        else
        {
            int len = last_index - first_index + 1;
            int pwr1 = clst_pwr(len / 2);
            int size1 = (len > 3 * pwr1) ? 2 * pwr1 : len - pwr1; // size of left subtree
            ptr->left = make_tree(sorted, first_index, first_index + size1 - 1);
            ptr->right = make_tree(sorted, first_index + size1, last_index);
        }
        return ptr;
    }

    // generates full tree that considers only last_val from sorted vector. "sorted" argument is vector of sorted and duplicate-freed values from x
    // assumes that last_val is in vector
    shared_ptr<node> make_tree(const vector<pair<int, int>> &sorted, int last_val, long long ind, int first_index, int last_index)
    {
        shared_ptr<node> tree = make_tree(sorted, first_index, last_index);
        shared_ptr<node> n = tree;
        while (n->range_a != n->range_b)
        {
            n->val = ind;
            if (last_val <= n->left->range_b)
                n = n->left;
            else
                n = n->right;
        }
        n->val = ind;
        return tree;
    }

    // generates updated root that it considers values strating from ith index of x. prev is root that considered only values from i+1 th index
    shared_ptr<node> make_root(int el, long long ind, shared_ptr<node> prev)
    {
        shared_ptr<node> n = prev, old_parent, new_parent, new_node, new_root;
        do
        {
            new_node = make_shared<node>();
            new_node->range_a = n->range_a;
            new_node->range_b = n->range_b;
            new_node->val = min(n->val, ind);
            if (new_parent)
            {
                if (el <= old_parent->left->range_b)
                {
                    new_parent->left = new_node;
                    new_parent->right = old_parent->right;
                }
                else
                {
                    new_parent->left = old_parent->left;
                    new_parent->right = new_node;
                }
            }
            else
                new_root = new_node;
            old_parent = n;
            if (n->range_a != n->range_b)
                n = (el <= n->left->range_b) ? n->left : n->right;
            else
                n = NULL;
            new_parent = new_node;
        } while (n);
        return new_root;
    }

    // returns minimal index in range (a, b) in given root or __LONG_LONG_MAX__ if no index staisfies request
    long long smallest_index(int a, int b, shared_ptr<node> root)
    {
        if (root->range_a == root->range_b)
            return (root->range_a >= a && root->range_a <= b) ? root->val : __LONG_LONG_MAX__;
        if (a <= root->range_a && b >= root->range_b)
            return root->val;
        long long res = __LONG_LONG_MAX__;
        if (a <= root->left->range_b && b >= root->left->range_a)
            res = min(res, smallest_index(a, b, root->left));
        if (b >= root->right->range_a && a <= root->right->range_b)
            res = min(res, smallest_index(a, b, root->right));
        return res;
    }
}

void init(const vector<int> &x)
{
    int size_x = (int)x.size();
    roots = new shared_ptr<node>[size_x];
    vector<pair<int, int>> cpy(size_x); // sorted values from x
    for (int i = 0; i < size_x; i++)
        cpy[i] = make_pair(x[i], i);
    sort(cpy.begin(), cpy.end(), [](pair<int, int> a, pair<int, int> b)
         { return (a.first < b.first || (a.first == b.first && a.second < b.second)); });
    vector<pair<int, int>> without_duped(size_x); // sorted values from x without duplicates
    int filled = 0;
    for (pair<int, int> el : cpy)
    {
        if (!filled || without_duped[filled - 1].first != el.first)
            without_duped[filled++] = el;
    }
    without_duped.resize(filled);
    without_duped.shrink_to_fit();
    int size = (int)without_duped.size();
    roots[size_x - 1] = make_tree(without_duped, x[size_x - 1], size_x - 1, 0, size - 1);
    for (int i = size_x - 2; i >= 0; i--)
    {
        roots[i] = make_root(x[i], (long long)i, roots[i + 1]);
    }
}

int nextInRange(int i, int a, int b)
{
    long long res = smallest_index(a, b, roots[i]);
    if (res == __LONG_LONG_MAX__)
        return -1;
    return (int)res;
}

void done()
{
    delete[] roots;
}
