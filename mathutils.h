#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <stdlib.h>

#include "macromagic.h"
#include "math.h"

#define MUT_TAU 6.283185307179586
#define MUT_PI 3.141592653589793

float mut_radians(float d)
{
	return d / (360.0f / MUT_TAU);
}

float mut_degrees(float r)
{
	return r * (360.0f / MUT_TAU);
}

float mut_randomf()
{
	return ((float)rand()/(float)RAND_MAX) * 1.0f;
}

int mut_randomi_range(int min, int max)
{
	return ((double)rand() / RAND_MAX) * (max - min) + min;
}

#define DEFINE_VEC_COMPONENTS_2 struct { float x, y; };
#define DEFINE_VEC_COMPONENTS_3 struct { float x, y, z; };
#define DEFINE_VEC_COMPONENTS_4 struct { float x, y, z, w; };

#define DEFINE_VEC_TYPE(N)\
typedef struct Vec##N {\
	union {\
		PRIMITIVE_CAT(DEFINE_VEC_COMPONENTS_, N)\
		float data[N];\
	};\
} Vec##N;

#define DEFINE_VEC_OP_FUNCS(N, OPERAND, NAME)\
void mut_vec##N##_##NAME(Vec##N* out, Vec##N const* rhs) {\
	for (int n = 0; n < N; ++n) out->data[n] OPERAND##= rhs->data[n]; }\
void mut_vec##N##_##NAME##c(Vec##N* out, Vec##N const* lhs, Vec##N const* rhs) {\
	mut_vec##N##_copy(out, lhs);\
	mut_vec##N##_##NAME(out, rhs); }\
void mut_vec##N##_##NAME##f(Vec##N* out, float s) {\
	for (int n = 0; n < N; ++n) out->data[n] OPERAND##= s; }\
void mut_vec##N##_##NAME##fc(Vec##N* out, Vec##N const* lhs, float s) {\
	mut_vec##N##_copy(out, lhs);\
	mut_vec##N##_##NAME##f(out, s); }\

#define DEFINE_VEC_FUNCS(N)\
void mut_vec##N##_init(Vec##N* out) {\
	for (int n = 0; n < N; ++n) out->data[n] = 0; }\
void mut_vec##N##_copy(Vec##N* out, Vec##N const* copy) {\
	for (int n = 0; n < N; ++n) out->data[n] = copy->data[n]; }\
int mut_vec##N##_compare(Vec##N const* lhs, Vec##N const* rhs) {\
	for (int n = 0; n < N; ++n)\
		if (lhs->data[n] != rhs->data[n])\
			return FALSE;\
	return TRUE; }\
DEFINE_VEC_OP_FUNCS(N, +, add)\
DEFINE_VEC_OP_FUNCS(N, -, subtract)\
DEFINE_VEC_OP_FUNCS(N, *, multiply)\
DEFINE_VEC_OP_FUNCS(N, /, divide)\
void mut_vec##N##_inverse(Vec##N* out, Vec##N const* v) {\
	mut_vec##N##_copy(out, v);\
	mut_vec##N##_multiplyf(out, -1); }\
float mut_vec##N##_dot(Vec##N const* lhs, Vec##N const* rhs) {\
	float dot = 0;\
	for (int n = 0; n < N; ++n)\
		dot += lhs->data[n] * rhs->data[n];\
	return dot; }\
float mut_vec##N##_magsq(Vec##N const* v) {\
	return mut_vec##N##_dot(v, v); }\
