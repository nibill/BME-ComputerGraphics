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

#include "Mesh.h"
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>
#include <cmath>


//== IMPLEMENTATION ===========================================================


Mesh::Mesh(std::istream &is, const std::string &scenePath)
{
    std::string meshFile, mode;
    is >> meshFile;

    const char pathSep = '/';


    // load mesh from file
    read(scenePath.substr(0, scenePath.find_last_of(pathSep) + 1) + meshFile);

    is >> mode;
    if      (mode ==  "FLAT") draw_mode_ = FLAT;
    else if (mode == "PHONG") draw_mode_ = PHONG;
    else throw std::runtime_error("Invalid draw mode " + mode);

    is >> material;
}


//-----------------------------------------------------------------------------


bool Mesh::read(const std::string &_filename)
{
    // read a mesh in OFF format


    // open file
    std::ifstream ifs(_filename);
    if (!ifs)
    {
        std::cerr << "Can't open " << _filename << "\n";
        return false;
    }


    // read OFF header
    std::string s;
    unsigned int nV, nF, dummy, i;
    ifs >> s;
    if (s != "OFF")
    {
        std::cerr << "No OFF file\n";
        return false;
    }
    ifs >> nV >> nF >> dummy;
    std::cout << "\n  read " << _filename << ": " << nV << " vertices, " << nF << " triangles";


    // read vertices
    Vertex v;
    vertices_.clear();
    vertices_.reserve(nV);
    for (i=0; i<nV; ++i)
    {
        ifs >> v.position;
        vertices_.push_back(v);
    }


    // read triangles
    Triangle t;
    triangles_.clear();
    triangles_.reserve(nF);
    for (i=0; i<nF; ++i)
    {
        ifs >> dummy >> t.i0 >> t.i1 >> t.i2;
        triangles_.push_back(t);
    }


    // close file
    ifs.close();


    // compute face and vertex normals
    compute_normals();

    // compute bounding box
    compute_bounding_box();


    return true;
}


//-----------------------------------------------------------------------------

// Determine the weights by which to scale triangle (p0, p1, p2)'s normal when
// accumulating the vertex normals for vertices 0, 1, and 2.
// (Recall, vertex normals are a weighted average of their incident triangles'
// normals, and in our raytracer we'll use the incident angles as weights.)
// \param[in] p0, p1, p2    triangle vertex positions
// \param[out] w0, w1, w2    weights to be used for vertices 0, 1, and 2
void angleWeights(const vec3 &p0, const vec3 &p1, const vec3 &p2,
                  double &w0, double &w1, double &w2) {
    // compute angle weights
    const vec3 e01 = normalize(p1-p0);
    const vec3 e12 = normalize(p2-p1);
    const vec3 e20 = normalize(p0-p2);
    w0 = acos( std::max(-1.0, std::min(1.0, dot(e01, -e20) )));
    w1 = acos( std::max(-1.0, std::min(1.0, dot(e12, -e01) )));
    w2 = acos( std::max(-1.0, std::min(1.0, dot(e20, -e12) )));
}


//-----------------------------------------------------------------------------

void Mesh::compute_normals()
{
    // compute triangle normals
    for (Triangle& t: triangles_)
    {
        double w0, w1, w2;

        const vec3& p0 = vertices_[t.i0].position;
        const vec3& p1 = vertices_[t.i1].position;
        const vec3& p2 = vertices_[t.i2].position;
        t.normal = normalize(cross(p1-p0, p2-p0));

        angleWeights(p0, p1, p2, w0, w1, w2);

        vertices_[t.i0].normal += w0 * t.normal;
        vertices_[t.i1].normal += w1 * t.normal;
        vertices_[t.i2].normal += w2 * t.normal;
    }

    // initialize vertex normals to zero
    for (Vertex& v: vertices_)
    {
        v.normal = normalize(v.normal);
    }

    /** \todo
     * In some scenes (e.g the office scene) some objects should be flat
     * shaded (e.g. the desk) while other objects should be Phong shaded to appear
     * realistic (e.g. chairs). You have to implement the following:
     * - Compute vertex normals by averaging the normals of their incident triangles.
     * - Store the vertex normals in the Vertex::normal member variable.
     * - Weigh the normals by their triangles' angles.
     */
    

}


//-----------------------------------------------------------------------------


void Mesh::compute_bounding_box()
{
    bb_min_ = vec3(std::numeric_limits<double>::max());
    bb_max_ = vec3(std::numeric_limits<double>::lowest());

    for (Vertex v: vertices_)
    {
        bb_min_ = min(bb_min_, v.position);
        bb_max_ = max(bb_max_, v.position);
    }
}


//-----------------------------------------------------------------------------


