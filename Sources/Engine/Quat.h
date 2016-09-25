#pragma once

#include "pch.h"
#include "MeshObject.h"

class Quat
{
public:
	union {
		struct {
			/**
			* Holds the float component of the Quat.
			*/
			float r;

			/**
			* Holds the first complex component of the
			* Quat.
			*/
			float i;

			/**
			* Holds the second complex component of the
			* Quat.
			*/
			float j;

			/**
			* Holds the third complex component of the
			* Quat.
			*/
			float k;
		};

		/**
		* Holds the Quat data in array form.
		*/
		float data[4];
	};

	// ... other Quat code as before ...

	/**
	* The default constructor creates a Quat representing
	* a zero rotation.
	*/
	Quat() : r(1), i(0), j(0), k(0) {}

	/**
	* The explicit constructor creates a Quat with the given
	* components.
	*
	* @param r The float component of the rigid body's orientation
	* Quat.
	*
	* @param i The first complex component of the rigid body's
	* orientation Quat.
	*
	* @param j The second complex component of the rigid body's
	* orientation Quat.
	*
	* @param k The third complex component of the rigid body's
	* orientation Quat.
	*
	* @note The given orientation does not need to be normalised,
	* and can be zero. This function will not alter the given
	* values, or normalise the Quat. To normalise the
	* Quat (and make a zero Quat a legal rotation),
	* use the normalise function.
	*
	* @see normalise
	*/
	Quat(const float r, const float i, const float j, const float k)
		: r(r), i(i), j(j), k(k)
	{
	}

	/**
	* Normalises the Quat to unit length, making it a valid
	* orientation Quat.
	*/
	void normalise()
	{
		float d = r*r + i*i + j*j + k*k;

		// Check for zero length Quat, and use the no-rotation
		// Quat in that case.
		if (d < 0.01f) {
			r = 1;
			return;
		}

		d = ((float)1.0) / Kore::sqrt(d);
		r *= d;
		i *= d;
		j *= d;
		k *= d;
	}

	/**
	* Multiplies the Quat by the given Quat.
	*
	* @param multiplier The Quat by which to multiply.
	*/
	void operator *=(const Quat &multiplier)
	{
		Quat q = *this;
		r = q.r*multiplier.r - q.i*multiplier.i -
			q.j*multiplier.j - q.k*multiplier.k;
		i = q.r*multiplier.i + q.i*multiplier.r +
			q.j*multiplier.k - q.k*multiplier.j;
		j = q.r*multiplier.j + q.j*multiplier.r +
			q.k*multiplier.i - q.i*multiplier.k;
		k = q.r*multiplier.k + q.k*multiplier.r +
			q.i*multiplier.j - q.j*multiplier.i;
	}

	/**
	* Adds the given vector to this, scaled by the given amount.
	* This is used to update the orientation Quat by a rotation
	* and time.
	*
	* @param vector The vector to add.
	*
	* @param scale The amount of the vector to add.
	*/
	void addScaledVector(const Kore::vec3& vector, float scale)
	{
		Quat q(0,
			vector.x() * scale,
			vector.y() * scale,
			vector.z() * scale);
		q *= *this;
		r += q.r * ((float)0.5);
		i += q.i * ((float)0.5);
		j += q.j * ((float)0.5);
		k += q.k * ((float)0.5);
	}

	void rotateByVector(const Kore::vec3& vector)
	{
		Quat q(0, vector.x(), vector.y(), vector.z());
		(*this) *= q;
	}

	Kore::mat4 getMatrix()
	{
		Kore::mat4 result = Kore::mat4::Identity();

		result.data[0] = 1 - (2 * j*j + 2 * k*k);
		result.data[1] = 2 * i*j + 2 * k*r;
		result.data[2] = 2 * i*k - 2 * j*r;

		result.data[4] = 2 * i*j - 2 * k*r;
		result.data[5] = 1 - (2 * i*i + 2 * k*k);
		result.data[6] = 2 * j*k + 2 * i*r;

		result.data[8] = 2 * i*k + 2 * j*r;
		result.data[9] = 2 * j*k - 2 * i*r;
		result.data[10] = 1 - (2 * i*i + 2 * j*j);

		return result;
	}
};