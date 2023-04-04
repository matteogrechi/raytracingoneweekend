#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)
            const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 A_C = r.origin() - center;

    double a = r.direction().length_squared();
    double half_b = dot(r.direction(), A_C);
    double c = A_C.length_squared() - radius * radius;

    double quarter_discriminant = half_b*half_b - a*c;
    if (quarter_discriminant < 0)
        return false;
    double sqrt_quarter_discriminant = sqrt(quarter_discriminant);

    // Find the nearest root that lies in the acceptable range
    double root = (-half_b - sqrt_quarter_discriminant) / a;
    if (root < t_min || root > t_max) {
        root = (-half_b + sqrt_quarter_discriminant) / a;
        if (root < t_min || root > t_max)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);

    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    rec.mat_ptr = mat_ptr;
    
    return true;
}

#endif