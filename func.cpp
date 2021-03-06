//
// Created on 2018/12/12.
//

#include <math.h>
#include <cstdio>
#include "func.hpp"
#include "main.hpp"
#include "method.hpp"

double toRad(double r) { return r * M_PI / 180.0; }

double toFreq(double r) { return r * 180.0 / M_PI; }

double toDot(double r) { return r * 3.6 / 60.0 / 60.0; }

double toKm(double r) { return r * 60.0 * 60.0 / 3.6; }

extern int TIME[3];
double expect_d = 0.0;

void TimePlus() {
    if (TIME[0] < 59) { TIME[0]++; }
    else {
        TIME[0] = 0;
        if (TIME[1] < 59) { TIME[1]++; }
        else {
            TIME[1] = 0;
            if (TIME[2] < 23) { TIME[2]++; } else { TIME[2] = 0; }
        }
    }
}

void TerritoryDef(double x, double X, double Y, double alpha, double height, double v, double *front,
                  double *behind, double *right, double *left, double *over, double *under) {
    double dist = (v * territoryFrotBehind) / 2.0;
    *front = x * tan(M_PI - alpha) + (Y + dist * sin(alpha) - (X + dist * cos(alpha)) * tan(M_PI - alpha));
    *behind = x * tan(M_PI - alpha) + (Y - dist * sin(alpha) - (X - dist * cos(alpha)) * tan(M_PI - alpha));
    *right =
            x * tan(alpha) +
            (Y + territorySide / 2.0 * sin(alpha - M_PI / 2.0) -
             (X + territorySide / 2.0 * cos(alpha - M_PI / 2.0)) * tan(alpha));
    *left = x * tan(alpha) +
            (Y + territorySide / 2.0 * sin(M_PI / 2.0 + alpha) -
             (X + territorySide / 2.0 * cos(M_PI / 2.0 + alpha)) * tan(alpha));
    *over = height + territoryHeight / 2.0;
    *under = height - territoryHeight / 2.0;
}

//前方(右上1,左上2,左下3,右下4)、後方5、存在なし0
void CheckTerritory(struct _State *checkPlane, struct _State *others, int i, int *area) {
    for (int k = 0; k < 6; ++k) { area[k] = 0; }
    int flag = 1, outcount = 0;
    int outplane[N];
    for (int l = 0; l < N; ++l) { outplane[l] = 0; }
    double front, behind, right, left, over, under;
    double X = checkPlane->x, Y = checkPlane->y, alpha = checkPlane->direction,
            height = checkPlane->height, v = checkPlane->velocity;
    for (int j = 0; j < N; ++j) {
        if (j == i) { continue; }
        else {
            TerritoryDef(others[j].x, X, Y, alpha, height, v, &front, &behind, &right, &left, &over, &under);
            double x = others[j].x, y = others[j].y, z = others[j].height;
            if ((y <= front && y >= behind) && (y <= left && y >= right) && (z <= over && z >= under)) {
                if (checkPlane->Turning) {
                    checkPlane->Turning = 0;
                    checkPlane->phase = 0;
                }
                flag = 0;
                if (y >= x * tan(alpha) + Y - X * tan(alpha)) {
                    outplane[j] = 1;
                    outcount++;
                    if (y <= x * tan(alpha) + Y - X * tan(alpha)) {
                        if (z >= height) { area[1] = 1; } else { area[4] = 1; }
                    } else {
                        if (z >= height) { area[2] = 1; } else { area[3] = 1; }
                    }
                } else { area[5] = 1; }
            }
        }
    }
    if (flag) { area[0] = 1; }
    if ((area[1] || area[4]) && (area[2] || area[3])) {
        double sum = 0.0;
        for (int j = 0; j < N; ++j) {
            if (outplane[j]) {
                double alpha = atan2(others[j].y - checkPlane->y, others[j].y - checkPlane->y);
                if (abs(alpha - checkPlane->direction) > M_PI) {
                    if (alpha >= 0) { alpha = 2 * M_PI - alpha; } else { alpha += M_PI * 2; }
                }
                sum += alpha;
            }
        }
        expect_d = sum / (double) outcount;
    }
}

void ChangeWaitOrder(int *order, int *phase, int *turning) {
    if (*order) {
        *phase = 1;
    } else {
        *phase = 0;
        *turning = 0;
    }
}