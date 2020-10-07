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
#include "Scene.h"

#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Mesh.h"

#include <limits>
#include <map>
#include <functional>
#include <stdexcept>

#ifdef _OPENMP
#  include <omp.h>
#endif

//-----------------------------------------------------------------------------


Image Scene::render()
{
    // allocate new image.
    Image img(camera.width, camera.height);

    // Function rendering a full column of the image
    auto raytraceColumn = [&img, this](int x) {
        for (int y=0; y<int(camera.height); ++y)
        {
            Ray ray = camera.primary_ray(x,y);

            // compute color by tracing this ray
            vec3 color = trace(ray, 0);

            // avoid over-saturation
            color = min(color, vec3(1, 1, 1));

            // store pixel color
            img(x,y) = color;
        }
    };

    // If possible, raytrace image columns in parallel.
    //
    // Mac users beware:
    // XCode's compiler (based on clang) does not support OpenMP
    // - you will have to install a vanialla clang (or gcc) if you want to use
    // OpenMP parallelization, e.g. via MacPorts.

#ifdef _OPENMP
    std::cout << "Rendering with up to " << omp_get_max_threads() << " threads." << std::endl;
#  pragma omp parallel for
#else
    std::cout << "Rendering singlethreaded." << std::endl;
#endif
    for (int x=0; x<int(camera.width); ++x)
        raytraceColumn(x);

    // Note: compiler will elide copy.
    return img;
}

//-----------------------------------------------------------------------------

vec3 Scene::trace(const Ray& _ray, int _depth)
{
    // stop if recursion depth (=number of reflection) is too large
    if (_depth > max_depth) return vec3(0,0,0);

    // Find first intersection with an object. If an intersection is found,
    // it is stored in object, point, normal, and t.
    Object_ptr  object;
    vec3        point;
    vec3        normal;
    double      t;
    if (!intersect(_ray, object, point, normal, t))
    {
        return background;
    }

    // compute local Phong lighting (ambient+diffuse+specular)
    vec3 color = lighting(point, normal, -_ray.direction, object->material);


    /** \todo
     * Compute reflections by recursive ray tracing:
     * - check whether `object` is reflective by checking its `material.mirror`
     * - check recursion depth
     * - generate reflected ray, compute its color contribution, and mix it with
     * the color computed by local Phong lighting (use `object->material.mirror` as weight)
     * - check whether your recursive algorithm reflects the ray `max_depth` times
     */
    if(object->material.mirror > 0) {
        color += trace(Ray(point + normal * 1e-10, reflect(_ray.direction, normal)),_depth + 1) * object->material.mirror;
    }

    return color;
}

//-----------------------------------------------------------------------------

bool Scene::intersect(const Ray& _ray, Object_ptr& _object, vec3& _point, vec3& _normal, double& _t)
{
    double  t, tmin(Object::NO_INTERSECTION);
    vec3    p, n;

    for (const auto &o: objects) // for each object
    {
        if (o->intersect(_ray, p, n, t)) // does ray intersect object?
        {
            if (t < tmin) // is intersection point the currently closest one?
            {
                tmin = t;
                _object = o.get();
                _point  = p;
                _normal = n;
                _t      = t;
            }
        }
    }

    return (tmin != Object::NO_INTERSECTION);
}

vec3 Scene::lighting(const vec3& _point, const vec3& _normal, const vec3& _view, const Material& _material)
{

     /** \todo
     * Compute the Phong lighting:
     * - start with global ambient contribution
     * - for each light source (stored in vector `lights`) add diffuse and specular contribution
     * - only add diffuse and specular light if object is not in shadow
     *
     * You can look at the classes `Light` and `Material` to check their attributes. Feel free to use
     * the existing vector functions in vec3.h e.g. mirror, reflect, norm, dot, normalize
     */

    // visualize the normal as a RGB color for now.
    //vec3 color = (_normal + vec3(1)) / 2.0;

    vec3 color = ambience * _material.ambient;
    for (auto light : lights) {
        vec3 l = light.position - _point;
        double distance = norm(l);
        l /= distance;

        bool no_shadow{true};
        for(auto &object : objects) {
            vec3       _intersection_point;
            vec3       _intersection_normal;
            double     _intersection_t;

            bool intersect = object->intersect(Ray(_point + l * 1e-10, l), _intersection_point, _intersection_normal, _intersection_t);
            if(intersect && _intersection_t < distance) {
                no_shadow = false;
                break;
            }
        }

        if(no_shadow) {
            vec3 r = -reflect(l, _normal);
            double n_l = dot(_normal, l);
            double r_v = dot(r, _view);

            if(n_l > 0) color += _material.diffuse * light.color * n_l;
            if(r_v > 0) color += _material.specular * light.color * pow(r_v, _material.shininess);
        }
    }


    return color;
}

//-----------------------------------------------------------------------------

void Scene::read(const std::string &_filename)
{
    std::ifstream ifs(_filename);
    if (!ifs)
        throw std::runtime_error("Cannot open file " + _filename);

    const std::map<std::string, std::function<void(void)>> entityParser = {
        {"depth",      [&]() { ifs >> max_depth; }},
        {"camera",     [&]() { ifs >> camera; }},
        {"background", [&]() { ifs >> background; }},
        {"ambience",   [&]() { ifs >> ambience; }},
        {"light",      [&]() { lights .emplace_back(ifs); }},
        {"plane",      [&]() { objects.emplace_back(new    Plane(ifs)); }},
        {"sphere",     [&]() { objects.emplace_back(new   Sphere(ifs)); }},
        {"cylinder",   [&]() { objects.emplace_back(new Cylinder(ifs)); }},
        {"mesh",       [&]() { objects.emplace_back(new     Mesh(ifs, _filename)); }}
    };

    // parse file
    std::string token;
    while (ifs && (ifs >> token) && (!ifs.eof())) {
        if (token[0] == '#') {
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (entityParser.count(token) == 0)
            throw std::runtime_error("Invalid token encountered: " + token);
        entityParser.at(token)();
    }
}


//=============================================================================
