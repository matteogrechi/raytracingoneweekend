#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
    public:
        camera(
            point3 lookfrom,
            point3 lookat,
            vec3 vup,
            float vfov, 
            float aspect_ratio,
            float aperture,
            float focal_length
        ) {
            float theta = degrees_to_radians(vfov);
            float h = tan(0.5 * theta);
            float viewport_height = 2.0 * h;
            float viewport_width = aspect_ratio * viewport_height;

            w = (lookfrom - lookat).qnormalize();
            u = cross(vup, w).qnormalize();
            v = cross(w,u).qnormalize();

            origin = lookfrom;
            horizontal = focal_length * viewport_width * u;
            vertical = focal_length * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - w * focal_length;

            lens_radius = aperture / 2;
        }

        ray get_ray(float s, float t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(
                origin + offset, 
                lower_left_corner + s*horizontal + t*vertical - origin - offset
            );
        }
    
    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        float lens_radius;
};

#endif