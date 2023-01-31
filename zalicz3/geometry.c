#include "geometry.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const double EPSILON = 1e-6;

static bool _eq(double a, double b) { return fabs(a - b) < EPSILON; }

// distance between two points
static double _dist(Point_S a, Point_S b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// point mirrored by line
static Point_S _mirror(FoldData_S *line, Point_S *point)
{
    Point_S new;
    if (_eq(line->p0.x, line->p1.x))
    {
        new.y = point->y;
        new.x = 2 * line->p0.x - point->x;
    }
    else if (_eq(line->p0.y, line->p1.y))
    {
        new.x = point->x;
        new.y = 2 * line->p1.y - point->y;
    }
    else
    {
        double a1, a2, b1, b2; // coefficients of given line and line perpendicular to it and containing p
        a1 = (line->p0.y - line->p1.y) / (line->p0.x - line->p1.x);
        a2 = -1. / a1;
        b1 = line->p0.y - a1 * line->p0.x;
        b2 = point->y - a2 * point->x;
        double xp, yp; // coordinates of intersection of lines
        xp = (b1 - b2) / (a2 - a1);
        yp = a1 * xp + b1;
        new.x = 2 * xp - point->x;
        new.y = 2 * yp - point->y;
    }
    return new;
}

// returns positive if point is on the left side of line, negative if on the right and 0 if on line
static double _side(FoldData_S *line, Point_S *point)
{
    return (line->p1.x - line->p0.x) * (point->y - line->p0.y) - (point->x - line->p0.x) * (line->p1.y - line->p0.y);
}

void make_circ(Shape_S *target, double x0, double y0, double r)
{
    target->fold_data = (FoldData_S *)0;
    target->folds = 0u;
    target->type = CIRCLE;
    target->fig = (Shape_U){.circ = {.o = (Point_S){.x = x0, .y = y0}, .r = r}};
}

void make_rect(Shape_S *target, double x0, double x1, double y0, double y1)
{
    *target = (Shape_S){
        .fold_data = (FoldData_S *)0,
        .folds = 0u,
        .type = RECTANGLE,
        .fig = (Shape_U){.rect = {.p0 = (Point_S){.x = x0, .y = y0}, .p1 = (Point_S){.x = x1, .y = y1}}}};
}

void cpy(Shape_S *target, Shape_S *shape)
{
    // copy Shape struct
    memcpy(target, shape, sizeof(Shape_S));
    // copy folds (if exist)
    if (shape->fold_data != NULL)
    {
        target->fold_data = (FoldData_S *)malloc(sizeof(FoldData_S) * target->folds);
        memcpy(target->fold_data, shape->fold_data, sizeof(FoldData_S) * target->folds);
    }
}

void destroy(Shape_S *shape)
{
    if (shape->folds)
        free(shape->fold_data);
}

// adds new fold to existing shape
void fold(Shape_S *shape, double x0, double x1, double y0, double y1)
{
    if (shape->folds == 0u)
        shape->fold_data = (FoldData_S *)malloc(sizeof(FoldData_S));
    else
        shape->fold_data = realloc(shape->fold_data, (shape->folds + 1) * sizeof(FoldData_S));
    shape->fold_data[shape->folds++] = (FoldData_S){
        .p0 = {.x = x0, .y = y0},
        .p1 = {.x = x1, .y = y1}};
}

static int _count_layers(Shape_S *shape, Point_S p, int end_on)
{
    // if some folds are still not processed call _count_layers recursively
    if (end_on > -1)
    {
        double d = _side(shape->fold_data + (unsigned)end_on, &p);
        // if point is on line call once
        if (_eq(d, 0.))
        {
            return _count_layers(shape, p, end_on - 1);
        }
        // if on the left, call twice
        else if (d > 0)
        {
            return _count_layers(shape, p, end_on - 1) +
                   _count_layers(shape, _mirror(&(shape->fold_data[(unsigned)end_on]), &p), end_on - 1);
        }
        // if on the right return 0
        else
            return 0;
    }
    // simple checks if point is inside shape
    else
    {
        if (shape->type == RECTANGLE)
        {
            Point_S p1 = shape->fig.rect.p0, p2 = shape->fig.rect.p1;
            return p1.x <= p.x && p.x <= p2.x && p1.y <= p.y && p.y <= p2.y;
        }
        else
        {
            return _dist(shape->fig.circ.o, p) <= shape->fig.circ.r;
        }
    }
}

int count_layers(Shape_S *shape, double x, double y)
{
    // printf("counting layers for shape of type %i with %u folds:\n", shape -> type, shape -> folds);
    //  for (int i = 0; i < (int)shape -> folds; i++){
    //      printf("%lf %lf %lf %lf\n", shape -> fold_data[i].p0.x, shape -> fold_data[i].p0.y, shape -> fold_data[i].p1.x, shape -> fold_data[i].p1.y);
    //  }
    return _count_layers(shape, (Point_S){x, y}, ((int)shape->folds) - 1);
}
