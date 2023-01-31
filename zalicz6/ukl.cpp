#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <assert.h>

using namespace std;

typedef vector<vector<bool>> rmap;
typedef vector<vector<char>> config;

// represents one rotation of element
struct Shape
{
    rmap map;
     // first index i that map[0][i] is filled by represented shape
    unsigned short begin_x;
    unsigned short height;
    unsigned short width;

    Shape(rmap raw_map)
    {
        // reducing map of full board size to minimal size that contains shape
        int start_x = 0;
        while (start_x < (int)raw_map[0].size())
        {
            int s = 0;
            for (int i = 0; i < (int)raw_map.size(); i++)
                if (raw_map[i][start_x])
                    s++;
            if (s)
                break;
            start_x++;
        }
        int end_x = (int)raw_map[0].size() - 1;
        while (end_x >= 0)
        {
            int s = 0;
            for (int i = 0; i < (int)raw_map.size(); i++)
                if (raw_map[i][end_x])
                    s++;
            if (s)
                break;
            end_x--;
        }
        int start_y = 0;
        while (start_y < (int)raw_map.size() - 1 &&
         !reduce(raw_map[start_y].begin(), raw_map[start_y].end()))
            start_y++;
        int end_y = (int)raw_map.size() - 1;
        while (end_y >= 0 &&
         !reduce(raw_map[end_y].begin(), raw_map[end_y].end()))
            end_y--;
        for (int i = start_y; i <= end_y; i++)
            map.push_back(vector<bool>(raw_map[i].begin() + start_x,
             raw_map[i].begin() + end_x + 1));
        height = (unsigned short)max(end_y - start_y + 1, 0);
        width = (unsigned short)max(end_x - start_x + 1, 0);
        begin_x = 0;
        while (begin_x + 1 < width && !map[0][begin_x])
            begin_x++;
    }

    Shape(){};
};

bool eq(Shape *s1, Shape *s2)
{
    if (s1->height != s2->height ||
     s1->width != s2->width ||
     s1->begin_x != s2->begin_x)
        return false;
    for (int i = 0; i < s1->height; i++)
    {
        for (int j = 0; j < s1->width; j++)
        {
            if (s1->map[i][j] != s2->map[i][j])
                return false;
        }
    }
    return true;
}

// struct representing group of elements with equal shape
typedef struct Element
{

    unsigned short surface;
    vector<Shape> rotations;
    string ids;

    int left_to_use;  // number of currently unused elements with shape
    int total_to_use; // number of total elements with shape

    // if set to true one element with given
    // shape is available only in one rotation
    // (when considering square board)
    // or two (when considering non-square board)
    bool special_one = false;
    // how many elements were left to use when fixed-roatation element
    // was placed (or -1 if it was never placed)
    int used_special = -1;

    // returns shape rotated by 90 degrees in clockwise direction
    static Shape rotate(Shape s)
    {
        Shape res;
        res.width = s.height;
        res.height = s.width;
        res.map = rmap(res.height);
        for (int i = 0; i < res.height; i++)
        {
            res.map[i] = vector<bool>(res.width);
            for (int j = 0; j < res.width; j++)
                res.map[i][j] = s.map[s.height - j - 1][i];
        }
        res.begin_x = 0;
        while (res.begin_x < res.width && !res.map[0][res.begin_x])
            res.begin_x++;
        return res;
    }

    Element(rmap &map_p, char id_p)
    {
        // generating rotations
        rotations = vector<Shape>{Shape(map_p)};
        Shape r1 = rotate(rotations[0]);
        if (!eq(&r1, &rotations[0]))
        {
            rotations.push_back(r1);
            Shape r2 = rotate(rotations[1]);
            if (!eq(&r2, &rotations[0]))
            {
                rotations.push_back(r2);
                rotations.push_back(rotate(r2));
            }
        }
        ids = string(1, id_p);
        surface = 0;
        for (auto v : map_p)
            for (bool val : v)
                if (val)
                    surface++;
        left_to_use = total_to_use = 1;
    }
} *ElPtr;

