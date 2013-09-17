//
//  RNQuaternion.h
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_QUATERNION_H__
#define __RAYNE_QUATERNION_H__

#include "RNBase.h"
#include "RNVector.h"
#include "RNMatrixQuaternion.h"

namespace RN
{	
	RN_INLINE Quaternion::Quaternion()
	{
		MakeIdentity();
	}
	
	RN_INLINE Quaternion::Quaternion(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	
	RN_INLINE Quaternion::Quaternion(const Vector3& euler)
	{
		MakeEulerAngle(euler);
	}
	
	RN_INLINE Quaternion::Quaternion(const Vector4& axis)
	{
		MakeAxisAngle(axis);
	}
	
	
	
	RN_INLINE Quaternion& Quaternion::operator+= (const Quaternion& other)
	{
		w += other.w;
		x += other.x;
		y += other.y;
		z += other.z;
		
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator-= (const Quaternion& other)
	{
		w -= other.w;
		x -= other.x;
		y -= other.y;
		z -= other.z;
		
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator*= (const Quaternion& other)
	{
		Vector4 temp = Vector4(x, y, z, w);
		
		w = -temp.x * other.x - temp.y * other.y - temp.z * other.z + temp.w * other.w;
		x =  temp.x * other.w + temp.y * other.z - temp.z * other.y + temp.w * other.x;
		y = -temp.x * other.z + temp.y * other.w + temp.z * other.x + temp.w * other.y;
		z =  temp.x * other.y - temp.y * other.x + temp.z * other.w + temp.w * other.z;
		
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator/= (const Quaternion& other)
	{
		Vector4 temp = Vector4(x, y, z, w);
		
		w = (temp.w * other.w + temp.x * other.x + temp.y * other.y + temp.z * other.z) / (other.w * other.w + other.x * other.x + other.y * other.y + other.z * other.z);
		x = (temp.x * other.w - temp.w * other.x - temp.z * other.y + temp.y * other.z) / (other.w * other.w + other.x * other.x + other.y * other.y + other.z * other.z);
		y = (temp.y * other.w + temp.z * other.x - temp.w * other.y - temp.x * other.z) / (other.w * other.w + other.x * other.x + other.y * other.y + other.z * other.z);
		z = (temp.z * other.w - temp.y * other.x + temp.x * other.y - temp.w * other.z) / (other.w * other.w + other.x * other.x + other.y * other.y + other.z * other.z);
		
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator*= (const Vector4& other)
	{
		Quaternion quaternion(other);
		*this *= quaternion;
		
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator/= (const Vector4& other)
	{
		Quaternion quaternion(other);
		*this /= quaternion;
		
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator+= (const Vector3& other)
	{
		Vector3 euler = GetEulerAngle();
		euler += other;
		
		MakeEulerAngle(euler);
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator-= (const Vector3& other)
	{
		Vector3 euler = GetEulerAngle();
		euler -= other;
		
		MakeEulerAngle(euler);
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator*= (float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		
		return *this;
	}
	
	RN_INLINE Quaternion& Quaternion::operator/= (float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		
		return *this;
	}
	
	RN_INLINE Quaternion Quaternion::operator+ (const Quaternion& other) const
	{
		Quaternion result(*this);
		result += other;
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::operator- (const Quaternion& other) const
	{
		Quaternion result(*this);
		result -= other;
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::operator* (const Quaternion& other) const
	{
		Quaternion result(*this);
		result *= other;
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::operator/ (const Quaternion& other) const
	{
		Quaternion result(*this);
		result /= other;
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::operator* (const Vector4& other) const
	{
		Quaternion result(*this);
		result *= other;
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::operator/ (const Vector4& other) const
	{
		Quaternion result(*this);
		result /= other;
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::operator+ (const Vector3& other) const
	{
		Vector3 euler = GetEulerAngle();
		euler += other;
		
		return Quaternion(euler);
	}
	
	RN_INLINE Quaternion Quaternion::operator- (const Vector3& other) const
	{
		Vector3 euler = GetEulerAngle();
		euler -= other;
		
		return Quaternion(euler);
	}
	
	RN_INLINE Quaternion Quaternion::operator* (float scalar) const
	{
		Quaternion result(*this);
		result *= scalar;
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::operator/ (float scalar) const
	{
		Quaternion result(*this);
		result /= scalar;
		
		return result;
	}
	
	RN_INLINE void Quaternion::MakeIdentity()
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}
	
	RN_INLINE void Quaternion::MakeEulerAngle(const Vector3& euler)
	{
		const float Pi_360(k::Pi / 360.0f);
		
		float fSinYaw = Math::Sin(euler.x * Pi_360);
		float fCosYaw = Math::Cos(euler.x * Pi_360);
		float fSinPitch   = Math::Sin(euler.y * Pi_360);
		float fCosPitch   = Math::Cos(euler.y * Pi_360);
		float fSinRoll  = Math::Sin(euler.z * Pi_360);
		float fCosRoll  = Math::Cos(euler.z * Pi_360);
		
		//qy*qx*qz;
		w = fSinYaw * fSinPitch * fSinRoll + fCosYaw * fCosPitch * fCosRoll;
		x = fCosYaw * fSinPitch * fCosRoll + fSinYaw * fCosPitch * fSinRoll;
		y = -fCosYaw * fSinPitch * fSinRoll + fSinYaw * fCosPitch * fCosRoll;
		z = -fSinYaw * fSinPitch * fCosRoll + fCosYaw * fCosPitch * fSinRoll;
		
		Normalize();
	}
	
	RN_INLINE void Quaternion::MakeAxisAngle(const Vector4& axis)
	{
		float half = axis.w * k::Pi / 360.0f;
		float fsin = Math::Sin(half);
		
		w = Math::Cos(half);
		x = fsin * axis.x;
		y = fsin * axis.y;
		z = fsin * axis.z;
		
		Normalize();
	}
	
	RN_INLINE void Quaternion::MakeLerpS(const Quaternion& start, const Quaternion& end, float factor)
	{
		Quaternion quat1(start);
		Quaternion quat2(end);
		
		float angle = quat1.Dot(quat2);
		if(angle < 0.0f)
		{
			quat1 *= -1.0f;
			angle *= -1.0f;
		}
		
		float scale, inverseScale;
		
		if((angle + 1.0f) > 0.05f)
		{
			if((1.0f - angle) >= 0.05f)
			{
				float theta = acos(angle);
				float inverseTheta = 1.0f / Math::Sin(theta);
				
				scale = Math::Sin(theta * (1.0f - factor)) * inverseTheta;
				inverseScale = Math::Sin(theta * factor) * inverseTheta;
			}
			else
			{
				scale = 1.0f - factor;
				inverseScale = factor;
			}
		}
		else
		{
			quat2 = Quaternion(-quat1.y, quat1.x, -quat1.w, quat1.z);
			scale = Math::Sin(k::Pi * (0.5f - factor));
			inverseScale = Math::Sin(k::Pi * factor);
		}
		
		*this = (quat1 * scale) + (quat2 * inverseScale);
	}
	
	RN_INLINE void Quaternion::MakeLerpN(const Quaternion& start, const Quaternion& end, float factor)
	{
		float inverseFactor = 1.0f - factor;
		*this = (end * factor) + (start * inverseFactor);
	}
	
	RN_INLINE void Quaternion::LookAt(const Vector3& tdir, const Vector3& tup, bool forceup)
	{
		Vector3 dir = Vector3(tdir);
		Vector3 up  = Vector3(tup);
		Vector3 right = up.Cross(dir);
		right.Normalize();
		
		if(forceup)
		{
			dir = up.Cross(right);
			up.Normalize();
			dir.Normalize();
		}
		else
		{
			up = dir.Cross(right);
			up.Normalize();
			dir.Normalize();
		}
		
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".
		// Implementation taken from Ogre3D.
		
		float kRot[3][3];
		kRot[0][0] = right.x;
		kRot[1][0] = right.y;
		kRot[2][0] = right.z;
		kRot[0][1] = up.x;
		kRot[1][1] = up.y;
		kRot[2][1] = up.z;
		kRot[0][2] = dir.x;
		kRot[1][2] = dir.y;
		kRot[2][2] = dir.z;
		
		float fTrace = kRot[0][0] + kRot[1][1] + kRot[2][2];
		float fRoot;
		
		if(fTrace > 0.0f)
		{
			fRoot = Math::Sqrt(fTrace + 1.0f);
			w = 0.5f * fRoot;
			
			fRoot = 0.5f / fRoot;
			x = (kRot[2][1] - kRot[1][2]) * fRoot;
			y = (kRot[0][2] - kRot[2][0]) * fRoot;
			z = (kRot[1][0] - kRot[0][1]) * fRoot;
		}
		else
		{
			static size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if(kRot[1][1] > kRot[0][0])
				i = 1;
			
			if(kRot[2][2] > kRot[i][i])
				i = 2;
			
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];
			
			fRoot = Math::Sqrt(kRot[i][i] - kRot[j][j] - kRot[k][k] + 1.0f);
			
			float *apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5f * fRoot;
			
			fRoot = 0.5f / fRoot;
			w = (kRot[k][j] - kRot[j][k]) * fRoot;
			*apkQuat[j] = (kRot[j][i] + kRot[i][j]) * fRoot;
			*apkQuat[k] = (kRot[k][i] + kRot[i][k]) * fRoot;
		}
		
		Normalize();
	}
	
	RN_INLINE Quaternion &Quaternion::Normalize()
	{
		float length = Length();
		if(length > k::EpsilonFloat)
		{
			float fac = 1.0f / length;
			w *= fac;
			x *= fac;
			y *= fac;
			z *= fac;
		}
		
		return *this;
	}
	
	RN_INLINE Quaternion &Quaternion::Conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
		
		return *this;
	}
	
	RN_INLINE Quaternion Quaternion::Normalize() const
	{
		return Quaternion(*this).Normalize();
	}
	
	RN_INLINE Quaternion Quaternion::Conjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}
	
	RN_INLINE Quaternion Quaternion::LerpS(const Quaternion& other, float factor) const
	{
		Quaternion result;
		result.MakeLerpS(*this, other, factor);
		
		return result;
	}
	
	RN_INLINE Quaternion Quaternion::LerpN(const Quaternion& other, float factor) const
	{
		Quaternion result;
		result.MakeLerpN(*this, other, factor);
		
		return result;
	}
	
	RN_INLINE Vector3 Quaternion::RotateVector(const Vector3& vector) const
	{
		Quaternion vectorquat(vector.x, vector.y, vector.z, 0.0f);
		Quaternion resultquat = (*this) * vectorquat * Conjugate();
		return Vector3(resultquat.x, resultquat.y, resultquat.z);
	}
	
	RN_INLINE Vector4 Quaternion::RotateVector(const Vector4& vector) const
	{
		Quaternion vectorquat(vector.x, vector.y, vector.z, 0.0f);
		Quaternion resultquat = (*this) * vectorquat * Conjugate();
		return Vector4(resultquat.x, resultquat.y, resultquat.z, vector.w);
	}
	
	RN_INLINE Matrix Quaternion::GetRotationMatrix() const
	{
		Matrix result;
		
		float xx = x * x;
		float yy = y * y;
		float zz = z * z;
		float xy = x * y;
		float xz = x * z;
		float xw = x * w;
		float yz = y * z;
		float yw = y * w;
		float zw = z * w;
		
		result.m[0] = 1.0f - 2.0f * (yy + zz);
		result.m[4] = 2.0f * (xy - zw);
		result.m[8] = 2.0f * (xz + yw);
		result.m[1] = 2.0f * (xy + zw);
		result.m[5] = 1.0f - 2.0f * (xx + zz);
		result.m[9] = 2.0f * (yz - xw);
		result.m[2] = 2.0f * (xz - yw);
		result.m[6] = 2.0f * (yz + xw);
		result.m[10] = 1.0f - 2.0f * (xx + yy);
		
		return result;
	}
	
	RN_INLINE Vector3 Quaternion::GetEulerAngle() const
	{
		float xx = x * x;
		float yy = y * y;
		float zz = z * z;
		float xy = x * y;
		float xz = x * z;
		float xw = x * w;
		float yz = y * z;
		float yw = y * w;
		float zw = z * w;
		
		Vector3 result;
		
		result.y = asin(fmax(fmin(-2.0f * (yz - xw), 1.0), -1.0));
		double cy = cos(result.y);
		if(Math::FastAbs(cy) > k::EpsilonFloat)
		{
			result.x = atan2(2.0f * (xz + yw)/cy, (1.0f - 2.0f * (xx + yy))/cy);
			result.z = atan2(2.0f * (xy + zw)/cy, (1.0f - 2.0f * (xx + zz))/cy);
		}
		else
		{
			result.z = 0.0f;
			if(result.y > 0.0f)
			{
				result.x = atan2(2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
			}
			else
			{
				result.x = atan2(-2.0f * (xy - zw), -1.0f - 2.0f * (yy + zz));
			}
		}
		
		result *= 180.0f / k::Pi;
		return result;
	}
	
	RN_INLINE Vector4 Quaternion::GetAxisAngle() const
	{
		Vector4 res;
		const float scale = Math::Sqrt(x * x + y * y + z * z);
		
		if(scale == 0.0f || w > 1.0f || w < -1.0f)
		{
			res.w = 0.0f;
			res.x = 0.0f;
			res.y = 1.0f;
			res.z = 0.0f;
		}
		else
		{
			const float invscale = 1.0f / scale;
			res.w = (360.0f / k::Pi) * acos(w);
			res.x = x * invscale;
			res.y = y * invscale;
			res.z = z * invscale;
		}
		return res;
	}
	
	RN_INLINE float Quaternion::Length() const
	{
		return Math::Sqrt(x * x + y * y + z * z + w * w);
	}
	
	RN_INLINE float Quaternion::Dot(const Quaternion& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}
}

#endif /* __RAYNE_QUATERNION_H__ */
