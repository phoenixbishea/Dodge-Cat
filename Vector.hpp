#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <btBulletDynamicsCommon.h>
#include <OgreVector3.h>

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
    bool operator == (const Vector&) const;
    bool operator == (const Vector&&) const;
    bool operator != (const Vector&) const;
    bool operator != (const Vector&&) const;

    bool operator == (const btVector3&) const;
    bool operator == (const btVector3&&) const;
    bool operator != (const btVector3&) const;
    bool operator != (const btVector3&&) const;

    bool operator == (const Ogre::Vector3&) const;
    bool operator == (const Ogre::Vector3&&) const;
    bool operator != (const Ogre::Vector3&) const;
    bool operator != (const Ogre::Vector3&&) const;

    Vector operator + (const Vector&) const;
    Vector operator - (const Vector&) const;
    Vector operator * (float) const;
    Vector operator / (float) const;

    Vector operator + (const btVector3&) const;
    Vector operator - (const btVector3&) const;

    Vector operator + (const Ogre::Vector3&) const;
    Vector operator - (const Ogre::Vector3&) const;

    Vector& operator += (const Vector&);
    Vector& operator -= (const Vector&);
    Vector& operator *= (float);
    Vector& operator /= (float);

    Vector& operator += (const btVector3&);
    Vector& operator -= (const btVector3&);

    Vector& operator += (const Ogre::Vector3&);
    Vector& operator -= (const Ogre::Vector3&);

    Vector& operator=(const Vector&);

    float dot(const Vector&) const;

    Vector& normalize();
    Vector normalized();
};



#endif