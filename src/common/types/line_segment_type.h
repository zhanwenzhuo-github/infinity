//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "internal_types.h"
#include "point_type.h"

namespace infinity {

// A line segment is specified by two endpoints.
struct LineSegmentType {
    PointType point1;
    PointType point2;
};

}