#pragma once

#include <sstream>

#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580
#define CHECK_VALID(_v) 0
#define Assert(_exp) ((void)0)

class Vector
{
public:

	

    Vector(void)
    {
        Invalidate();
    }
    Vector(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
    Vector(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

	inline float sqrt2(float sqr)
	{
		float root = 0;

		__asm
		{
			sqrtss xmm0, sqr
			movss root, xmm0
		}

		return root;
	}

	float x, y, z;

	Vector Vector::Angle()
	{
		if (!x && !y)
			return Vector(0, 0, 0);

		float roll = 0;



		return Vector(atan2f(-z, sqrt2(x*x + y*y)) * 180.0f / M_PI, atan2f(y, x) * 180.0f / M_PI, roll);
	}

	inline float Vector::Length2DSqr(void) const
	{
		return (x * x + y * y);
	}

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }
    bool IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    void Invalidate()
    {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float& operator[](int i)
    {
        return ((float*)this)[i];
    }
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

	__inline void Mul(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}

	__inline void MulAdd(const Vector& a, const Vector& b, float scalar) {
		x = a.x + b.x * scalar;
		y = a.y + b.y * scalar;
		z = a.z + b.z * scalar;
	}


    void Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const Vector& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    bool operator!=(const Vector& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    Vector& operator+=(const Vector& v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector& operator-=(const Vector& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    Vector& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    Vector& operator*=(const Vector& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vector& operator/=(const Vector& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    Vector& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    Vector& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    Vector& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    void NormalizeInPlace()
    {
        *this = Normalized();
    }
    Vector Normalized() const
    {
        Vector res = *this;
        float l = res.Length();
        if(l != 0.0f) {
            res /= l;
        } else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

    float DistTo(const Vector &vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
    float DistToSqr(const Vector &vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
    float Dot(const Vector& vOther) const
    {
        return (x*vOther.x + y*vOther.y + z*vOther.z);
    }
    float Length() const
    {
        return sqrt(x*x + y*y + z*z);
    }
    float LengthSqr(void) const
    {
        return (x*x + y*y + z*z);
    }
    float Length2D() const
    {
        return sqrt(x*x + y*y);
    }

    Vector& operator=(const Vector &vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    Vector operator-(void) const
    {
        return Vector(-x, -y, -z);
    }
    Vector operator+(const Vector& v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    Vector operator*(float fl) const
    {
        return Vector(x * fl, y * fl, z * fl);
    }
    Vector operator*(const Vector& v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }
    Vector operator/(float fl) const
    {
        return Vector(x / fl, y / fl, z / fl);
    }
    Vector operator/(const Vector& v) const
    {
        return Vector(x / v.x, y / v.y, z / v.z);
    }

   
};

inline Vector operator*(float lhs, const Vector& rhs)
{
    return rhs * lhs;
}
inline Vector operator/(float lhs, const Vector& rhs)
{
    return rhs / lhs;
}

inline void VectorMultiply(const Vector &a, float b, Vector &c)
{
	CHECK_VALID(a);
	Assert(IsFinite(b));
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

inline void VectorMA(const Vector &start, float scale, const Vector &direction, Vector &dest)
{
	CHECK_VALID(start);
	CHECK_VALID(direction);

	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}

inline void VectorAdd(const Vector &a, const Vector &b, Vector &c)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

inline void VectorSubtract(const Vector &a, const Vector &b, Vector &c)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

class __declspec(align(16)) VectorAligned : public Vector
{
public:
    inline VectorAligned(void) {};
    inline VectorAligned(float X, float Y, float Z)
    {
        Init(X, Y, Z);
    }

public:
    explicit VectorAligned(const Vector &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    VectorAligned& operator=(const Vector &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    VectorAligned& operator=(const VectorAligned &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;
};