/**
 * ray_tracer.cpp
 * CS230
 * -------------------------------
 * Implement ray tracer here.
 */

#define SET_RED(P, C)   (P = (((P) & 0x00ffffff) | ((C) << 24)))
#define SET_GREEN(P, C)  (P = (((P) & 0xff00ffff) | ((C) << 16)))
#define SET_BLUE(P, C) (P = (((P) & 0xffff00ff) | ((C) << 8)))

#include "ray_tracer.h"
#include <algorithm>

using namespace std;

const double Object::small_t=1e-6;
//--------------------------------------------------------------------------------
// utility functions
//--------------------------------------------------------------------------------
double sqr(const double x)
{
    return x*x;
}

Pixel Pixel_Color(const Vector_3D<double>& color)
{
    Pixel pixel=0;
    SET_RED(pixel,(unsigned char)(min(color.x,1.0)*255));
    SET_GREEN(pixel,(unsigned char)(min(color.y,1.0)*255));
    SET_BLUE(pixel,(unsigned char)(min(color.z,1.0)*255));
    return pixel;
}
//--------------------------------------------------------------------------------
// Shader
//--------------------------------------------------------------------------------
Vector_3D<double> Phong_Shader::
Shade_Surface(const Ray& ray,const Object& intersection_object,const Vector_3D<double>& intersection_point,const Vector_3D<double>& same_side_normal) const
{
    Vector_3D<double> V3D;
    Vector_3D<double> color;

    bool in_shadow;
    double ambient, diffuse, specular;
    Vector_3D<double> new_intersection = intersection_point + (same_side_normal * intersection_object.small_t);
    Vector_3D<double> view = (Vector_3D<double>(0,0,0) - ray.direction).Normalized();
    Vector_3D<double> incidence;
    Vector_3D<double> reflection;
    for(unsigned int i = 0; i < world.lights.size(); i++)
    {
	incidence = (world.lights[i]->position - new_intersection).Normalized();
	reflection = (same_side_normal * (2 * V3D.Dot_Product(incidence, same_side_normal)) - incidence).Normalized();
	//calculate red component
	in_shadow = false;
	ambient = color_ambient.x * world.lights[i]->Emitted_Light(ray).x;
	//check if point is in shadow
	if(world.enable_shadows)
	{
	    Ray shadow(new_intersection, incidence);
   	    bool intersections = false;
    	    for(unsigned int j = 0; j < world.objects.size(); j++)
     	    {
		if(world.objects[j]->Intersection(shadow)) intersections = true;
    	    }
	    if(intersections)
	    {
		in_shadow = true;
		diffuse = 0.0;
		specular = 0.0;
	    }
	}
	if(!in_shadow)
	{
	    diffuse = color_diffuse.x * world.lights[i]->Emitted_Light(ray).x * max(0.0, V3D.Dot_Product(incidence, same_side_normal));
	    specular = color_specular.x * world.lights[i]->Emitted_Light(ray).x * pow(max(0.0, V3D.Dot_Product(view, reflection)), specular_power);
	}
	color.x += ambient + diffuse + specular;
	//calculate green component
	in_shadow = false;
	ambient = color_ambient.y * world.lights[i]->Emitted_Light(ray).y;
	//check if point is in shadow
	if(world.enable_shadows)
	{
	    Ray shadow(new_intersection, incidence);
   	    bool intersections = false;
    	    for(unsigned int j = 0; j < world.objects.size(); j++)
     	    {
		if(world.objects[j]->Intersection(shadow)) intersections = true;
    	    }
	    if(intersections)
	    {
		in_shadow = true;
		diffuse = 0.0;
		specular = 0.0;
	    }
	}
	if(!in_shadow)
	{
	    diffuse = color_diffuse.y * world.lights[i]->Emitted_Light(ray).y * max(0.0, V3D.Dot_Product(incidence, same_side_normal));
	    specular = color_specular.y * world.lights[i]->Emitted_Light(ray).y * pow(max(0.0, V3D.Dot_Product(view, reflection)), specular_power);
	}
	color.y += ambient + diffuse + specular;
	//calculate blue component
	in_shadow = false;
	ambient = color_ambient.z * world.lights[i]->Emitted_Light(ray).z;
	//check if point is in shadow
	if(world.enable_shadows)
	{
	    Ray shadow(new_intersection, incidence);
   	    bool intersections = false;
    	    for(unsigned int j = 0; j < world.objects.size(); j++)
     	    {
		if(world.objects[j]->Intersection(shadow)) intersections = true;
    	    }
	    if(intersections)
	    {
		in_shadow = true;
		diffuse = 0.0;
		specular = 0.0;
	    }
	}
	if(!in_shadow)
	{
	    diffuse = color_diffuse.z * world.lights[i]->Emitted_Light(ray).z * max(0.0, V3D.Dot_Product(incidence, same_side_normal));
	    specular = color_specular.z * world.lights[i]->Emitted_Light(ray).z * pow(max(0.0, V3D.Dot_Product(view, reflection)), specular_power);
	}
	color.z += ambient + diffuse + specular;
    }

    return color;
}

