#include "Transform.h"

Matrix4 Transform::ComputeWorldMatrix()
{
    return Matrix4::CreateScale(scale) *
           Matrix4::CreateRotationFromDegree(rotation) *
           Matrix4::CreateTranslation(position);
}