#include "Vector.hpp"

std::ostream& operator << (std::ostream& out, const Vector& val)
{
    out << "(" << val.x() << ", " << val.y() << ", " << val.z() << ")";
    return out;
}

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

void Vector::setX(float x) {
    return this->value.setX(x);
}

void Vector::setY(float y) {
    return this->value.setY(y);
}

void Vector::setZ(float z) {
    return this->value.setZ(z);
}

void Vector::setValue(const btVector3& other) {
    this->value.setValue(other.x(), other.y(), other.z());
}

void Vector::setValue(const btVector3&& other) {
    this->value.setValue(other.x(), other.y(), other.z());
}

void Vector::setValue(const Ogre::Vector3& other) {
    this->value.setValue(other.x, other.y, other.z);
}

void Vector::setValue(const Ogre::Vector3&&

              other) {
    this->value.setValue(other.x, other.y, other.z);
}

Ogre::Vector3 Vector::toOgre() const {
    return Ogre::Vector3(value.x(), value.y(), value.z());
}

btVector3 Vector::toBullet() const {
    return value;
}

/* Vector operations */
bool Vector::operator == (const Vector& other) const {
    return this->toOgre() == other.toOgre();
}

bool Vector::operator == (const Vector&& other) const {
    return this->toOgre() == other.toOgre();
}

bool Vector::operator != (const Vector& other) const {
    return this->toOgre() != other.toOgre();
}

bool Vector::operator != (const Vector&& other) const {
    return this->toOgre() != other.toOgre();
}

bool Vector::operator == (const btVector3& other) const {
    return this->value.x() == other.x() &&
        this->value.y() == other.y() &&
        this->value.z() == other.z();
}

bool Vector::operator == (const btVector3&& other) const {
    return this->value.x() == other.x() &&
        this->value.y() == other.y() &&
        this->value.z() == other.z();
}

bool Vector::operator != (const btVector3& other) const {
    return this->value.x() != other.x() ||
        this->value.y() != other.y() ||
        this->value.z() != other.z();
}

bool Vector::operator != (const btVector3&& other) const {
    return this->value.x() != other.x() ||
        this->value.y() != other.y() ||
        this->value.z() != other.z();
}

bool Vector::operator == (const Ogre::Vector3& other) const {
    return this->toOgre() == other;
}

bool Vector::operator == (const Ogre::Vector3&& other) const {
    return this->toOgre() == other;
}

bool Vector::operator != (const Ogre::Vector3& other) const {
    return this->toOgre() != other;
}

bool Vector::operator != (const Ogre::Vector3&& other) const {
    return this->toOgre() != other;
}

Vector Vector::operator + (const Vector& other) const {
    return Vector(value.x() + other.x(), value.y() + other.y(), value.z() + other.z());
}

Vector Vector::operator - (const Vector& other) const {
    return Vector(value.x() - other.x(), value.y() - other.y(), value.z() - other.z());
}

Vector Vector::operator * (float scale) const {
    return Vector(value * scale);
}

Vector Vector::operator / (float scale) const {
    return Vector(value / scale);
}

/* btVector3 operations */
Vector Vector::operator + (const btVector3& other) const {
    return Vector(value.x() + other.x(), value.y() + other.y(), value.z() + other.z());
}

Vector Vector::operator - (const btVector3& other) const {
    return Vector(value.x() - other.x(), value.y() - other.y(), value.z() - other.z());
}

/* Ogre::Vector3 operations */
Vector Vector::operator + (const Ogre::Vector3& other) const {
    return Vector(value.x() + other.x, value.y() + other.y, value.z() + other.z);
}

Vector Vector::operator - (const Ogre::Vector3& other) const {
    return Vector(value.x() - other.x, value.y() - other.y, value.z() - other.z);
}



Vector& Vector::operator += (const Vector& other) {
    this->value += other.value;
    return *this;
}

Vector& Vector::operator -= (const Vector& other) {
    this->value -= other.value;
    return *this;
}

Vector& Vector::operator *= (float scale) {
    this->value *= scale;
    return *this;
}

Vector& Vector::operator /= (float scale) {
    this->value /= scale;
    return *this;
}


/* btVector3 operations */
Vector& Vector::operator += (const btVector3& other) {
    this->value += other;
    return *this;
}

Vector& Vector::operator -= (const btVector3& other) {
    this->value -= other;
    return *this;
}

/* Ogre::Vector3 operations */
Vector& Vector::operator += (const Ogre::Vector3& other) {
    this->value += btVector3(other.x, other.y, other.z);
    return *this;
}

Vector& Vector::operator -= (const Ogre::Vector3& other) {
    this->value -= btVector3(other.x, other.y, other.z);
    return *this;
}

Vector& Vector::operator=(const Vector& other)
{
    this->value.setValue(other.value.x(), other.value.y(), other.value.z());
    return *this;
}

float Vector::dot (const Vector& other) const {
    return this->value.dot(other.value);
}

Vector& Vector::normalize () {
    this->value.normalize();
    return *this;
}

Vector Vector::normalized () {
    return Vector(this->value.normalized());
}
