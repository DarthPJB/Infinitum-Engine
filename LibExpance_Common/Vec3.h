#ifndef Vec3_H
#define Vec3_H
#include <math.h>
template <class T> class Vec3
{
public:
	Vec3();
	Vec3(T,T,T);
	Vec3 GetDirection(Vec3 Dest);
	T GetDistance(Vec3 Dest);
	bool IsPerp(Vec3);
	
	Vec3 operator*(T Right);
	Vec3 operator*(Vec3 Right);
	void operator*=(T Right);
	void operator*=(Vec3 Right);
	Vec3 operator/(T Right);
	Vec3 operator/(Vec3 Right);
	void operator/=(T Right);
	void operator/=(Vec3 Right);
	Vec3 operator+(T Right);
	Vec3 operator+(Vec3 Right);
	void operator+=(T Right);
	void operator+=(Vec3 Right);
	Vec3 operator-(T Right);
	Vec3 operator-(Vec3 Right);
	void operator-=(T Right);
	void operator-=(Vec3 Right);
	
	T Length();
	void MakeNormal();
	Vec3 Normalise();
	T X;
	T Y;
	T Z;
private:
    static const float PI;
};

//Extra vector operations
template <class T> float Dot(Vec3<T> A,Vec3<T> B);
template <class T> Vec3<T> Cross(Vec3<T> A,Vec3<T> B);
template <class T> float AngleRad(Vec3<T> A,Vec3<T> B);
template <class T> float AngleDeg(Vec3<T> A,Vec3<T> B);
template <class T> Vec3<T> Axis(Vec3<T> A,Vec3<T> B);
template <class T> Vec3<T> TripleProduct(Vec3<T> A,Vec3<T> B,Vec3<T> C);
template <class T> Vec3<T> GetDirection(Vec3<T> Origin,Vec3<T> Dest);
template <class T> T GetDistance(Vec3<T> Origin,Vec3<T> Dest);
template <class T> bool ArePerp(Vec3<T> A, Vec3<T> B);



//Template function to get destination co-ords from vector.
template <class T> inline T Vec3<T>::GetDistance(Vec3<T> Destination)
{
	Vec3<T> RetV;
	RetV.X = Destination.X - X;
	RetV.Y = Destination.Y - Y;
	RetV.Z = Destination.Z - Z;
	return RetV.Length();
}

//Template function to get direction vector.
template <class T> inline Vec3<T> Vec3<T>::GetDirection(Vec3<T> Direction)
{
	Vec3<T> RetVal;
	RetVal.X = Direction.X - X;
	RetVal.Y = Direction.Y - Y;
	RetVal.Z = Direction.Z - Z;
	RetVal.MakeNormal();
	return RetVal;
}

//Template function for getting if a vector is perpendicular to this one
template <class T> inline bool Vec3<T>::IsPerp(Vec3<T> B)
{
	float dot;
	dot = (X * B.X) + (Y * B.Y) + (Z * B.Z); //Dot product
	
	if(dot = 0){return true;}	//If the dot product = 0 
	else{return false;}			//then cos(theta) = 0 therefore theta = 90 -> perpendicular
}

//Template function for getting length of a vector.
template <class T> inline T Vec3<T>::Length()
{
	T Sum = X*X + Y*Y + Z*Z;
	T RetVal = sqrt(Sum);
	return RetVal;
}

//Template function to return normalised vector.
template <class T> inline Vec3<T> Vec3<T>::Normalise()
{
	Vec3 Norm;
	T Scale;
	Scale = Length();
	Norm.X = X / Scale;
	Norm.Y = Y / Scale;
	Norm.Z = Z / Scale;
	return Norm;
}

//Template function to turn vector into normalised version.
template <class T> inline void Vec3<T>::MakeNormal()
{
	T Scale;
	Scale = Length();
	X /= Scale;
	Y /= Scale;
	Z /= Scale;
}

//Template funciton for multiplying vector by constant.
template <class T> inline Vec3<T> Vec3<T>::operator*(T Right)
{
	Vec3<T> RetVal;
	RetVal.X = X * Right;
	RetVal.Y = Y * Right;
	RetVal.Z = Z * Right;
	return RetVal;
}

//Template function for multiplying one vector by another.
template <class T> inline Vec3<T> Vec3<T>::operator*(Vec3<T> Right)
{
	Vec3<T> RetVal;
	RetVal.X = X * Right.X;
	RetVal.Y = Y * Right.Y;
	RetVal.Z = Z * Right.Z;
	return RetVal;
}

//Templace function for multiplaying vector by constant.
template <class T> inline void Vec3<T>::operator*=(T Right)
{
	X *= Right;
	Y *= Right;
	Z *= Right;
}

//Template function for multiplying one vector by another.
template <class T> inline void Vec3<T>::operator*=(Vec3<T> Right)
{
	X *= Right.X;
	Y *= Right.Y;
	Z *= Right.Z;
}

//Template funciton for dividing a vector by a constant.
template <class T> inline Vec3<T> Vec3<T>::operator/(T Right)
{
	Vec3<T> RetVal;
	RetVal.X = X / Right;
	RetVal.Y = Y / Right;
	RetVal.Z = Z / Right;
	return RetVal;
}

//Template function for dividing one vector by another.
template <class T> inline Vec3<T> Vec3<T>::operator/(Vec3<T> Right)
{
	Vec3<T> RetVal;
	RetVal.X = X / Right.X;
	RetVal.Y = Y / Right.Y;
	RetVal.Z = Z / Right.Z;
	return RetVal;
}

//Template funciton for dividing a vector by a constant.
template <class T> inline void Vec3<T>::operator/=(T Right)
{
	X /= Right;
	Y /= Right;
	Z /= Right;
}