float mut_vec##N##_mag(Vec##N const* v) {\
	return sqrtf(mut_vec##N##_magsq(v)); }\
void mut_vec##N##_normal(Vec##N* out, Vec##N const* v) {\
	mut_vec##N##_dividefc(out, v, mut_vec##N##_mag(out)); }\
void mut_vec##N##_normalise(Vec##N* out) {\
	mut_vec##N##_normal(out, out); }\

#define DEFINE_VEC(N) DEFINE_VEC_TYPE(N) DEFINE_VEC_FUNCS(N)

DEFINE_VEC(2)
DEFINE_VEC(3)
DEFINE_VEC(4)

void mut_vec3_cross(Vec3* out, Vec3 const* lhs, Vec3 const* rhs) {
	out->x = lhs->y * rhs->z - lhs->z * rhs->y;
	out->y = lhs->z * rhs->x - lhs->x * rhs->z;
	out->z = lhs->x * rhs->y - lhs->y * rhs->x;
}

#define DEFINE_MAT_COMPONENTS_2 struct { float xx, xy, yx, yy; };
#define DEFINE_MAT_COMPONENTS_3 struct { float xx, xy, xz, yx, yy, yz, zx, zy, zz; };
#define DEFINE_MAT_COMPONENTS_4 struct { float xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww; };

#define DEFINE_MAT_TYPE(N, TYPE)\
typedef struct Mat##N {\
	union {\
		PRIMITIVE_CAT(DEFINE_MAT_COMPONENTS_, N)\
		TYPE data[N * N];\
	};\
} Mat##N;

#define DEFINE_MAT_COMPONENTWISE_OP_FUNCS(N, OPERAND, NAME)\
void mut_mat##N##_compwise_##NAME(Mat##N* out, Mat##N const* rhs) {\
	for (int n = 0; n < N * N; ++n) out->data[n] OPERAND##= rhs->data[n]; }\
void mut_mat##N##_compwise_##NAME##c(Mat##N* out, Mat##N const* lhs, Mat##N const* rhs) {\
	mut_mat##N##_copy(out, lhs);\
	mut_mat##N##_compwise_##NAME(out, rhs); }\
void mut_mat##N##_compwise_##NAME##f(Mat##N* out, float s) {\
	for (int n = 0; n < N * N; ++n) out->data[n] OPERAND##= s; }\
void mut_mat##N##_compwise_##NAME##fc(Mat##N* out, Mat##N const* lhs, float s) {\
	mut_mat##N##_copy(out, lhs);\
	mut_mat##N##_compwise_##NAME##f(out, s); }\

#define DEFINE_MAT_FUNCS(N, TYPE)\
void mut_mat##N##_init(Mat##N* out) {\
	for (int n = 0; n < N * N; ++n) out->data[n] = 0;\
	for (int n = 0; n < N; ++n) out->data[n * N + n] = 1; }\
void mut_mat##N##_copy(Mat##N* out, Mat##N const* m) {\
	for (int n = 0; n < N * N; ++n) out->data[n] = m->data[n]; }\
int mut_mat##N##_compare(Mat##N const* lhs, Mat##N const* rhs) {\
	for (int n = 0; n < N * N; ++n)\
		if (lhs->data[n] != rhs->data[n])\
			return FALSE;\
	return TRUE; }\
int mut_mat##N##_multiply(Mat##N* out, Mat##N const* lhs, Mat##N const* rhs) {\
	for (int nr = 0; nr < N; ++nr) {\
		for (int nc = 0; nc < N; ++nc) {\
			TYPE d = 0;\
			for (int n = 0; n < N; ++n)\
				d += lhs->data[nr * N + n] * rhs->data[n * N + nc];\
			out->data[nr * 4 + nc] = d;\
		}\
	} }\
DEFINE_MAT_COMPONENTWISE_OP_FUNCS(N, +, add)\
DEFINE_MAT_COMPONENTWISE_OP_FUNCS(N, -, subtract)\
DEFINE_MAT_COMPONENTWISE_OP_FUNCS(N, *, multiply)\
DEFINE_MAT_COMPONENTWISE_OP_FUNCS(N, /, divide)

#define DEFINE_MAT(N) DEFINE_MAT_TYPE(N, float) DEFINE_MAT_FUNCS(N, float)

DEFINE_MAT(2)
DEFINE_MAT(3)
DEFINE_MAT(4)

void mut_mat4_transform_t(Mat4* out, const float* tXYZ)
{
	mut_mat4_init(out);
	out->data[12] = tXYZ[0];
	out->data[13] = tXYZ[1];
	out->data[14] = tXYZ[2];
}

void mut_mat4_transform_s(Mat4* out, const float* sXYZ)
{
	mut_mat4_init(out);
	out->data[0]  = sXYZ[0];
	out->data[5]  = sXYZ[1];
	out->data[10] = sXYZ[2];
}

