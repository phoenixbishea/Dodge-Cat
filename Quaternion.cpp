#include "Quaternion.hpp"

Quaternion::Quaternion() : value(0.0, 0.0, 0.0, 0.0) {}

Quaternion::Quaternion(float x, float y, float z, float w) : value(x, y, z, w) {}

Quaternion::Quaternion(Vector axis, float w) : value(axis.toBullet(), w) {}

Quaternion::Quaternion(const btQuaternion& other) : value(other) {}
Quaternion::Quaternion(const btQuaternion&& other) : value(other) {}

Quaternion::Quaternion(const Ogre::Quaternion& other) : value(other.x, other.y, other.z, other.w) {}
Quaternion::Quaternion(const Ogre::Quaternion&& other) : value(other.x, other.y, other.z, other.w) {}

Ogre::Quaternion Quaternion::toOgre() const {
    return Ogre::Quaternion(value.w(), value.x(), value.y(), value.z());
}

btQuaternion Quaternion::toBullet() const {
    return this->value;
}

bool Quaternion::operator == (const Quaternion& other) const {
    return this->value == other.value;
}

bool Quaternion::operator == (const Quaternion&& other) const {
    return this->value == other.value;
}

bool Quaternion::operator != (const Quaternion& other) const {
    return this->value != other.value;
}

bool Quaternion::operator != (const Quaternion&& other) const {
    return this->value != other.value;
}

bool Quaternion::operator == (const btQuaternion& other) const {
    return this->value == other;
}

bool Quaternion::operator == (const btQuaternion&& other) const {
    return this->value == other;
}

bool Quaternion::operator != (const btQuaternion& other) const {
    return this->value != other;
}

bool Quaternion::operator != (const btQuaternion&& other) const {
    return this->value != other;
}

bool Quaternion::operator == (const Ogre::Quaternion& other) const {
    return this->toOgre() == other;
}

bool Quaternion::operator == (const Ogre::Quaternion&& other) const {
    return this->toOgre() == other;
}

bool Quaternion::operator != (const Ogre::Quaternion& other) const {
    return this->toOgre() != other;
}

bool Quaternion::operator != (const Ogre::Quaternion&& other) const {
    return this->toOgre() != other;
}

Vector Quaternion::operator * (const Vector& other) const {
    Ogre::Quaternion temp = this->toOgre();
    return Vector(temp * other.toOgre());
}

Vector Quaternion::operator * (const Vector&& other) const {
    Ogre::Quaternion temp = this->toOgre();
    return Vector(temp * other.toOgre());
}