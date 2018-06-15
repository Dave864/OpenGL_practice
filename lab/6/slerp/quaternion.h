#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>

class quaternion;
float dot(const quaternion& p, const quaternion& q);

class quaternion
{
public:
    friend float dot(const quaternion&, const quaternion&);

    quaternion();
    quaternion(float angle, float x, float y, float z);
    ~quaternion();
    
    // interpolate from p to q (u is in the range 0 to 1)
    void slerp(
        float u, const quaternion& p, const quaternion& q
        );

    // angle is in degrees
    void from_angle_axis(
        float angle, float x, float y, float z
        );
    void to_angle_axis(
        float &angle, float &x, float &y, float &z
        ) const;

    void scale(float a);
    void add(const quaternion& q);
    void add(const quaternion& p, const quaternion& q);
    void sub(const quaternion& q);
    void sub(const quaternion& p, const quaternion& q);
    void mult(const quaternion& p, const quaternion& q);
    void mult(const quaternion& q);
    float dot(const quaternion& q) const;
    void inv();
    void pow(float r);
    void normalize();



    void set(const quaternion& p);
    void set(float s, float x, float y, float z);
    void get(float &s, float &x, float &y, float &z) const;

private:
    float s, x, y, z;
};

std::ostream& operator<<(std::ostream& ostr, const quaternion& q);


#endif
