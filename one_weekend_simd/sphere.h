#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere() {}
        sphere(point3 cen, float r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)
            const override;

    public:
        point3 center;
        float radius;
        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 A_C = r.origin() - center;

    float a = r.direction().length_squared();
    float half_b = dot(r.direction(), A_C);
    float c = A_C.length_squared() - radius * radius;

    float quarter_discriminant = half_b*half_b - a*c;
    if (quarter_discriminant < 0)
        return false;
    float sqrt_quarter_discriminant = sqrt(quarter_discriminant);

    // Find the nearest root that lies in the acceptable range
    float root = (-half_b - sqrt_quarter_discriminant) / a;
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