//
// Created by Lian chenyu on 2018-12-04.
//

#ifndef ENEYES_IWATCH_API_IWATCH_COMMON_H
#define ENEYES_IWATCH_API_IWATCH_COMMON_H

#include <stdint.h>

typedef struct iWatch_Point {
    int x;
    int y;
} iWatch_Point;

typedef struct iWatch_Size {
    int width;
    int height;
} iWatch_Size;

typedef struct iWatch_Rect {
    int x;
    int y;
    int width;
    int height;
} iWatch_Rect;

#define IWATCH_MAX_CONTOUR_ROI_POINT_COUNT 10   //temporary and undetermined

typedef struct iWatch_Contour {
    iWatch_Point contour_roi_points[IWATCH_MAX_CONTOUR_ROI_POINT_COUNT];
    int contour_roi_point_count;
} iWatch_Contour;

typedef struct iWatch_Region {
    uint64_t id;
    iWatch_Contour region;
} iWatch_Region;

#endif //IWATCH_API_IWATCH_COMMON_H
