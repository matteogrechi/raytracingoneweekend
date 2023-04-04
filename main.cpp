#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "camera.h"
#include "sphere.h"
#include "mesh.h"

#include <iostream>

color ray_color(const ray& r, const hittable& world){
    hit_record rec;
    if (world.hit(r,0,infinity,rec)) {
        return 0.5 * (rec.normal + color(1,1,1));
    }
    
    // Sky color
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0,1.0,1.0) + t*color(0.5,0.7,1.0);
}

/*
color ray_color(const ray& r){
    // Sphere color
    // sphere_radius = 0.5
    const color sphere_color = color(1,0,0);
    double discriminant = hit_sphere(point3(0,0,-1), 0.5, r);
    double sphere_alpha = 0;
    if (discriminant > 0)
        sphere_alpha = discriminant;
    
    // Sky color
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5*(unit_direction.y() + 1.0);
    color sky_color = (1.0-t)*color(1.0,1.0,1.0) + t*color(0.5,0.7,1.0);

    // Total color
    return sphere_alpha * sphere_color + (1-sphere_alpha) * sky_color;
}
*/

int main() {
    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(-3,0,-1), 2));
    world.add(make_shared<mesh>(point3(0.25,0,-1),point3(0.125,0.5,-1.25),point3(0,0,-2)));
    world.add(make_shared<sphere>(point3(0.5,0,-1), 0.4));
    world.add(make_shared<sphere>(point3(0,-100.5,-1),100));
    
    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;

    // Camera
    camera cam;

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int s=0; s<samples_per_pixel; ++s){
                double u = (i + random_double()) / (image_width-1);
                double v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u,v);
                pixel_color += ray_color(r,world);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