bool eq(ElPtr e1, ElPtr e2)
{
    if (e1->surface != e2->surface)
        return false;
    for (unsigned int i = 0; i < e1->rotations.size(); i++)
    {
        for (unsigned int j = 0; j < e2->rotations.size(); j++)
        {
            if (eq(&e1->rotations[i], &e2->rotations[j]))
                return true;
        }
    }
    return false;
}

// reads element from stdin
ElPtr scan_el(unsigned short width, unsigned short height)
{
    rmap v(height);
    char id = '~';
    for (unsigned short i = 0; i < height; i++)
    {
        v[i] = vector<bool>(width);
        for (unsigned short j = 0; j < width; j++)
        {
            char c = (char)getchar();
            if (c != '.')
                id = c;
            v[i][j] = c != '.';
        }
        getchar();
    };
    ElPtr e = new Element(v, id);
    return e;
}

struct Solver_BT
{

    // used for holding some config parameters that are non trivial
    // in reversing step
    struct BtState{
        // how many consequentive rows starting from top are already filled
        unsigned short filled_rows = 0;
        // how many cells starting from left in first non-completed row are filled
        unsigned short filled_in_row = 0;
        // all puzzles with index below that value are currently used
        unsigned short used_up_to = 0;
    };

    // '*' means empty field
    config configuration;
    unsigned short surface_filled;
    BtState s;

    bool found;
    config solution;

    unsigned short width;
    unsigned short height;
    unsigned short surface;
    vector<ElPtr> puzzles;

    void update_filled()
    {
        while (s.filled_rows < height &&
         configuration[s.filled_rows][s.filled_in_row] != '*')
        {
            if (s.filled_in_row == width - 1)
            {
                s.filled_in_row = 0;
                s.filled_rows++;
            }
            else
                s.filled_in_row++;
        }
    }

    void step(ElPtr elem, unsigned short rotation)
    {
        elem->left_to_use--;
        if (elem->special_one &&
         elem->used_special == -1 &&
         rotation == 0)
         elem->used_special=elem->left_to_use;
        Shape *r = &(*(elem->rotations.begin() + rotation));
        for (unsigned short i = 0; i < r->height; i++)
        {
            for (unsigned short j = 0; j < r->width; j++)
            {
                if (r->map[i][j])
                    configuration[i + s.filled_rows][j + s.filled_in_row - 
                    r->begin_x] = elem->ids[elem->left_to_use];
            }
        }
        surface_filled += elem->surface;
        update_filled();
    }

    void step_back(ElPtr elem, unsigned short rotation)
    {
        Shape *r = &elem->rotations[rotation];
        for (unsigned int i = 0; i < r->height; i++)
        {
            for (unsigned int j = 0; j < r->width; j++)
            {
                if (r->map[i][j])
                    configuration[s.filled_rows + i]
                    [s.filled_in_row - r->begin_x + j] = '*';
            }
        }
        if (elem -> special_one && elem -> left_to_use == elem->used_special)
         elem->used_special = -1;
        elem->left_to_use++;
        surface_filled -= elem->surface;
    }

    bool is_final()
    {
        return surface_filled == width * height;
    }

    // checks if element can be placed to cover 
    // first free cell (position: [filled_rows, filled_in_row])
    bool fits(ElPtr elem, unsigned short rotation)
    {
        Shape *r = &elem->rotations[rotation];
        if (s.filled_in_row < r->begin_x)
            return false;
        if (s.filled_in_row + r->width - r->begin_x > width)
            return false;
        if (s.filled_rows + r->height > height)
            return false;
        for (unsigned short i = 0; i < r->height; i++)
        {
            for (unsigned short j = 0; j < r->width; j++)
            {
                if (configuration[s.filled_rows + i]
                [s.filled_in_row + j - r->begin_x] != '*' && r->map[i][j])
                    return false;
            }
        }
        return true;
    }

