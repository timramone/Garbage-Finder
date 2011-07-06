/*
 * point.h
 *
 *  Created on: 02.11.2010
 *      Author: bro_
 */

#ifndef POINT_H_
#define POINT_H_

struct POINT
{
	double x;
	double y;
};

struct MYDBPOINT: POINT
{
	double status;
	int ID;
	double dist;
};


#endif /* POINT_H_ */