Vector_3D<double> Reflective_Shader::
Shade_Surface(const Ray& ray,const Object& intersection_object,const Vector_3D<double>& intersection_point,const Vector_3D<double>& same_side_normal) const
{
    Vector_3D<double> V3D;
    Vector_3D<double> color = Phong_Shader::Shade_Surface(ray, intersection_object, intersection_point, same_side_normal);

    Vector_3D<double> new_intersection = intersection_point + (same_side_normal * intersection_object.small_t);
    Vector_3D<double> view = (Vector_3D<double>(0,0,0) - ray.direction).Normalized();
    Vector_3D<double> reflection = same_side_normal * 2 * V3D.Dot_Product(view, same_side_normal) - view; 
    if(ray.recursion_depth < world.recursion_depth_limit)
    {
	Ray reflect_ray(new_intersection, reflection);
	reflect_ray.recursion_depth = ray.recursion_depth+1;
	color = color * (1 - reflectivity) + world.Cast_Ray(reflect_ray, ray) * reflectivity;
    }

    return color;
}

Vector_3D<double> Flat_Shader::
Shade_Surface(const Ray& ray,const Object& intersection_object,const Vector_3D<double>& intersection_point,const Vector_3D<double>& same_side_normal) const
{
    return color;
}

//--------------------------------------------------------------------------------
// Objects
//--------------------------------------------------------------------------------
// determine if the ray intersects with the sphere
// if there is an intersection, set t_max, current_object, and semi_infinite as appropriate and return true
bool Sphere::
Intersection(Ray& ray) const
{
    Vector_3D<double> V3D;
    Vector_3D<double> rayLen = ray.endpoint - center;
    //Calculate discriminant
    double discriminant = sqr(V3D.Dot_Product(ray.direction, rayLen)) - rayLen.Length_Squared() + sqr(radius);
    //There is at least 1 intersection
    if(discriminant >= 0)
    {
	double cur_t_max = -(V3D.Dot_Product(ray.direction, rayLen)) - sqrt(discriminant);
	if(cur_t_max <= small_t)
	{
	    return false;
	}
	if(ray.semi_infinite)
	{
	    ray.current_object = this;
	    ray.t_max = cur_t_max;
	    ray.semi_infinite = false;
	}
	else
	{
	    if(ray.t_max > cur_t_max)
	    {
	    	ray.current_object = this;
		ray.t_max = cur_t_max;
	    }
	}
	return true;
    }
    //There are no intersections
    else
    {
	return false;
    }
}

Vector_3D<double> Sphere::
Normal(const Vector_3D<double>& location) const
{
    Vector_3D<double> normal;
    normal = (location - center).Normalized();
    return normal;
}

// determine if the ray intersects with the plane
// if there is an intersection, set t_max, current_object, and semi_infinite as appropriate and return true
bool Plane::
Intersection(Ray& ray) const
{
    Vector_3D<double> V3D;
    double denominator = V3D.Dot_Product(ray.direction, normal);
    //The ray and plane intersect
    if(denominator != 0)
    {
	double cur_t_max = V3D.Dot_Product((x1 - ray.endpoint), normal) / denominator;
	if(cur_t_max <= small_t)
	{
	    return false;
	}
	if(ray.semi_infinite)
	{
	    ray.current_object = this;
	    ray.t_max = cur_t_max;
	    ray.semi_infinite = false;
	}
	else
	{
	    if(ray.t_max > cur_t_max)
	    {
	    	ray.current_object = this;
		ray.t_max = cur_t_max;
	    }
	}
	return true;
    }
    //The ray and plane do not intersect
    else
    {
	return false;
    }
}

Vector_3D<double> Plane::
Normal(const Vector_3D<double>& location) const
{
    return normal;
}
//--------------------------------------------------------------------------------
// Camera
//--------------------------------------------------------------------------------
// Find the world position of the input pixel
Vector_3D<double> Camera::
World_Position(const Vector_2D<int>& pixel_index)
{
    Vector_3D<double> result;
    Vector_2D<double> pixel_pos = film.pixel_grid.X(pixel_index);
    result = focal_point + (vertical_vector * pixel_pos.y) + (horizontal_vector * pixel_pos.x);
    return result;
}
//--------------------------------------------------------------------------------
// Render_World
//--------------------------------------------------------------------------------
// Find the closest object of intersection and return a pointer to it
//   if the ray intersects with an object, then ray.t_max, ray.current_object, and ray.semi_infinite will be set appropriately
//   if there is no intersection do not modify the ray and return 0
const Object* Render_World::
Closest_Intersection(Ray& ray)
{
    bool intersections = false;
    for(unsigned int i = 0; i < objects.size(); i++)
    {
	if(objects[i]->Intersection(ray)) intersections = true;
    }
    if(intersections)
    {
	return ray.current_object;
    }
    else
    {
	return 0;
    }
}

// set up the initial view ray and call 
void Render_World::
Render_Pixel(const Vector_2D<int>& pixel_index)
{
    Vector_3D<double> dir = camera.World_Position(pixel_index) - camera.position;
    Ray ray(camera.position, dir);

    Ray dummy_root;
    Vector_3D<double> color=Cast_Ray(ray,dummy_root);
    camera.film.Set_Pixel(pixel_index,Pixel_Color(color));
}

// cast ray and return the color of the closest intersected surface point, 
// or the background color if there is no object intersection
Vector_3D<double> Render_World::
Cast_Ray(Ray& ray,const Ray& parent_ray)
{
    Vector_3D<double> color;

    // determine the color here
    const Object * closest = Closest_Intersection(ray);
    if(!ray.semi_infinite)
    {
	Vector_3D<double> intersection = ray.endpoint + (ray.direction * ray.t_max);
	Vector_3D<double> normal = closest->Normal(intersection);
	color = closest->material_shader->Shade_Surface(ray, *closest, intersection, normal);
    }

    return color;
}
