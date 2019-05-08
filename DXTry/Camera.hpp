#ifndef DXTRY_CAMERA_HPP
#define DXTRY_CAMERA_HPP

#include "includes.hpp"

struct Camera {
	Vector3 position { 0.0f, 0.7f, 1.5f };
	float yaw = 0, pitch = 0;

	float near_plane = 0.01f;
	float far_plane = 100.0f;
	float aspect_ratio = 1.0f;
	float FOV = 70;

	Vector3 direction() {
		float y = sinf(pitch);
		float r = cosf(pitch);
		float z = r * cosf(yaw);
		float x = r * sinf(yaw);
		return -Vector3(x, y, z);
	}

	Vector3 left() {
		Vector3 x = direction().Cross(Vector3::Up);
		x.Normalize();
		return x;
	}

	Matrix view() {
		Vector3 at = position + direction();
		return Matrix::CreateLookAt(position, at, Vector3::Up).Transpose();
	}

	Matrix projection() {
		return Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(FOV), aspect_ratio, near_plane, far_plane).Transpose();
	}
};

#endif // !DXTRY_CAMERA_HPP