//Template function for dividing one vector by another.
template <class T> inline void Vec3<T>::operator/=(Vec3<T> Right)
{
	X /= Right.X;
	Y /= Right.Y;
	Z /= Right.Z;
}

//Template funciton for adding a constant to a vector.
template <class T> inline Vec3<T> Vec3<T>::operator+(T Right)
{
	Vec3<T> RetVal;
	RetVal.X = X + Right;
	RetVal.Y = Y + Right;
	RetVal.Z = Z + Right;
	return RetVal;
}

//Template function for adding one vector to another.
template <class T> inline Vec3<T> Vec3<T>::operator+(Vec3<T> Right)
{
	Vec3<T> RetVal;
	RetVal.X = X + Right.X;
	RetVal.Y = Y + Right.Y;
	RetVal.Z = Z + Right.Z;
	return RetVal;
}

//Template funciton for adding a constant to a vector.
template <class T> inline void Vec3<T>::operator+=(T Right)
{
	X += Right;
	Y += Right;
	Z += Right;
}

//Template function for adding one vector to another.
template <class T> inline void Vec3<T>::operator+=(Vec3<T> Right)
{
	X += Right.X;
	Y += Right.Y;
	Z += Right.Z;
}

//Template funciton for subtracting a constant from a vector.
template <class T> inline Vec3<T> Vec3<T>::operator-(T Right)
{
	Vec3<T> RetVal;
	RetVal.X = X - Right;
	RetVal.Y = Y - Right;
	RetVal.Z = Z - Right;
	return RetVal;
}

//Template function for subtracting one vector from another.
template <class T> inline Vec3<T> Vec3<T>::operator-(Vec3<T> Right)
{
	Vec3<T> RetVal;
	RetVal.X = X - Right.X;
	RetVal.Y = Y - Right.Y;
	RetVal.Z = Z - Right.Z;
	return RetVal;
}


//Template funciton for subtracting a constant from a vector.
template <class T> inline void Vec3<T>::operator-=(T Right)
{
	X -= Right;
	Y -= Right;
	Z -= Right;
}

//Template function for subtracting one vector from another.
template <class T> inline void Vec3<T>::operator-=(Vec3<T> Right)
{
	X -= Right.X;
	Y -= Right.Y;
	Z -= Right.Z;
	PI = 3.1415926535;
}

//Template function for creating empty vector.
template <class T> inline Vec3<T>::Vec3()
{
	PI = 3.1415926535;
	X=0;
	Y=0;
	Z=0;
}

//Template function for creating vector with starting values.
template <class T> inline Vec3<T>::Vec3(T x,T y, T z)
{
	X=x;
	Y=y;
	Z=z;
}

/*********** Extra vector operations **********/
//Template funciton to return the dot product of two vectors.
template <class T> inline float Dot(Vec3<T> Left, Vec3<T> Right)
{
	float dot;
	dot = (Left.X * Right.X) + (Left.Y * Right.Y) + (Left.Z * Right.Z);
	return dot;
}

//Template function to return the cross product of two vectors.
template <class T> inline Vec3<T> Cross(Vec3<T> Left, Vec3<T> Right)
{
	Vec3<T> RetVal;
	RetVal.X = (Left.Y * Right.Z) - (Right.Y * Left.Z);
	RetVal.Y = (Left.Z * Right.X) - (Right.Z * Left.X);
	RetVal.Z = (Left.X * Right.Y) - (Right.X * Left.Y);
	return RetVal;
}

//Template funciton to return the angle in RADIANS between two vectors.
template <class T> inline float AngleRad(Vec3<T> Left, Vec3<T> Right)
{
	float dot, angle;
	Left.MakeNormal();
	Right.MakeNormal();
	dot = Dot(Left,Right);
	angle = acos(dot);
	return angle;
}

//Template funciton to return the angle in DEGREES between two vectors.
template <class T> inline float AngleDeg(Vec3<T> Left, Vec3<T> Right)
{
        static const float PI = 3.1415926535;
	float dot, angle;
	Left.MakeNormal();
	Right.MakeNormal();
	dot = Dot(Left,Right);
	angle = acos(dot) /180 * PI;
	return angle;
}

//Template function to return a vector representing the axis of rotation of the two inputs.
template <class T> inline Vec3<T> Axis(Vec3<T> Left, Vec3<T> Right)
{
	Vec3<T> axis;
	axis = Cross(Left,Right).Normalise();
	return axis;
}

//Template function to return the triple product vector of 3 vectors.
template <class T> inline Vec3<T> TripleProduct(Vec3<T> A, Vec3<T> B, Vec3<T> C)
{
	Vec3<T> RetVal;
	RetVal = -A*Dot(B,C)+B*Dot(A,C);
	return RetVal;
}

//Template function to return normalised direction vector from one vector to another.
template <class T> inline Vec3<T> GetDirection(Vec3<T> A, Vec3<T> B)
{
	Vec3<T> RetVal;
	RetVal = A - B;
	return RetVal.MakeNormal();
}

//Template function to return distance between two vectors.
template <class T> inline Vec3<T> GetDistance(Vec3<T> A, Vec3<T> B)
{
	Vec3<T> RetVal;
	RetVal = A - B;
	return RetVal.Length();
}

//Template funciton to find if two vectors are perpendicular to each other.
template <class T> inline bool ArePerp(Vec3<T> A, Vec3<T> B)
{
	if(Dot(A,B) = 0){return true;}	//If the dot product = 0 
	else{return false;}				//then cos(theta) = 0 therefore theta = 90 -> perpendicular
}
#endif