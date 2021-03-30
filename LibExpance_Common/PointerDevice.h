/* 
 * File:   PointerDevice.h
 * Author: pjb
 *
 * Created on 27 June 2011, 05:12
 */

#ifndef POINTERDEVICE_H
#define	POINTERDEVICE_H
#include "Vec3.h"

struct PointDev
{
    Vec3<float> Pos;
    Vec3<float> Vel;
    bool Buttons[12]; //support for 12 buttons!! :-D
};

#endif	/* POINTERDEVICE_H */

