#pragma once
#include "Math.h"

class Transform
{
public:
	Transform() :
		position(Vector3::Zero),
		scale(Vector3(100.f, 100.f, 1.0f)),
		rotation(Vector3::Zero)
	{
	}

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& pos) { position = pos; }
	Vector3 GetRotation() const { return rotation; }
	void SetRotation(const Vector3& rot) { rotation = rot; }
	Vector3 GetScale() const { return scale; }
	void SetScale(const Vector3& s) { scale = s; }

	Matrix4 ComputeWorldMatrix();

private:
	Vector3 position;
	Vector3 rotation; // Degrees
	Vector3 scale;
};