void mut_mat4_perspective(Mat4* out, float vFov, float aspect, float zNear, float zFar)
{
	const float zRange = zNear - zFar;
	const float f = 1.0f / tanf(vFov * 0.5f);

	mut_mat4_init(out);
	out->data[0]  = f / aspect;
	out->data[5]  = f;
	out->data[10] = (zFar + zNear) / zRange;
	out->data[11] = -1.0f;
	out->data[14] = (2.0f * zFar * zNear) / zRange;
	out->data[15] = 0.0f;
}

void mut_mat4_orthographic(Mat4* out, float left, float right, float top, float bottom, float zNear, float zFar)
{
	mut_mat4_init(out);
	out->data[0]  = 2.0f / (right - left);
	out->data[5]  = 2.0f / (top - bottom);
	out->data[10] = -2.0f / (zFar - zNear);
	out->data[12] = -((right + left) / (right - left));
	out->data[13] = -((top + bottom) / (top - bottom));
	out->data[14] = -((zFar + zNear) / (zFar - zNear));
}

void mut_mat4_view(Mat4* out, const float* rightXYZ, const float* upXYZ, const float* forwardXYZ, const float* eyeXYZ)
{
	out->data[0]  = rightXYZ[0];
	out->data[1]  = rightXYZ[1];
	out->data[2]  = rightXYZ[2];

	out->data[4]  = upXYZ[0];
	out->data[5]  = upXYZ[1];
	out->data[6]  = upXYZ[2];

	out->data[8]  = -forwardXYZ[0];
	out->data[9]  = -forwardXYZ[1];
	out->data[10] = -forwardXYZ[2];

	out->data[12] = -eyeXYZ[0];
	out->data[13] = -eyeXYZ[1];
	out->data[14] = -eyeXYZ[2];

	out->data[3]  =
	out->data[7]  =
	out->data[11] = 0;
	out->data[15] = 1.0f;
}

void mut_mat4_lookat(Mat4* out, const Vec3* eye, const Vec3* target, const Vec3* up)
{
	Vec3 f, s, u;

	mut_vec3_subtractc(&f, eye, target);
	mut_vec3_normalise(&f);

	mut_vec3_cross(&s, up, &f);
	mut_vec3_normalise(&s);

	mut_vec3_cross(&u, &f, &s);

	out->data[0]  = s.x;
	out->data[1]  = u.x;
	out->data[2]  = f.x;
	out->data[4]  = s.y;
	out->data[5]  = u.y;
	out->data[6]  = f.y;
	out->data[8]  = s.z;
	out->data[9]  = u.z;
	out->data[10] = f.z;
	out->data[12] = -mut_vec3_dot(&s, eye);
	out->data[13] = -mut_vec3_dot(&u, eye);
	out->data[14] = -mut_vec3_dot(&f, eye);
	out->data[3]  =
	out->data[7]  =
	out->data[11] = 0;
	out->data[15] = 1.0f;
}

typedef struct Quaternion
{
	float x, y, z, w;
} Quaternion;

void mut_quat_init(Quaternion* out)
{
	out->x = out->y = out->z = 0;
	out->w = 1;
}

void mut_quat_copy(Quaternion* out, const Quaternion* rhs)
{
	out->x = rhs->x;
	out->y = rhs->y;
	out->z = rhs->z;
	out->w = rhs->w;
}

int mut_quat_compare(const Quaternion* lhs, const Quaternion* rhs)
{
	return lhs->x == rhs->x
		&& lhs->y == rhs->y
		&& lhs->z == rhs->z
		&& lhs->w == rhs->w;
}

void mut_quat_from_axis_angle(Quaternion* out, const Vec3* axis, float radians)
{
	const float h = radians * 0.5f;
	const float sinh = sinf(h);
	out->x = axis->x * sinh;
	out->y = axis->y * sinh;
	out->z = axis->z * sinh;
	out->w = cosf(h);
}

