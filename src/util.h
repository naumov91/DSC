//
//  Deformabel Simplicial Complex (DSC) method
//  Copyright (C) 2013  Technical University of Denmark
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  See licence.txt for a copy of the GNU General Public License.

#pragma once

#include <vector>
#include <list>
#include <sstream>
#include <cmath>
#include <cassert>

#include "CGLA_API.h"

constexpr double EPSILON = 1e-8;

#ifndef INFINITY
constexpr double INFINITY = 1e32;
#endif

namespace Util
{
    template <typename vec3>
    inline vec3 normal_direction(const vec3& a, const vec3& b, const vec3& c);
    
    
    template <typename real>
    inline int sign(real val)
    {
        return (0. < val) - (val < 0.);
    }
    
    /**
     * Computes the signed area of the triangle spanned by vertices with positions v0, v1 and v2.
     */
    template <typename real, typename vec3>
    inline real signed_area(const vec3& v0, const vec3& v1, const vec3& v2)
    {
        vec3 n = cross(v1-v0, v2-v0);
        return 0.5 * n.length();
    }
    
    /**
     * Computes the area of the triangle spanned by vertices with positions v0, v1 and v2.
     */
    template <typename real, typename vec3>
    inline real area(const vec3& v0, const vec3& v1, const vec3& v2)
    {
        return std::abs(signed_area<real>(v0, v1, v2));
    }
    
    template <typename real, typename vec3>
    inline real signed_volume(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        return dot(a-d, cross(c-d, b-d))/6.;
    }
    
    template <typename real, typename vec3>
    inline real volume(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        return std::abs(signed_volume<real>(a, b, c, d));
    }
    
    /**
     * Calculates the cosine of the angle between the line segments |ab| and |ac|.
     */
    template <typename real, typename vec3>
    inline real cos_angle(const vec3& a, const vec3& b, const vec3& c)
    {
        vec3 ab = normalize(b - a);
        vec3 ac = normalize(c - a);
        return dot(ab, ac);
    }
    
    /**
     * Calculates the angle between the line segments |ab| and |ac|.
     */
    template <typename real, typename vec3>
    inline real angle(const vec3& a, const vec3& b, const vec3& c)
    {
        return acos(cos_angle<real>(a, b, c));
    }
    
    /**
     * Calculate the cosine of angles in the triangle defined by the vertices a, b and c.
     */
    template <typename real, typename vec3>
    inline std::vector<real> cos_angles(const vec3& a, const vec3& b, const vec3& c)
    {
        std::vector<real> cosines(3);
        cosines[0] = cos_angle<real>(a, b, c);
        cosines[1] = cos_angle<real>(b, c, a);
        cosines[2] = cos_angle<real>(c, a, b);
        return cosines;
    }
    
    template <typename real, typename vec3>
    inline real min_angle(const vec3& a, const vec3& b, const vec3& c)
    {
        std::vector<real> cosines = cos_angles<real>(a, b, c);
        real max_cos = -1.;
        for(auto cos : cosines)
        {
            max_cos = std::max(cos, max_cos);
        }
        return acos(max_cos);
    }
    
    template <typename real, typename vec3>
    inline real max_angle(const vec3& a, const vec3& b, const vec3& c)
    {
        std::vector<real> cosines = cos_angles<real>(a, b, c);
        double min_cos = 1.;
        for(auto cos : cosines)
        {
            min_cos = std::min(cos, min_cos);
        }
        return std::acos(min_cos);
    }
    
