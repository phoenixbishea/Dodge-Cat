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

float Quaternion::x() const {
    return this->value.x();
}

float Quaternion::y() const {
    return this->value.y();
}

float Quaternion::z() const {
    return this->value.z();
}

float Quaternion::w() const {
    return this->value.w();
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

Quaternion& Quaternion::operator *= (const Quaternion& other)
{
    this->value *= other.value;
    return *this;
}

Quaternion& Quaternion::operator = (const Ogre::Quaternion& other)
{
    btQuaternion rhs(other.x, other.y, other.z, other.w);
    this->value.setValue(rhs.x(), rhs.y(), rhs.z(), rhs.w());
    return *this;
}
