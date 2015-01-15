// Name: Randy Truong
// Quarter, Year: Fall, 2014
// Lab: 022
//
// This file is to be modified by the student.
// triangles.h
////////////////////////////////////////////////////////////
#ifndef __TRIANGLE3D_H__
#define __TRIANGLE3D_H__

struct Triangle3D
{
	//The index of the points stored in the pts_3D vector.
	int p1;
	int p2;
	int p3;

	Triangle3D() : p1(0), p2(0), p3(0) {}
  	Triangle3D(const int & newP1, const int & newP2, const int & newP3) : p1(newP1), p2(newP2), p3(newP3) {}
};

#endif