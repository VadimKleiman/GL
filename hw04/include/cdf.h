#pragma once

#include <cmath>
#include <QImage>

struct Point {
    short dx, dy;
    int f;
};

struct Grid {
    int w, h;
    Point *grid;
};

Point pointInside = {0, 0, 0};
Point pointEmpty = {9999, 9999, 9999 * 9999};
Grid grid[2];

static inline Point Get(Grid &g, int x, int y) {
    return g.grid[y * (g.w + 2) + x];
}

static inline void Put(Grid &g, int x, int y, const Point &p) {
    g.grid[y * (g.w + 2) + x] = p;
}

static inline void Compare(Grid &g, Point &p, int x, int y, int offsetx, int offsety) {
    int add;
    Point other = Get(g, x + offsetx, y + offsety);
    if (offsety == 0) {
        add = 2 * other.dx + 1;
    } else if (offsetx == 0) {
        add = 2 * other.dy + 1;
    } else {
        add = 2 * (other.dy + other.dx + 1);
    }
    other.f += add;
    if (other.f < p.f) {
        p.f = other.f;
        if (offsety == 0) {
            p.dx = other.dx + 1;
            p.dy = other.dy;
        } else if (offsetx == 0) {
            p.dy = other.dy + 1;
            p.dx = other.dx;
        } else {
            p.dy = other.dy + 1;
            p.dx = other.dx + 1;
        }
    }
}

static void GenerateSDF(Grid &g) {
    for (int y = 1; y <= g.h; y++) {
        for (int x = 1; x <= g.w; x++) {
            Point p = Get(g, x, y);
            Compare(g, p, x, y, -1, 0);
            Compare(g, p, x, y, 0, -1);
            Compare(g, p, x, y, -1, -1);
            Compare(g, p, x, y, 1, -1);
            Put(g, x, y, p);
        }
    }

    for (int y = g.h; y > 0; y--) {
        for (int x = g.w; x > 0; x--) {
            Point p = Get(g, x, y);
            Compare(g, p, x, y, 1, 0);
            Compare(g, p, x, y, 0, 1);
            Compare(g, p, x, y, -1, 1);
            Compare(g, p, x, y, 1, 1);
            Put(g, x, y, p);
        }
    }
}

QImage dfcalculate(QImage &&img, int distanceFieldScale, int width, int height) {
    int x, y;
    int w = img.width(), h = img.height();
    grid[0].w = grid[1].w = w;
    grid[0].h = grid[1].h = h;
    grid[0].grid = (Point *) malloc(sizeof(Point) * (w + 2) * (h + 2));
    grid[1].grid = (Point *) malloc(sizeof(Point) * (w + 2) * (h + 2));
    for (x = 0; x < w + 2; x++) {
        Put(grid[0], x, 0, pointInside);
        Put(grid[1], x, 0, pointEmpty);
    }
    for (y = 1; y <= h; y++) {
        Put(grid[0], 0, y, pointInside);
        Put(grid[1], 0, y, pointEmpty);
        for (x = 1; x <= w; x++) {
            if (qGreen(img.pixel(x - 1, y - 1)) > 128) {
                Put(grid[0], x, y, pointEmpty);
                Put(grid[1], x, y, pointInside);
            } else {
                Put(grid[0], x, y, pointInside);
                Put(grid[1], x, y, pointEmpty);
            }
        }
        Put(grid[0], w + 1, y, pointInside);
        Put(grid[1], w + 1, y, pointEmpty);
    }
    for (x = 0; x < w + 2; x++) {
        Put(grid[0], x, h + 1, pointInside);
        Put(grid[1], x, h + 1, pointEmpty);
    }
    GenerateSDF(grid[0]);
    GenerateSDF(grid[1]);
    QImage out(w, h, QImage::Format_ARGB32);
    for (y = 1; y <= h; y++)
        for (x = 1; x <= w; x++) {
            double dist1 = sqrt((double) (Get(grid[0], x, y).f + 1));
            double dist2 = sqrt((double) (Get(grid[1], x, y).f + 1));
            double dist = dist1 - dist2;
            int c = dist * 64 / distanceFieldScale + 128;
            if (c < 0) c = 0;
            if (c > 255) c = 255;
            out.setPixel(x - 1, y - 1, qRgb(c, c, c));
        }
    free(grid[0].grid);
    free(grid[1].grid);
    return out.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}