    Solver_BT(vector<ElPtr> &puzzles_p, unsigned short width_p,
     unsigned short height_p)
    {
        puzzles = puzzles_p;
        // branches that begin with large elemetns are considered at first
        sort(puzzles.begin(), puzzles.end(), [](ElPtr a, ElPtr b)
             { return a->surface > b->surface; });
        height = height_p;
        width = width_p;
        // selects one element that will be considered in one rotation
        // this allows to eliminate some final configurations
        // that are rotations of others
        if (width == height)
        {
            int index = (int)puzzles.size() - 1;
            // index of puzzle to be marked as with fixed rotation
            int index_to_mark = 0;
            while (index > 0 && puzzles[index]->rotations.size() < 4)
            {
                if (puzzles[index]->rotations.size() >
                 puzzles[index_to_mark]->rotations.size())
                    index_to_mark = index;
                index--;
            }
            if (puzzles[index]->rotations.size() >
             puzzles[index_to_mark]->rotations.size())
                index_to_mark = index;
            puzzles[index]->special_one = true;
        }
        else
        {
            int index = (int)puzzles.size() - 1;
            while (index >= 0 && puzzles[index]->rotations.size() < 4)
            {
                index--;
            }
            if (index >= 0)
                puzzles[index]->special_one = true;
        }
        surface = width * height;
        surface_filled = 0;
        found = false;
        configuration = config(height, vector<char>(width, '*'));
    }

    void solve_bt()
    {
        if (is_final())
        {
            found = true;
            solution = configuration;
            return;
        }
        // config parameters keept untouched when stepping into branch
        BtState state_cpy;
        for (unsigned short i = s.used_up_to; i < puzzles.size(); i++)
        {
            if (!puzzles[i]->left_to_use)
                continue;
            if (surface_filled + puzzles[i]->surface > surface)
                continue;
            // how many rotations to consider
            unsigned r_lim = (puzzles[i]->special_one && 
            puzzles[i]->left_to_use == 1 && puzzles[i]->used_special != -1) ?
             ((width == height) ? 1 : 2) :
              (unsigned)puzzles[i]->rotations.size();
            for (unsigned short r = 0; r < r_lim; r++)
            {
                if (!fits(puzzles[i], r))
                    continue;
                state_cpy = s;
                if (!r) puzzles[i]->used_special = true; 
                if (i == s.used_up_to && puzzles[i]->left_to_use == 1)
                    s.used_up_to++;
                while (s.used_up_to < puzzles.size() &&
                 !puzzles[s.used_up_to]->left_to_use)
                    s.used_up_to++;
                step(puzzles[i], r);
                solve_bt();
                if (found)
                    return;
                s = state_cpy;
                step_back(puzzles[i], r);
            }
        }
    }

    pair<config, bool> extract()
    {
        return make_pair(solution, found);
    }
};

int main()
{
    unsigned short n, m, k;
    cin >> n >> m >> k;
    getchar();
    vector<ElPtr> puzzles(k);
    unsigned index = 0;
    unsigned surface_sum = 0;
    for (int i = 0; i < k; i++)
    {
        puzzles[index] = scan_el(m, n);
        surface_sum += puzzles[index]->surface;
        bool is_duplicate = false;
        for (unsigned j = 0; j < index; j++)
        {
            if (eq(puzzles[index], puzzles[j]))
            {
                // merging the same elements into one structure
                puzzles[j]->left_to_use++;
                puzzles[j]->ids += puzzles[index]->ids;
                is_duplicate = true;
                delete puzzles[index];
            }
        }
        if (!is_duplicate)
            index++;
        if (i < k - 1)
            getchar();
    }
    puzzles.resize(index);
    puzzles.shrink_to_fit();
    Solver_BT solver(puzzles, m, n);
    if (surface_sum >= solver.surface) solver.solve_bt();
    auto t = solver.extract();
    if (t.second)
    {
        cout << "TAK" << endl;
        for (unsigned short j = 0; j < n; j++)
        {
            for (unsigned short i = 0; i < m; i++)
            {
                cout << t.first[j][i];
            }
            cout << endl;
        }
    }
    else
        cout << "NIE";
    for (unsigned short j = 0; j < index; j++)
        delete puzzles[j];
    return 0;
}
