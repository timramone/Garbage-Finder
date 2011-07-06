/*
 * mydbconnection.h
 *
 *
 *	This is DB-SERVER-COMMUNICATION part of Trash Geo-location application.
 *	Licensed by GPL.
 *
 *  Created on: 02.11.2010
 *      Author: bro_
 */

#ifndef MYDBCONNECTION_H_

#include <iostream>
#include <math.h>
#include <string.h>
#include <iomanip>
#include <mysql/mysql.h>
#include <mysql/mysql++.h>

#include "point.h"

using namespace std;

#define DB_LOGIN_LEN 20
#define DB_PW_LEN 33
#define DB_HOST_LEN 128
#define DB_NAME_LEN 33

#define TR_LOGIN_LEN 40
#define TR_PW_LEN 33

enum MYDBCONNECTIONFLAGS {
	MYDB_NOTAUTH=1,
	MYDB_NONCONNECT=2,
	MYDB_DBERR=4,
	MYDB_NONINIT=8
};

typedef unsigned int UINT;

class MyDBConnection
{
	// 0 - it's okay =) others values are 'binary or' combinations of MYDBCONNECTIONFLAGS
	UINT flags;

	// fields needed for connection to DB, which initialize in constructor function
	char szDBLogin[DB_LOGIN_LEN];
	char szDBPassword[DB_PW_LEN];
	char szDBHost[DB_HOST_LEN];
	char szDBName[DB_NAME_LEN];
	int nDBPort;

	// fields needed for authorized access to server features
	char szTrashLogin[TR_LOGIN_LEN];
	char szTrashPassword[TR_PW_LEN];

	// connection object MySQL++
	mysqlpp::Connection myDBConnection;

	// try to connect to server. If fail, MYDB_NONCONNECT part of 'flags' switch to 1;
	bool connect(char* _szDBLogin, char* _szDBPassword, char* _szDBHost, char* _szDBName, int _nDBPort);

public:

	operator int()
	{
		return flags;
	}

	UINT myState()
	{
		return flags;
	}

	const char* getLogin()
	{
		return (flags&(MYDB_NOTAUTH))?0:szTrashLogin;
	}

	// Constructors
	MyDBConnection()
	{
		flags = MYDB_NONINIT|MYDB_NONCONNECT|MYDB_NOTAUTH;
	}

	MyDBConnection(char* _szDBLogin, char* _szDBPassword, char* _szDBHost, char* _szDBName, int _nDBPort);

	//Connect func
	UINT Connect(char* _szDBLogin, char* _szDBPassword, char* _szDBHost, char* _szDBName, int _nDBPort);

	// simple destructor
	~MyDBConnection()
	{
		myDBConnection.disconnect();
	}

	// authorize function. if trashlogin and pw match, authorize part of variable flags set to 0
	UINT myDBAuthorize(char* _szTrashLogin, char* _szTrashPassword);

	// find points which distance to POINT userPt < distance. Function creates an array of MYDBPOINT elements,
	// where puts data of founded points. Size of this array returned to n variable.
	MYDBPOINT* myDBFindPoints(POINT userPt, double distance, int& n);

	// Function for sending confirmation about existing point. nAction = 0 - point exist, 1 - point don't exist
	UINT myDBUpdPoint(int nPointID, int nAction);

	// Function for sending information about new point from users.
	UINT myDBNewPoint(POINT userPt);

	// Add a new user in DB.
	UINT myDBNewUser(char* _szTrashLogin, char* _szTrashPassword);

};

#define MYDBCONNECTION_H_


#endif /* MYDBCONNECTION_H_ */
