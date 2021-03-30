#ifndef Ex_Vector
#define Ex_Vector
/************************************************\
 *  This file is used by all vector files	* 
 ************************************************
 * Created by John Bargman and Declan Buchan    *
\************************************************/

/*Note from PJB:
	It would be simpler to define a Vec4 class that all other vectors dirive from,
	each would then only provide acess to the items specific to that vector 
	eg: x and y, but not z and w
	However this would require z and w to still exist in memory. Thus being a total
	fuckwit solution that dumb people like Treyarc would use.
	
	Rather we should present a base abstract class with no functionallity at all, then 
	have the other clases inherit from this base to insure a standardised impimentation.
Note from pl0x:
	the vector classes have support for 3D and 4D vectors,
	t's x, y, and z, for location and w fortime, it is undecided if a vector type of
	5 dimensions will be added with a 5thvalue, h, for spacial compression.
Further note from PJB:
        I think you'll find that's 'Spacial elasticsy', but yes. Doctor who quotes FTW.
	*/
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

/*
//Type Defintions for legacy codebase
typedef Vec2<int> PJBVec2Di;
typedef Vec2<float> PJBVec2Df;
typedef Vec2<float> PJBVec2Dd;
typedef Vec3<int> PJBVec3Di;
typedef Vec3<float> PJBVec3Df;
typedef Vec3<float> PJBVec3Dd;
typedef Vec4<int> PJBVec4Di;
typedef Vec4<float> PJBVec4Df;
typedef Vec4<float> PJBVec4Dd;
*/
#endif