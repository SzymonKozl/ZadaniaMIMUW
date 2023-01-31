#include <vector>
#include <cstdlib>

#include "kol.h"

namespace
{

    // returns node in list that is a neighbour of l1 but is not l2
    interesant *_opposite(interesant *i1, interesant *i2);

    // assuming that i1 and i2 are neighbours, changes pointer from i1 to i2 to point to i3
    void _put_in_place_of(interesant *i1, interesant *i2, interesant *i3);

    // assuming that 2 nodes are in the same list, i1 != i2, what is the next node in direction of i2 starting from i1
    interesant *_direction(interesant *i1, interesant *i2);

    struct _queue
    {
        interesant *head, *tail;

        void push(interesant *i)
        {
            i->next = tail->next;
            i->prev = tail;
            tail->next = i;
            _put_in_place_of(i->next, tail, i);
        }

        bool is_empty()
        {
            return head->next == tail;
        }

        static void empty_queue(_queue *target)
        {
            target->head = (interesant *)malloc(sizeof(interesant));
            target->tail = (interesant *)malloc(sizeof(interesant));
            target->head->next = target->tail;
            target->head->prev = NULL;
            target->tail->next = target->head;
            target->tail->prev = NULL;
        }
    };

    _queue *_queue_data = NULL;

    int _queue_count;

    static int _next_num()
    {
        static int n = 0;
        return n++;
    }

    interesant *_opposite(interesant *i1, interesant *i2)
    {
        return (i1->next == i2) ? (i1->prev) : (i1->next);
    }

    void _put_in_place_of(interesant *i1, interesant *i2, interesant *i3)
    {
        if (i1->next == i2)
            i1->next = i3;
        else
            i1->prev = i3;
    }

    interesant *_direction(interesant *i1, interesant *i2)
    {
        interesant *dir1, *dir2, *h1, *h2, *p1, *p2, *t;
        h1 = dir1 = i1->next;
        h2 = dir2 = i1->prev;
        p1 = p2 = i1;
        bool found = false;
        while (!found)
        {
            if (h1 == NULL || h2 == i2)
            {
                dir1 = dir2;
                found = true;
            }
            else if (h2 == NULL || h1 == i2)
                found = true;
            else
            {
                t = h1;
                h1 = _opposite(h1, p1);
                p1 = t;
                t = h2;
                h2 = _opposite(h2, p2);
                p2 = t;
            }
        }
        return dir1;
    }
}

void otwarcie_urzedu(int m)
{
    _queue_count = m;
    _queue_data = (_queue *)malloc(sizeof(_queue) * m);
    for (int i = 0; i < m; i++)
        _queue::empty_queue(_queue_data + i);
}

interesant *nowy_interesant(int k)
{
    interesant *res = (interesant *)malloc(sizeof(interesant));
    res->id = _next_num();
    _queue_data[k].push(res);
    return res;
}

int numerek(interesant *i) { return i->id; }

interesant *obsluz(int k)
{
    if (_queue_data[k].is_empty())
        return NULL;
    interesant *i = _queue_data[k].head->next;
    _queue_data[k].head->next = _opposite(i, _queue_data[k].head);
    _put_in_place_of(_queue_data[k].head->next, i, _queue_data[k].head);
    return i;
}

void zmiana_okienka(interesant *i, int k)
{
    _put_in_place_of(i->next, i, i->prev);
    _put_in_place_of(i->prev, i, i->next);
    _queue_data[k].push(i);
}

void zamkniecie_okienka(int k1, int k2)
{
    if (_queue_data[k1].is_empty())
        return;
    interesant *head1 = _queue_data[k1].head, *tail1 = _queue_data[k1].tail;
    interesant *tail2 = _queue_data[k2].tail;
    _put_in_place_of(tail2->next, tail2, head1->next);
    _put_in_place_of(head1->next, head1, tail2->next);
    tail2->next = tail1->next;
    _put_in_place_of(tail2->next, tail1, tail2);
    head1->next = tail1;
    tail1->next = head1;
}

std::vector<interesant *> fast_track(interesant *i1, interesant *i2)
{
    std::vector<interesant *> res;
    res.push_back(i1);
    if (i1 != i2)
    {
        interesant *dir = _direction(i1, i2), *prev = _opposite(i1, dir), *t = i1, *temp;
        while (t != i2)
        {
            temp = t;
            t = _opposite(t, prev);
            prev = temp;
            res.push_back(t);
        }
        // connecting remainig nodes
        _put_in_place_of(_opposite(i1, dir), i1, _opposite(i2, prev));
        _put_in_place_of(_opposite(i2, prev), i2, _opposite(i1, dir));
    }
    else
    {
        _put_in_place_of(i1->prev, i1, i1->next);
        _put_in_place_of(i1->next, i1, i1->prev);
    }
    return res;
}

void naczelnik(int k)
{
    interesant *t = _queue_data[k].tail;
    _queue_data[k].tail = _queue_data[k].head;
    _queue_data[k].head = t;
}

std::vector<interesant *> zamkniecie_urzedu()
{
    std::vector<interesant *> res;
    for (int i = 0; i < _queue_count; i++)
    {
        interesant *prev = _queue_data[i].head, *node = _queue_data[i].head->next, *t;
        while (node != _queue_data[i].tail)
        {
            res.push_back(node);
            t = prev;
            prev = node;
            node = _opposite(node, t);
        }
        free(_queue_data[i].head);
        free(_queue_data[i].tail);
    }
    free(_queue_data);
    return res;
}
