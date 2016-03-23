#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <btBulletDynamicsCommon.h>
#include <OgreVector3.h>

#include "Quaternion.hpp"

class Vector {
private:
    btVector3 value;
public:
    Vector();
    Vector(float, float, float);
    Vector(const btVector3&);
    Vector(const btVector3&&);
    Vector(const Ogre::Vector3&);
    Vector(const Ogre::Vector3&&);
    Vector(const Vector&);
    Vector(const Vector&&);

    float x() const;
    float y() const;
    float z() const;

    void setX(float);
    void setY(float);
    void setZ(float);

    void setValue(const btVector3&);
    void setValue(const btVector3&&);
    void setValue(const Ogre::Vector3&);
    void setValue(const Ogre::Vector3&&);

    Ogre::Vector3 toOgre() const;
    btVector3 toBullet() const;

    /*
      Vector math operations
    */
    Vector3 operator + (const Vector3& other) const;
    Vector3 operator - (const Vector3& other) const;
    Vector3 operator * (const Vector3& other) const;
    Vector3 operator / (const Vector3& other) const;

    Vector3& operator += (const Vector3& other);
    Vector3& operator -= (const Vector3& other);
    Vector3& operator *= (const Vector3& other);
    Vector3& operator /= (const Vector3& other);

    Vector3& dot(const Vector3&);
    Vector3 dotted(const Vector3&);

    Vector3
};



#endif
