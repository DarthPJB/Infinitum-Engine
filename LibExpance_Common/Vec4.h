#ifndef Vec4_H
#define Vec4_H
#pragma once
#include <math.h>
// Four Dimentional Vector
template <class T> class Vec4
{
public:
	Vec4();
	Vec4(T,T,T,T);
	Vec4 GetDirection(Vec4 Dest);
	T GetDistance(Vec4 Dest);
	
	Vec4 operator*(T Right);
	Vec4 operator*(Vec4 Right);
	void operator*=(T Right);
	void operator*=(Vec4 Right);
	Vec4 operator/(T Right);
	Vec4 operator/(Vec4 Right);
	void operator/=(T Right);
	void operator/=(Vec4 Right);
	Vec4 operator+(T Right);
	Vec4 operator+(Vec4 Right);
	void operator+=(T Right);
	void operator+=(Vec4 Right);
	Vec4 operator-(T Right);
	Vec4 operator-(Vec4 Right);
	void operator-=(T Right);
	void operator-=(Vec4 Right);
	
	T Length();
	void MakeNormal();
	Vec4 Normalise();
	T X;
	T Y;
	T Z;
	T W;
private:
    float PI;
};

//Template function to get destination co-ords from vector.
template <class T> inline T Vec4<T>::GetDistance(Vec4<T> Destination)
{
	Vec4<T> RetV;
	RetV.X = Destination.X - X;
	RetV.Y = Destination.Y - Y;
	RetV.Z = Destination.Z - Z;
	RetV.W = Destination.W - W;
	return RetV.Length();
}

//Template function to get direction vector.
template <class T> inline Vec4<T> Vec4<T>::GetDirection(Vec4<T> Direction)
{
	Vec4<T> RetVal;
	RetVal.X = Direction.X - X;
	RetVal.Y = Direction.Y - Y;
	RetVal.Z = Direction.Z - Z;
	RetVal.W = Direction.W - W;
	RetVal.MakeNormal();
	return RetVal;
}

//Template function for getting length of a vector.
template <class T> inline T Vec4<T>::Length()
{
	T Sum = X*X + Y*X + Z*Z + W*W;
	T RetVal = sqrt(Sum);
	return RetVal;
}

//Template function to return normalised vector.
template <class T> inline Vec4<T> Vec4<T>::Normalise()
{
	Vec4 Norm;
	T Scale;
	Scale = Length();
	Norm.X = X / Scale;
	Norm.Y = Y / Scale;
	Norm.Z = Z / Scale;
	Norm.W = W / Scale;
	return Norm;
}

//Template function to turn vector into normalised version.
template <class T> inline void Vec4<T>::MakeNormal()
{
	T Scale;
	Scale = Length();
	X /= Scale;
	Y /= Scale;
	Z /= Scale;
	W /= Scale;
}

//Template funciton for multiplying vector by constant.
template <class T> inline Vec4<T> Vec4<T>::operator*(T Right)
{
	Vec4<T> RetVal;
	RetVal.X = X * Right;
	RetVal.Y = Y * Right;
	RetVal.Z = Z * Right;
	RetVal.W = W * Right;
	return RetVal;
}

//Template function for multiplying one vector by another.
template <class T> inline Vec4<T> Vec4<T>::operator*(Vec4<T> Right)
{
	Vec4<T> RetVal;
	RetVal.X = X * Right.X;
	RetVal.Y = Y * Right.Y;
	RetVal.Z = Z * Right.Z;
	RetVal.W = W * Right.W;
	return RetVal;
}

//Templace function for multiplaying vector by constant.
template <class T> inline void Vec4<T>::operator*=(T Right)
{
	X *= Right;
	Y *= Right;
	Z *= Right;
	W *= Right;
}

//Template function for multiplying one vector by another.
template <class T> inline void Vec4<T>::operator*=(Vec4<T> Right)
{
	X *= Right.X;
	Y *= Right.Y;
	Z *= Right.Z;
	W *= Right.W;
}