void mut_quat_from_euler_angles(Quaternion* out, const Vec3* v)
{
	const float cy = cosf(v->z * 0.5f);
	const float sy = sinf(v->z * 0.5f);
	const float cp = cosf(v->y * 0.5f);
	const float sp = sinf(v->y * 0.5f);
	const float cr = cosf(v->x * 0.5f);
	const float sr = sinf(v->x * 0.5f);
	out->x = cy * cp * sr - sy * sp * cr;
	out->y = sy * cp * sr + cy * sp * cr;
	out->z = sy * cp * cr - cy * sp * sr;
	out->w = cy * cp * cr + sy * sp * sr;
}

void mut_quat_to_euler_angles(Vec3* out, const Quaternion* q)
{
	out->x = atan2f(2.0f * (q->w * q->x + q->y * q->z), 1.0f - 2.0f * (q->x * q->x + q->y * q->y));
	out->y = asinf(-2.0f * (q->w * q->y - q->z * q->x));
	out->z = atan2f(2.0f * (q->w * q->z + q->x * q->y), 1.0f - 2.0f * (q->y * q->y + q->z * q->z));
}

void mut_quat_normal(Quaternion* out, const Quaternion* q)
{
	float n = sqrtf(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);
	out->x /= n;
	out->y /= n;
	out->z /= n;
	out->w /= n;
}

void mut_quat_normalise(Quaternion* out)
{
	mut_quat_normal(out, out);
}

void mut_quat_multiply(Quaternion* out, const Quaternion* lhs, const Quaternion* rhs)
{
	out->x = lhs->w * rhs->x + lhs->x * rhs->w + lhs->y * rhs->z - lhs->z * rhs->y;
	out->y = lhs->w * rhs->y - lhs->x * rhs->z + lhs->y * rhs->w + lhs->z * rhs->x;
	out->z = lhs->w * rhs->z + lhs->x * rhs->y - lhs->y * rhs->x + lhs->z * rhs->w;
	out->w = lhs->w * rhs->w - lhs->x * rhs->x - lhs->y * rhs->y - lhs->z * rhs->z;
}

void mut_quat_multiply_vec3(Vec3* out, const Quaternion* lhs, const Vec3* rhs)
{
	Vec3 qxyz;
	qxyz.x = lhs->x;
	qxyz.y = lhs->y;
	qxyz.z = lhs->z;

	Vec3 a, b, c, cross;
	mut_vec3_multiplyfc(&a, &qxyz, 2.0f * mut_vec3_dot(&qxyz, rhs));
	mut_vec3_multiplyfc(&b, rhs, lhs->w * lhs->w - mut_vec3_dot(&qxyz, &qxyz));
	mut_vec3_cross(&cross, &qxyz, rhs);
	mut_vec3_multiplyfc(&c, &cross, 2.0f * lhs->w);
	mut_vec3_add(&a, &b);
	mut_vec3_add(&a, &c);

	mut_vec3_copy(out, &a);
}

void mut_quat_to_rotation_mat4(Mat4* out, const Quaternion* q)
{
	const float xx = q->x * q->x;
	const float xy = q->x * q->y;
	const float xz = q->x * q->z;
	const float xw = q->x * q->w;
	const float yy = q->y * q->y;
	const float yz = q->y * q->z;
	const float yw = q->y * q->w;
	const float zz = q->z * q->z;
	const float zw = q->z * q->w;
	out->data[ 0] = 1.0f - 2.0f * (yy + zz);
	out->data[ 1] = 2.0f * (xy - zw);
	out->data[ 2] = 2.0f * (xz + yw);
	out->data[ 3] = 0.0f;
	out->data[ 4] = 2.0f * (xy + zw);
	out->data[ 5] = 1.0f - 2.0f * (xx + zz);
	out->data[ 6] = 2.0f * (yz - xw);
	out->data[ 7] = 0.0f;
	out->data[ 8] = 2.0f * (xz - yw);
	out->data[ 9] = 2.0f * (yz + xw);
	out->data[10] = 1.0f - 2.0f * (xx + yy);
	out->data[11] = 0.0f;
	out->data[12] = 0.0f;
	out->data[13] = 0.0f;
	out->data[14] = 0.0f;
	out->data[15] = 1.0f;
}

#endif