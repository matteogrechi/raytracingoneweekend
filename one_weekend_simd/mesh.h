#ifndef MESH_H
#define MESH_H

#include "hittable.h"
#include "vec3.h"

class mesh : public hittable {
    public:
        mesh() {}
        mesh(point3 point_A, point3 point_B, point3  point_C, shared_ptr<material> m) : A(point_A), B(point_B), C(point_C), mat_ptr(m) {};

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)
            const override;

    public:
        point3 A;
        point3 B;
        point3 C;
        shared_ptr<material> mat_ptr;
};

bool mesh::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 B_A = B - A;
    vec3 C_A = C - A;
    vec3 _r = - r.direction();
    
    vec3 O_A = r.origin() - A;

    // The matrix A is [B_A, C_A, _r]
    // The vector b is O_A

    // Cramer's rule for matrix inversion
    // D = det(A)
    // [B_A, C_A, _r, B_A, C_A]
    float D  = det(B_A,C_A,_r);
    /*        B_A.x() * C_A.y() *  _r.z()
              + C_A.x() *  _r.y() * B_A.z()
              +  _r.x() * B_A.y() * C_A.z()
              - B_A.z() * C_A.y() *  _r.x()
              - C_A.z() *  _r.y() * B_A.x()
              -  _r.z() * B_A.y() * C_A.x();*/

    // If ill-conditioned no solution
    if (D <= 1e-3 && D >= -1e-3)
        return false;

    // Dx
    // [O_A, C_A, _r, O_A, C_A]
    float D1 = det(O_A,C_A,_r);
    /*O_A.x() * C_A.y() *  _r.z()
              + C_A.x() *  _r.y() * O_A.z()
              +  _r.x() * O_A.y() * C_A.z()
              - O_A.z() * C_A.y() *  _r.x()
              - C_A.z() *  _r.y() * O_A.x()
              -  _r.z() * O_A.y() * C_A.x();*/

    // Dy
    // [B_A, O_A, _r, B_A, O_A]
    float D2 = det(B_A,O_A,_r);
    /*B_A.x() * O_A.y() *  _r.z()
              + O_A.x() *  _r.y() * B_A.z()
              +  _r.x() * B_A.y() * O_A.z()
              - B_A.z() * O_A.y() *  _r.x()
              - O_A.z() *  _r.y() * B_A.x()
              -  _r.z() * B_A.y() * O_A.x();*/

    // Dz
    // [B_A, C_A, O_A, B_A, C_A]
    float D3 = det(B_A,C_A,O_A);
    /*B_A.x() * C_A.y() * O_A.z()
              + C_A.x() * O_A.y() * B_A.z()
              + O_A.x() * B_A.y() * C_A.z()
              - B_A.z() * C_A.y() * O_A.x()
              - C_A.z() * O_A.y() * B_A.x()
              - O_A.z() * B_A.y() * C_A.x();*/

    // Compute l1, l2 and t
    float l1 = D1 / D;
    float l2 = D2 / D;
    float  t = D3 / D;

    if (l1 >= 0 && l2 >= 0 && l1+l2 <= 1 && t >= t_min && t <= t_max) {
        rec.t = t;
        rec.p = r.origin() + t * r.direction();

        vec3 outward_normal = cross(B_A,C_A).qnormalize();
        rec.set_face_normal(r, outward_normal);

        rec.mat_ptr = mat_ptr;
        
        return true;
    }

    return false;
}

#endif