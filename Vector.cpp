#include "Vector.hpp"

Vector::Vector() : value(0.0, 0.0, 0.0) {}

Vector::Vector(float x, float y, float z) : value(x, y, z) {}

Vector::Vector(const btVector3& other) : value(other.x(), other.y(), other.z()) {}

Vector::Vector(const btVector3&& other) : value(other.x(), other.y(), other.z()) {}

Vector::Vector(const Ogre::Vector3& other) : value(other.x, other.y, other.z) {}

Vector::Vector(const Ogre::Vector3&& other) : value(other.x, other.y, other.z) {}

Vector::Vector(const Vector& other) : value(other.x(), other.y(), other.z()) {}

Vector::Vector(const Vector&& other) : value(other.x(), other.y(), other.z()) {}

float Vector::x() const {
    return this->value.x();
}

float Vector::y() const {
    return this->value.y();
}

float Vector::z() const {
    return this->value.z();
}

void Vector::setX(float x) const {
    return this->value.setX(x);
}

void Vector::setY(float y) const {
    return this->value.setY(y);
}

void Vector::setZ(float z) const {
    return this->value.setZ(z);
}

void setValue(const btVector3& other) {
    this->value.setValue(other.x(), other.y(), other.z());
}

void setValue(const btVector3&& other) {
    this->value.setValue(other.x(), other.y(), other.z());
}

void setValue(const Ogre::Vector3& other) {
    this->value.setValue(other.x, other.y, other.z);
}

void setValue(const Ogre::Vector3&& other) {
    this->value.setValue(other.x, other.y, other.z);
}

Ogre::Vector3 Vector::toOgre() const {
    return Ogre::Vector3(value.x(), value.y(), value.z());
}

btVector3 Vector::toBullet() const {
    return value;
}

Vector3 Vector::operator + (const Vector3& other) const {
    return Vector3(value.x() + other.x(), value.y() + other.y(), value.z() + other.z());
}

Vector3 Vector::operator - (const Vector3& other) const {
    return Vector3(value.x() - other.x(), value.y() - other.y(), value.z() - other.z());
}

Vector3 Vector::operator * (const Vector3& other) const {
    return Vector3(value.x() * other.x(), value.y() * other.y(), value.z() * other.z());
}

Vector3 Vector::operator / (const Vector3& other) const {
    return Vector3(value.x() / other.x(), value.y() / other.y(), value.z() / other.z());
}


Vector3& Vector::operator += (const Vector3& other) const {
    value.setX(value.x() + other.x());
    value.setY(value.y() + other.y());
    value.setZ(value.z() + other.z());
    return value;
}

Vector3& Vector::operator -= (const Vector3& other) const {
    value.setX(value.x() - other.x());
    value.setY(value.y() - other.y());
    value.setZ(value.z() - other.z());
    return value;
}

Vector3& Vector::operator *= (const Vector3& other) const {
    value.setX(value.x() * other.x());
    value.setY(value.y() * other.y());
    value.setZ(value.z() * other.z());
    return value;
}

Vector3& Vector::operator /= (const Vector3& other) const {
    value.setX(value.x() / other.x());
    value.setY(value.y() / other.y());
    value.setZ(value.z() / other.z());
    return value;
}

Vector3& Vector::dot (const Vector3& other) {
    this->value = this->value.dot(other.value);
    return this;
}

Vector3 Vector::dotted (const Vector3& other) {
    return this->value.dot(other.value);
}