    /**
     * Returns the cosine to the dihedral angle between face |abc| and face |abd|.
     */
    template<typename real, typename vec3>
    inline real cos_dihedral_angle(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {        
        vec3 n0 = normal_direction(a, b, c);
        vec3 n1 = normal_direction(b, a, d);
        real angle = dot(n0, n1);
#ifdef DEBUG
        assert(angle < 1. + EPSILON);
        assert(angle > -1. - EPSILON);
#endif
        return angle;
    }
    
    /**
     * Returns the dihedral angle between face |abc| and face |abd|.
     */
    template<typename real, typename vec3>
    inline real dihedral_angle(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        return std::acos(cos_dihedral_angle<real>(a, b, c, d));
    }
    
    template <typename vec3>
    inline vec3 barycenter(const vec3& a, const vec3& b)
    {
        return (a + b)*0.5;
    }
    
    template <typename vec3>
    inline vec3 barycenter(const vec3& a, const vec3& b, const vec3& c)
    {
        return (a + b + c)/3.;
    }
    
    template <typename vec3>
    inline vec3 barycenter(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        return (a + b + c + d)*0.25;
    }
    
    /**
     * Finds the barycentric coordinates of point v in a triangle spanned by the vertices a, b and c.
     */
    template <typename real, typename vec3>
    inline std::vector<real> barycentric_coords(const vec3& p, const vec3& a, const vec3& b, const vec3& c)
    {
        std::vector<real> coords(3);
        
        vec3 v0 = b - a;
        vec3 v1 = c - a;
        vec3 v2 = p - a;
        real d00 = dot(v0, v0);
        real d01 = dot(v0, v1);
        real d11 = dot(v1, v1);
        real d20 = dot(v2, v0);
        real d21 = dot(v2, v1);
        real denom = d00 * d11 - d01 * d01;
#ifdef DEBUG
        assert(denom != 0.);
#endif
        coords[0] = (d11 * d20 - d01 * d21) / denom;
        coords[1] = (d00 * d21 - d01 * d20) / denom;
        coords[2] = 1. - coords[0] - coords[1];
           
        return coords;
    }
    
    /**
     * Calculates the barycentric coordinates of a point v in a tetrahedron spanned by the four vertices a, b, c and d.
     */
    template <typename real, typename vec3>
    inline std::vector<real> barycentric_coords(const vec3& p, const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        std::vector<real> coords(4);
        coords[0] = signed_volume<real>(p, b, c, d);
        coords[1] = signed_volume<real>(a, p, c, d);
        coords[2] = signed_volume<real>(a, b, p, d);
        coords[3] = signed_volume<real>(a, b, c, p);
        
        real s = coords[0] + coords[1] + coords[2] + coords[3];
        for (unsigned int i = 0; i < 4; ++i)
        {
            coords[i] /= s;
        }
        return coords;
    }
    
    template <typename vec3>
    inline vec3 normal_direction(const vec3& a, const vec3& b, const vec3& c)
    {
        vec3 ab = b - a;
        vec3 ac = c - a;
        vec3 n = cross(ab, ac);
#ifdef DEBUG
        assert(!MT::is_nan(n[0]) && !MT::is_nan(n[1]) && !MT::is_nan(n[2]));
#endif
        return normalize(n);
    }
    
    template <typename real, typename vec3>
    inline vec3 normal_direction(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        vec3 n = normal_direction<real>(a, b, c);
        vec3 bf = barycenter<real>(a, b, c);
        vec3 bt = barycenter<real>(a, b, c, d);
        vec3 v_out = bf - bt;
        if (dot(v_out, n) > 0)
            return n;
        else
            return -n;
    }
    
    /**
     Returns p projected onto the line spanned by the two points a and b.
     */
    template <typename vec3>
    inline vec3 project(const vec3& p, const vec3& a, const vec3& b)
    {
        vec3 v1 = p - a;
        vec3 v2 = b - a;
        return a + v2 * dot(v1,v2)/dot(v2, v2);
    }
    
    /**
     * Project the point p onto the plane spanned by the three points a, b and c.
     */
    template<typename vec3>
    inline vec3 project(const vec3& p, const vec3& a, const vec3& b, const vec3& c)
    {
        vec3 normal = normal_direction(a, b, c);
        return p - normal * dot(p - a, normal);
    }
    
    template <typename MT>
    inline typename MT::real_type calc_flatness(typename MT::vector3_type const & a,
                                                typename MT::vector3_type const & b,
                                                typename MT::vector3_type const & c,
                                                typename MT::vector3_type const & d)
    {
        typedef typename MT::vector3_type V;
        V normal0 = normal_direction<MT>(d, a, b);
        V normal1 = normal_direction<MT>(c, b, a);
        
        return MT::dot(normal0, normal1);
    }
    
    template<typename real, typename vec3>
    inline real ms_length(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        real result = 0.;
        result += sqr_length(a - b);
        result += sqr_length(a - c);
        result += sqr_length(a - d);
        result += sqr_length(b - c);
        result += sqr_length(b - d);
        result += sqr_length(c - d);
        return result / 6.;
    }
    
    template<typename real, typename vec3>
    inline real rms_length(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
    {
        return sqrt(ms_length<real>(a, b, c, d));
    }
    
    template<typename MT>
    inline typename MT::real_type quality(typename MT::vector3_type const a, typename MT::vector3_type const b, typename MT::vector3_type const c, typename MT::vector3_type const d)
    {
        typedef typename MT::real_type      T;
        
        T v = Util::signed_volume<T>(a, b, c, d);
        T lrms = rms_length<T>(a, b, c, d);
        
        T q = 8.48528 * v / (lrms * lrms * lrms);
#ifdef DEBUG
        assert(!MT::is_nan(q));
#endif
        return q;
    }
    
    
    /**
     * Finds the center of a smallest circle containing the triangle specified by vertices a, b, c.
     * For an acute or right triangle, this is the circumcircle. For an obtuse triangle this is the midpoint of the longest edge.
     */
//    template <typename MT>
//    inline typename MT::vector3_type min_circle_center(typename MT::vector3_type & a,
//                                                       typename MT::vector3_type & b,
//                                                       typename MT::vector3_type & c)
//    {
//        typedef typename MT::real_type    T;
//        typedef typename MT::vector3_type V;
//        
//        V eba = b-a,
//        eca = c-a,
//        ecb = c-b;
//        
//        T c2 = MT::sqr_length(eba),
//        b2 = MT::sqr_length(eca),
//        a2 = MT::sqr_length(ecb);
//        
//        T alpha = a2 * (b2 + c2 - a2);
//        T beta  = b2 * (a2 + c2 - b2);
//        T gamma = c2 * (a2 + b2 - c2);
//        
//        T sum = alpha + beta + gamma;
//        alpha /= sum;	beta /= sum;	gamma /= sum;
//        
//        if (alpha <= 0)
//            return (b+c)/2.0;
//        if (beta <= 0)
//            return (a+c)/2.0;
//        if (gamma <= 0)
//            return (a+b)/2.0;
//        
//        return alpha * a + beta * b + gamma * c;
//    }
    
    /**
     * Computes the determinant of a 4-by-4 matrix specified by four 4D vectors a, b, c, d
     */
    template <typename MT>
    inline typename MT::real_type determinant(typename MT::vector4_type & a,
                                              typename MT::vector4_type & b,
                                              typename MT::vector4_type & c,
                                              typename MT::vector4_type & d)
    {
        typedef typename MT::real_type T;
        
        T a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
        
        a1 = a[0];	b1 = b[0];	c1 = c[0];	d1 = d[0];
        a2 = a[1];	b2 = b[1];	c2 = c[1];	d2 = d[1];
        a3 = a[2];	b3 = b[2];	c3 = c[2];	d3 = d[2];
        a4 = a[3];	b4 = b[3];	c4 = c[3];	d4 = d[3];
        
        return   a1 * (b2*(c3*d4-d3*c4)-c2*(b3*d4-d3*b4)+d2*(b3*c4-c3*b4))
        - b1 * (a2*(c3*d4-d3*c4)-c2*(a3*d4-d3*a4)+d2*(a3*c4-c3*a4))
        + c1 * (a2*(b3*d4-d3*b4)-b2*(a3*d4-d3*a4)+d2*(a3*b4-b3*a4))
        - d1 * (a2*(b3*c4-c3*b4)-b2*(a3*c4-c3*a4)+c2*(a3*b4-b3*a4));
    }
    
    template <typename MT>
    inline std::vector<typename MT::vector3_type> find_basis(std::vector<typename MT::vector3_type> & basis, std::vector<typename MT::vector3_type> & points)
    {
        typedef typename MT::real_type      T;
        typedef typename MT::vector3_type   V;
        V vp = points[0];
        std::vector<V> b,m;
        for (int i = 1; i < points.size(); ++i)
        {
            m.push_back(points[i]);
        }
        
        if (points.size() == 1)
        {
            if (basis.size() == 0)
            {
                return points;
            }
            for (unsigned int i = 0; i < basis.size(); ++i)
            {
                b.push_back(basis[i]);
            }
        }
        else
            b = find_basis<MT>(basis, m);
        
        if (b.size() == 1)
        {
            V vq = b[0];
            if (MT::dot(vq,vp-vq) >= 0)
            {
                return b;
            }
        }
        else if (b.size() == 2)
        {
            V vq = b[0];
            V vr = b[1];
            V vs = vp - vr;
            V vt = vq - vr;
            if (MT::dot(MT::cross(vs,vt),MT::cross(vr,vt)) >= 0)
            {
                return b;
            }
        }
        else if (b.size() == 3)
        {
            V vq = b[0];
            V vr = b[1];
            V vs = b[2];
            if (signed_volume<MT>(vp, vq, vr, vs) * signed_volume<MT>(V(0.0), vq, vr, vs) <= 0)
            {
                return b;
            }
        }
        else
        {
            return b;
        }
        
        basis.push_back(vp);
        if (points.size() == 1 || basis.size() == 3)
        {
            return basis;
        }
        else
        {
            return find_basis<MT>(basis, m);
        }
    }
    
    /**
     * Finds the minimum convex hull point.
     */
    template <typename MT>
    inline typename MT::vector3_type min_convex_hull_point(std::vector<typename MT::vector3_type> & points)
    {
        typedef typename MT::real_type      T;
        typedef typename MT::vector3_type   V;
        
        std::vector<V> basis;
        std::vector<V> b = find_basis<MT>(basis, points);
        if (b.size() == 1)
        {
            return b[0];
        }
        else if (b.size() == 2)
        {
            V vp = b[0];
            V vq = b[1];
            return vq - (vp-vq)*(MT::dot(vq,vp-vq)/MT::sqr_length(vp-vq));
        }
        else if (b.size() == 3)
        {
            V vp = b[0];
            V vq = b[1];
            V vr = b[2];
            V vs = vp-vr;
            V vt = vq-vr;
            return vr - (MT::dot(MT::cross(vs,vt), MT::cross(vr,vt))/MT::dot(MT::cross(vs,vt),MT::cross(vs,vt)))*vs - (MT::dot(MT::cross(vs,vt),MT::cross(vs,vr))/MT::dot(MT::cross(vs,vt),MT::cross(vs,vt)))*vt;
        }
        else
            return V(0.0);
    }
    
    /**
     * Returns the shortest distance from the point p to the plane spanned by the points a, b and c.
     */
    template<typename MT>
    inline typename MT::real_type distance(const typename MT::vector3_type& p, const typename MT::vector3_type& a, const typename MT::vector3_type& b, const typename MT::vector3_type& c)
    {
        typedef typename MT::vector3_type   V;
        
        V v = p - a;
        V n = normal_direction<MT>(a, b, c);
        
        return std::abs(MT::dot(v, n));
    }
    
    /**
     Returns whether you have to turn left when going from a to b to c.
     */
    template<typename MT>
    inline bool is_left_of(const typename MT::vector3_type& a, const typename MT::vector3_type& b, const typename MT::vector3_type& c)
    {
        if(signed_area<MT>(a, b, c) > 0.)
        {
            return true;
        }
        return false;
    }
    
    template<typename MT>
    inline bool is_between(const typename MT::vector3_type& p, const std::vector<typename MT::vector3_type>& verts)
    {
        bool is_l1 = is_left_of<MT>(verts[0], verts[1], p);
        bool is_l2 = is_left_of<MT>(verts[1], verts[2], p);
        bool is_l3 = is_left_of<MT>(verts[2], verts[0], p);
        return (is_l1 && is_l2 && is_l3) | (!is_l1 && !is_l2 && !is_l3);
    }
    
    /**
     * Calculates the intersection between the line segment |p0 p1| and the plane spanned by the vertices v0, v1 and v2. The intersection point is defined by p0 + t*(p1 - p0) and the function returns t. Returns infinity if it does not intersect.
     */
    template<typename MT>
    typename MT::real_type intersection_ray_plane(const typename MT::vector3_type& p0, const typename MT::vector3_type& p1, const typename MT::vector3_type& v0, const typename MT::vector3_type& v1, const typename MT::vector3_type& v2)
    {
        typedef typename MT::real_type      T;
        typedef typename MT::vector3_type   V;
        
        V normal = normal_direction(v0, v1, v2);
        T n = MT::dot(normal, v0 - p0);
        T d = MT::dot(normal, p1 - p0);
        
        if (std::abs(d) < EPSILON) // Plane and line are parallel if true.
        {
            if (std::abs(n) < EPSILON)
            {
                return 0.; // Line intersection
            }
            return INFINITY; // No intersection.
        }
        
        // Compute the t value for the directed line ray intersecting the plane.
        return n / d;
    }
    
    /**
     * Calculates the intersection between the line segment |p0 p1| and the triangle |v0 v1 v2|. The intersection point is defined by p0 + t*(p1 - p0) and the function returns t. Returns infinity if it does not intersect.
     */
    template<typename MT>
    typename MT::real_type intersection_ray_triangle(const typename MT::vector3_type& p0, const typename MT::vector3_type& p1, const typename MT::vector3_type& v0, const typename MT::vector3_type& v1, const typename MT::vector3_type& v2)
    {
        typedef typename MT::real_type      T;
        typedef typename MT::vector3_type   V;
        
        T t = intersection_ray_plane<MT>(p0, p1, v0, v1, v2);
        if(t < 0.) // The ray goes away from the triangle
        {
            return t;
        }
        V p = p0 + t*(p1 - p0);
        
        std::vector<T> coords = barycentric_coords<real>(p, v0, v1, v2);
        if(coords[0] > EPSILON && coords[1] > EPSILON && coords[2] > EPSILON) // The intersection happens inside the triangle.
        {
            return t;
        }
        return INFINITY; // The intersection happens outside the triangle.
    }
    
    /**
     * Implies ordering in the space of binary vectors of given size.
     *
     * @param n     Size of binary vectors.
     * @param v1    First vector.
     * @param v2    Second vector.
     *
     * @return      True if the first vector is smaller than the second, false otherwise.
     */
    inline bool compare(int n, const std::vector<bool> & v1, const std::vector<bool> & v2)
    {
        for (int i = 0; i < n; ++i)
        {
            if (v1[i] && !v2[i]) return false;
            if (!v1[i] && v2[i]) return true;
        }
        return false;
    }
    
    
    
    /**
     Concatonates the integer number to the string name.
     */
    inline std::string concat4digits(std::string name, int number)
    {
        std::ostringstream s;
        if (number < 10)
            s << name << "000" << number;
        else if (number < 100)
            s << name << "00" << number;
        else if (number < 1000)
            s << name << "0" << number;
        else
            s << name << number;
        return s.str();
    }
    
}
