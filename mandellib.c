#include <stdio.h>
#include "mandellib.h"

#define FLOAT float
#define PAL_SIZE 64
int max_iter = 32;
FLOAT range_x1 = -1.5;
FLOAT range_x2 = -0.5;
FLOAT range_y1 = -0.5;
FLOAT range_y2 = 0.5;
FLOAT step_x, step_y;

typedef struct {
    FLOAT r;
    FLOAT i;
} point_t;

FLOAT abs2(point_t *p) {
    return p->r * p->r + p->i * p->i;
}

/* (ac − bd) + (ad + bc)i */
void mult(point_t *n1, point_t *n2, point_t *res) {
    res->r = n1->r * n2->r - n1->i*n2->i;
    res->i = n1->r * n2->i + n1->i*n2->r;
}

void add(point_t *n1, point_t *n2, point_t *res) {
    res->r = n1->r + n2->r;
    res->i = n1->i + n2->i;
}

int calc_point(point_t *p) {
    point_t t, z;
    int iter = 1;
    z = *p;
    while ((abs2(&z) < 4) && (iter < max_iter)) {
	mult(&z, &z, &t);
	add(&t, p, &z);
	iter++;
    }
    if (iter < max_iter) {
	return (iter % (PAL_SIZE - 1)) + 1;
    }
    return 0;
}

void mandel_init(int x, int y)
{
    step_x = (range_x2 - range_x1) / x;
    step_y = (range_y2 - range_y1) / y;
    printf("rangex: %02.2f - %2.2f\nrangey: %02.2f - %2.2f\nstepx: %2.4f\nstepy: %02.4f\n",
	   range_x1, range_x2, range_y1, range_y2, step_x, step_y);
}

void mandel_update(int x1, int y1, int x2, int y2)
{
    printf("Zoom to: (%d,%d) x (%d,%d)\n", x1, y1, x2, y2);
    range_x1 += (x1 * step_x);
    range_x2 = range_x1 + ((x2 - x1) * step_x);
    range_y1 += (y1 * step_y);
    range_y2 = range_y1 + ((y2 - y1) * step_y);
}

int mandel(int x, int y)
{
    point_t p = {x * step_x + range_x1, y * step_y + range_y1};
    return calc_point(&p);
}
