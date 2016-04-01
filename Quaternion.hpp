#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include <btBulletDynamicsCommon.h>
#include <OgreQuaternion.h>

#include <iostream>

#include "Vector.hpp"

class Quaternion {
private:
	btQuaternion value;
public:
    friend std::ostream& operator << (std::ostream&, const Quaternion&);

	Quaternion();
	Quaternion(float, float, float, float);
	Quaternion(Vector, float);
	Quaternion(const btQuaternion&);
	Quaternion(const btQuaternion&&);
	Quaternion(const Ogre::Quaternion&);
	Quaternion(const Ogre::Quaternion&&);

	Ogre::Quaternion toOgre() const;
    btQuaternion toBullet() const;
 
    float x() const;
    float y() const;
    float z() const;
    float w() const;

    void setX(float);
    void setY(float);
    void setZ(float);
    void setW(float);

	bool operator == (const Quaternion&) const;
	bool operator == (const Quaternion&&) const;
	bool operator != (const Quaternion&) const;
	bool operator != (const Quaternion&&) const;

	bool operator == (const btQuaternion&) const;
	bool operator == (const btQuaternion&&) const;
	bool operator != (const btQuaternion&) const;
	bool operator != (const btQuaternion&&) const;

	bool operator == (const Ogre::Quaternion&) const;
	bool operator == (const Ogre::Quaternion&&) const;
	bool operator != (const Ogre::Quaternion&) const;
	bool operator != (const Ogre::Quaternion&&) const;

	Vector operator * (const Vector&) const;
	Vector operator * (const Vector&&) const;

	Quaternion& operator *= (const Quaternion&);
    Quaternion& operator = (const Ogre::Quaternion&);
};

#endif
