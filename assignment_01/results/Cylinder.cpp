//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================

//== INCLUDES =================================================================

#include "Cylinder.h"
#include "SolveQuadratic.h"

#include <array>
#include <cmath>

//== IMPLEMENTATION =========================================================

bool
Cylinder::
intersect(const Ray&  _ray,
          vec3&       _intersection_point,
          vec3&       _intersection_normal,
          double&     _intersection_t) const
{
    /** \todo
     * - compute the first valid intersection `_ray` with the cylinder
     *   (valid means in front of the viewer: t > 0)
     * - store intersection point in `_intersection_point`
     * - store ray parameter in `_intersection_t`
     * - store normal at _intersection_point in `_intersection_normal`.
     * - return whether there is an intersection with t > 0
    */

    const vec3 &dir = _ray.direction;
    const vec3   oc = _ray.origin - center;

    std::array<double, 2> t;
    size_t nsol = solveQuadratic(dot(dir, dir) - dot(axis, dir) * dot(axis, dir),
                                 2 * (dot(dir, oc) - dot(axis, dir) * dot(axis, oc)),
                                 dot(oc, oc) - dot(axis, oc) * dot(axis, oc) - radius * radius, t);

    _intersection_t = NO_INTERSECTION;

    // Find the closest valid solution (in front of the viewer)
    for (size_t i = 0; i < nsol; ++i) {
        if (t[i] <= 0) continue;
        if (2 * abs(dot(_ray(t[i]) - center, axis)) < height)
        {
            _intersection_t = std::min(_intersection_t, t[i]);
        }
    }

    if (_intersection_t == NO_INTERSECTION) return false;

    _intersection_point  = _ray(_intersection_t);
    _intersection_normal = cross(cross(axis, _intersection_point-center),axis);
    _intersection_normal = _intersection_normal / norm(_intersection_normal);

    if(dot(_intersection_normal, _ray.direction) > 0)
    {
        _intersection_normal = _intersection_normal * -1;
    }

    return true;
}