//Template funciton for dividing a vector by a constant.
template <class T> inline Vec4<T> Vec4<T>::operator/(T Right)
{
	Vec4<T> RetVal;
	RetVal.X = X / Right;
	RetVal.Y = Y / Right;
	RetVal.Z = Z / Right;
	RetVal.W = W / Right;
	return RetVal;
}

//Template function for dividing one vector by another.
template <class T> inline Vec4<T> Vec4<T>::operator/(Vec4<T> Right)
{
	Vec4<T> RetVal;
	RetVal.X = X / Right.X;
	RetVal.Y = Y / Right.Y;
	RetVal.Z = Z / Right.Z;
	RetVal.W = W / Right.W;
	return RetVal;
}

//Template funciton for dividing a vector by a constant.
template <class T> inline void Vec4<T>::operator/=(T Right)
{
	X /= Right;
	Y /= Right;
	Z /= Right;
	W /= Right;
}

//Template function for dividing one vector by another.
template <class T> inline void Vec4<T>::operator/=(Vec4<T> Right)
{
	X /= Right.X;
	Y /= Right.Y;
	Z /= Right.Z;
	W /= Right.W;
}

//Template funciton for adding a constant to a vector.
template <class T> inline Vec4<T> Vec4<T>::operator+(T Right)
{
	Vec4<T> RetVal;
	RetVal.X = X + Right;
	RetVal.Y = Y + Right;
	RetVal.Z = Z + Right;
	RetVal.W = W + Right;
	return RetVal;
}

//Template function for adding one vector to another.
template <class T> inline Vec4<T> Vec4<T>::operator+(Vec4<T> Right)
{
	Vec4<T> RetVal;
	RetVal.X = X + Right.X;
	RetVal.Y = Y + Right.Y;
	RetVal.Z = Z + Right.Z;
	RetVal.W = W + Right.W;
	return RetVal;
}

//Template funciton for adding a constant to a vector.
template <class T> inline void Vec4<T>::operator+=(T Right)
{
	X += Right;
	Y += Right;
	Z += Right;
	W += Right;
}

//Template function for adding one vector to another.
template <class T> inline void Vec4<T>::operator+=(Vec4<T> Right)
{
	X += Right.X;
	Y += Right.Y;
	Z += Right.Z;
	W += Right.W;
}

//Template funciton for subtracting a constant from a vector.
template <class T> inline Vec4<T> Vec4<T>::operator-(T Right)
{
	Vec4<T> RetVal;
	RetVal.X = X - Right;
	RetVal.Y = Y - Right;
	RetVal.Z = Z - Right;
	RetVal.W = W - Right;
	return RetVal;
}

//Template function for subtracting one vector from another.
template <class T> inline Vec4<T> Vec4<T>::operator-(Vec4<T> Right)
{
	Vec4<T> RetVal;
	RetVal.X = X - Right.X;
	RetVal.Y = Y - Right.Y;
	RetVal.Z = Z - Right.Z;
	RetVal.W = W - Right.W;
	return RetVal;
}


//Template funciton for subtracting a constant from a vector.
template <class T> inline void Vec4<T>::operator-=(T Right)
{
	X -= Right;
	Y -= Right;
	Z -= Right;
	W -= Right;
}

//Template function for subtracting one vector from another.
template <class T> inline void Vec4<T>::operator-=(Vec4<T> Right)
{
	X -= Right.X;
	Y -= Right.Y;
	Z -= Right.Z;
	W -= Right.W;
}

//Template function for creating empty vector.
template <class T> inline Vec4<T>::Vec4()
{
	PI = 3.1415926535;
	X=0;
	Y=0;
	Z=0;
	W=0;
}

//Template function for creating vector with starting values.
template <class T> inline Vec4<T>::Vec4(T x, T y, T z, T w)
{
	PI = 3.1415926535;
	X=x;
	Y=y;
	Z=z;
	W=w;
}
#endif