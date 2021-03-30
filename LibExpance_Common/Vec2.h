#ifndef Vec2_H
#define Vec2_H
//Two dimensional Vectors
#include <math.h>
template <class T> class Vec2
{
public:
	Vec2();
	Vec2(T,T);
	Vec2 GetDirection(Vec2 Dest);
	T GetDistance(Vec2 Dest);
	bool IsPerp(Vec2);
	
	Vec2 operator*(T Right);
	Vec2 operator*(Vec2 Right);
	void operator*=(T Right);
	void operator*=(Vec2 Right);
	Vec2 operator/(T Right);
	Vec2 operator/(Vec2 Right);
	void operator/=(T Right);
	void operator/=(Vec2 Right);
	Vec2 operator+(T Right);
	Vec2 operator+(Vec2 Right);
	void operator+=(T Right);
	void operator+=(Vec2 Right);
	Vec2 operator-(T Right);
	Vec2 operator-(Vec2 Right);
	void operator-=(T Right);
	void operator-=(Vec2 Right);
	
	T Length();
	void MakeNormal();
	Vec2 Normalise();
	T X;
	T Y;
private:
    float PI;
};

//Extra vector operations
template <class T> float Dot(Vec2<T> A,Vec2<T> B);
template <class T> T Cross(Vec2<T> A,Vec2<T> B);
template <class T> float AngleRad(Vec2<T> A,Vec2<T> B);
template <class T> float AngleDeg(Vec2<T> A,Vec2<T> B);
template <class T> Vec2<T> GetDirection(Vec2<T> Origin,Vec2<T> Dest);
template <class T> T GetDistance(Vec2<T> Origin,Vec2<T> Dest);
template <class T> bool ArePerp(Vec2<T> A, Vec2<T> B);

//Template function to get destination co-ords from vector.
template <class T> inline T Vec2<T>::GetDistance(Vec2<T> Destination)
{
	Vec2<T> RetVal;
	RetVal.X = Destination.X - X;
	RetVal.Y = Destination.Y - Y;
	return RetVal.Length();
}

//Template function to get direction vector.
template <class T> inline Vec2<T> Vec2<T>::GetDirection(Vec2<T> Direction)
{
	Vec2<T> RetVal;
	RetVal.X = Direction.X - X;
	RetVal.Y = Direction.Y - Y;
	return RetVal.MakeNormal();
}

//Template function for getting if a vector is perpendicular to this one
template <class T> inline bool Vec2<T>::IsPerp(Vec2<T> B)
{
	float dot;
	dot = (X * B.X) + (Y * B.Y);//Dot product
	
	if(dot = 0){return true;}	//If the dot product = 0 
	else{return false;}			//then cos(theta) = 0 therefore theta = 90 -> perpendicular
}

//Template function for getting length of a vector.
template <class T> inline T Vec2<T>::Length()
{
	T Sum = X*X + Y*X;
	T RetVal = sqrt(Sum);
	return RetVal;
}

//Template function to return normalised vector.
template <class T> inline Vec2<T> Vec2<T>::Normalise()
{
	Vec2 Norm;
	T Scale;
	Scale = Length();
	Norm.X = X / Scale;
	Norm.Y = Y / Scale;
	return Norm;
}

//Template funciton to turn vector into normalised version.
template <class T> inline void Vec2<T>::MakeNormal()
{
	T Scale;
	Scale = Length();
	X /= Scale;
	Y /= Scale;
}

//Template funciton for multiplying vector by constant.
template <class T> inline Vec2<T> Vec2<T>::operator*(T Right)
{
	Vec2<T> RetVal;
	RetVal.X = X * Right;
	RetVal.Y = Y * Right;
	return RetVal;
}

//Template function for multiplying one vector by another.
template <class T> inline Vec2<T> Vec2<T>::operator*(Vec2<T> Right)
{
	Vec2<T> RetVal;
	RetVal.X = X * Right.X;
	RetVal.Y = Y * Right.Y;
	return RetVal;
}

//Templace function for multiplaying vector by constant.
template <class T> inline void Vec2<T>::operator*=(T Right)
{
	X *= Right;
	Y *= Right;
}

//Template function for multiplying one vector by another.
template <class T> inline void Vec2<T>::operator*=(Vec2<T> Right)
{
	X *= Right.X;
	Y *= Right.Y;
}

//Template funciton for dividing a vector by a constant.
template <class T> inline Vec2<T> Vec2<T>::operator/(T Right)
{
	Vec2<T> RetVal;
	RetVal.X = X / Right;
	RetVal.Y = Y / Right;
	return RetVal;
}