bool Mesh::intersect_bounding_box(const Ray& _ray) const
{

    /** \todo
    * Intersect the ray `_ray` with the axis-aligned bounding box of the mesh.
    * Note that the minimum and maximum point of the bounding box are stored
    * in the member variables `bb_min_` and `bb_max_`. Return whether the ray
    * intersects the bounding box.
    * This function is ued in `Mesh::intersect()` to avoid the intersection test
    * with all triangles of every mesh in the scene. The bounding boxes are computed
    * in `Mesh::compute_bounding_box()`.
    */
   
   //these must be outside the loop!!!
   double __min = 0;
   double __max = INFINITY;

   double _under = 0, _upper = 0;

   auto _switch = [](double &a, double &b) {
        double _temp = a;
        a = b;
        b = _temp;
   };

   _under = (bb_min_[0] - _ray.origin[0]) / _ray.direction[0];
   _upper = (bb_max_[0] - _ray.origin[0]) / _ray.direction[0];
   if (_upper < _under) _switch(_under, _upper);
   __min = fmax(__min, _under);
   __max = fmin(__max, _upper);
   if (__max < __min) return false;

   _under = (bb_min_[1] - _ray.origin[1]) / _ray.direction[1];
   _upper = (bb_max_[1] - _ray.origin[1]) / _ray.direction[1];
   if (_upper < _under) _switch(_under, _upper);
   __min = fmax(__min, _under);
   __max = fmin(__max, _upper);
   if (__max < __min) return false;

   _under = (bb_min_[2] - _ray.origin[2]) / _ray.direction[2];
   _upper = (bb_max_[2] - _ray.origin[2]) / _ray.direction[2];
   if (_upper < _under) _switch(_under, _upper);
   __min = fmax(__min, _under);
   __max = fmin(__max, _upper);
   if (__max < __min) return false;
   
   return true;
}


//-----------------------------------------------------------------------------


bool Mesh::intersect(const Ray& _ray,
                     vec3&      _intersection_point,
                     vec3&      _intersection_normal,
                     double&    _intersection_t ) const
{
    // check bounding box intersection
    if (!intersect_bounding_box(_ray))
    {
        return false;
    }

    vec3   p, n;
    double t;

    _intersection_t = NO_INTERSECTION;

    // for each triangle
    for (const Triangle& triangle : triangles_)
    {
        // does ray intersect triangle?
        if (intersect_triangle(triangle, _ray, p, n, t))
        {
            // is intersection closer than previous intersections?
            if (t < _intersection_t)
            {
                // store data of this intersection
                _intersection_t      = t;
                _intersection_point  = p;
                _intersection_normal = n;
            }
        }
    }

    return (_intersection_t != NO_INTERSECTION);
}


//-----------------------------------------------------------------------------


bool
Mesh::
intersect_triangle(const Triangle&  _triangle,
                   const Ray&       _ray,
                   vec3&            _intersection_point,
                   vec3&            _intersection_normal,
                   double&          _intersection_t) const
{
    const vec3& p0 = vertices_[_triangle.i0].position;
    const vec3& p1 = vertices_[_triangle.i1].position;
    const vec3& p2 = vertices_[_triangle.i2].position;

    /** \todo
    * - intersect _ray with _triangle
    * - store intersection point in `_intersection_point`
    * - store ray parameter in `_intersection_t`
    * - store normal at intersection point in `_intersection_normal`.
    * - Depending on the member variable `draw_mode_`, use either the triangle
    *  normal (`Triangle::normal`) or interpolate the vertex normals (`Vertex::normal`).
    * - return `true` if there is an intersection with t > 0 (in front of the viewer)
    *
    * Hint: Rearrange `ray.origin + t*ray.dir = a*p0 + b*p1 + (1-a-b)*p2` to obtain a solvable
    * system for a, b and t.
    * Refer to [Cramer's Rule](https://en.wikipedia.org/wiki/Cramer%27s_rule) to easily solve it.
    */

    vec3 _a = - _ray.direction;
    vec3 _b = p0 - p2;
    vec3 _c = p1 - p2;

    vec3  _res = _ray.origin - p2;

    auto det = [](vec3 a, vec3 b, vec3 c){ return dot(cross(a, b), c); };

    double _det_a = det(_a, _b, _c);
    if (fabs(_det_a) <= - DBL_MAX) return false;

    double _alpha = det(_a, _res, _c) / _det_a;
    double _beta = det(_a, _b, _res) / _det_a;
    double _gamma = 1.0 - _alpha - _beta;

    if(!(_alpha >= 0.0 && _beta >= 0.0 && _gamma >= 0.0 && _alpha + _beta + _gamma <= 1.0 + 1.0e10 && _alpha + _beta + _gamma >= 1.0 - 1.0e10)) return false;

    double t = det(_res, _b, _c) / _det_a;
    if (t <= 0) return false;

    if(draw_mode_ == 0)
    {
        _intersection_normal = _triangle.normal;
    }

    if(draw_mode_ == 1)
    {
        vec3 _x = vertices_[_triangle.i0].normal;
        vec3 _y = vertices_[_triangle.i1].normal;
        vec3 _z = vertices_[_triangle.i2].normal;
        _intersection_normal = normalize(_x*_alpha + _y*_beta + _z*_gamma);
    }

    _intersection_t      = t;
    _intersection_point  = _ray(t);

    return true;
}


//=============================================================================
