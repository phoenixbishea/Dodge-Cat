#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include <btBulletDynamicsCommon.h>
#include <OgreQuaternion.h>

#include "Vector.hpp"

class Quaternion {
private:
	btQuaternion value;
public:
	Quaternion();
	Quaternion(float, float, float, float);
	Quaternion(Vector, float);
	Quaternion(const btQuaternion&);
	Quaternion(const btQuaternion&&);
	Quaternion(const Ogre::Quaternion&);
	Quaternion(const Ogre::Quaternion&&);

	Ogre::Quaternion toOgre() const;
	btQuaternion toBullet() const;

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