//Template function for dividing one vector by another.
template <class T> inline Vec2<T> Vec2<T>::operator/(Vec2<T> Right)
{
	Vec2<T> RetVal;
	RetVal.X = X / Right.X;
	RetVal.Y = Y / Right.Y;
	return RetVal;
}

//Template funciton for dividing a vector by a constant.
template <class T> inline void Vec2<T>::operator/=(T Right)
{
	X /= Right;
	Y /= Right;
}

//Template function for dividing one vector by another.
template <class T> inline void Vec2<T>::operator/=(Vec2<T> Right)
{
	X /= Right.X;
	Y /= Right.Y;
}

//Template funciton for adding a constant to a vector.
template <class T> inline Vec2<T> Vec2<T>::operator+(T Right)
{
	Vec2<T> RetVal;
	RetVal.X = X + Right;
	RetVal.Y = Y + Right;
	return RetVal;
}

//Template function for adding one vector to another.
template <class T> inline Vec2<T> Vec2<T>::operator+(Vec2<T> Right)
{
	Vec2<T> RetVal;
	RetVal.X = X + Right.X;
	RetVal.Y = Y + Right.Y;
	return RetVal;
}

//Template funciton for adding a constant to a vector.
template <class T> inline void Vec2<T>::operator+=(T Right)
{
	X += Right;
	Y += Right;
}

//Template function for adding one vector to another.
template <class T> inline void Vec2<T>::operator+=(Vec2<T> Right)
{
	X += Right.X;
	Y += Right.Y;
}

//Template funciton for subtracting a constant from a vector.
template <class T> inline Vec2<T> Vec2<T>::operator-(T Right)
{
	Vec2<T> RetVal;
	RetVal.X = X - Right;
	RetVal.Y = Y - Right;
	return RetVal;
}

//Template function for subtracting one vector from another.
template <class T> inline Vec2<T> Vec2<T>::operator-(Vec2<T> Right)
{
	Vec2<T> RetVal;
	RetVal.X = X - Right.X;
	RetVal.Y = Y - Right.Y;
	return RetVal;
}


//Template funciton for subtracting a constant from a vector.
template <class T> inline void Vec2<T>::operator-=(T Right)
{
	X -= Right;
	Y -= Right;
}

//Template function for subtracting one vector from another.
template <class T> inline void Vec2<T>::operator-=(Vec2<T> Right)
{
	X -= Right.X;
	Y -= Right.Y;
}

//Template function for creating empty vector.
template <class T> inline Vec2<T>::Vec2()
{
	X=0;
	Y=0;
	PI = 3.1415926535;
}

//Template function for creating vector with starting values.
template <class T> inline Vec2<T>::Vec2(T x,T y)
{
	PI = 3.1415926535;
	X=x;
	Y=y;
}

/*********** Extra vector operations **********/
//Templace function to return dot product of two vectors.
template <class T> inline float Dot(Vec2<T> Left, Vec2<T> Right)
{
	float dot;
	dot = (Left.X * Right.X) + (Left.Y * Right.Y);
	return dot;
}

//Template function to return cross product of two vectors.
template <class T> inline T Cross(Vec2<T> Left, Vec2<T> Right)
{
	T Val;
	Val = (Left.X * Right.Y) - (Left.Y * Right.X);
	return Val;
}

//Template function to return angle in RADIANS between two vectors.
template <class T> inline float AngleRad(Vec2<T> Left, Vec2<T> Right)
{
	float dot, angle;
	Left.MakeNormal();
	Right.MakeNormal();
	dot = Dot(Left,Right);
	angle = acos(dot);
	return angle;
}

//Template function to return angle in DEGREES between two vectors.
template <class T> inline float AngleDeg(Vec2<T> Left, Vec2<T> Right)
{
        static const float PI = 3.1415926535;
	float dot, angle;
	Left.MakeNormal();
	Right.MakeNormal();
	dot = Dot(Left,Right);
	angle = acos(dot) / 180 * PI;
	return angle;
}

//Template function to return normalised direction vector from one vector to another.
template <class T> inline Vec2<T> GetDirection(Vec2<T> A, Vec2<T> B)
{
	Vec2<T> RetVal;
	RetVal = A - B;
	return RetVal.MakeNormal();
}

//Template function to return distance between two vectors.
template <class T> inline Vec2<T> GetDistance(Vec2<T> A, Vec2<T> B)
{
	Vec2<T> RetVal;
	RetVal = A - B;
	return RetVal.Length();
}

//Template funciton to find if two vectors are perpendicular to each other.
template <class T> inline bool ArePerp(Vec2<T> A, Vec2<T> B)
{	
	if(Dot(A,B) = 0){return true;}	//If the dot product = 0 
	else{return false;}				//then cos(theta) = 0 therefore theta = 90 -> perpendicular
}


#endif