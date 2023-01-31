#ifndef __GEOMETRY_ZALICZ3_HEADER
#define __GEOMETRY_ZALICZ3_HEADER

#include <math.h>
#include <stdbool.h>

typedef struct
{
    double x, y;
} Point_S;

typedef enum
{
    CIRCLE,
    RECTANGLE
} ShapeType_E;

typedef struct
{
    Point_S p0, p1;
} Rectangle_S;

typedef struct
{
    double r;
    Point_S o;
} Circle_S;

typedef union
{
    Rectangle_S rect;
    Circle_S circ;
} Shape_U;

typedef Rectangle_S FoldData_S;

typedef struct
{
    FoldData_S *fold_data;
    unsigned folds;
    ShapeType_E type;
    Shape_U fig;
} Shape_S;

int count_layers(Shape_S *shape, double x, double y);

void make_circ(Shape_S *target, double x0, double y0, double r);

void make_rect(Shape_S *target, double x0, double x1, double y0, double y1);

void cpy(Shape_S *target, Shape_S *shape);

void fold(Shape_S *shape, double x0, double x1, double y0, double y1);

// for safe deletion of Shape_s
// important: does not free the memory used by struct itself
void destroy(Shape_S *shape);

#endif
