/*
 * mydbconnection.cpp
 *
 *  Created on: 02.11.2010
 *      Author: bro_
 */

#include "mydbconnection.h"

bool MyDBConnection::connect(char* _szDBLogin, char* _szDBPassword, char* _szDBHost, char* _szDBName,
		int _nDBPort)
{
	try
	{
		myDBConnection.connect( _szDBName, _szDBHost, _szDBLogin,  _szDBPassword, _nDBPort);
	}
	catch(const mysqlpp::Exception &ex)
	{
		cerr<<"My DB error connection error: "<<ex.what()<<endl;
		flags |= MYDB_NONCONNECT|MYDB_DBERR;
		return false;
	}
	return true;
}

	MyDBConnection::MyDBConnection(char* _szDBLogin, char* _szDBPassword, char* _szDBHost, char* _szDBName,
		int _nDBPort)
{
	Connect(_szDBLogin,  _szDBPassword, _szDBHost,  _szDBName,  _nDBPort);
}


UINT MyDBConnection::Connect(char* _szDBLogin, char* _szDBPassword, char* _szDBHost, char* _szDBName,
		int _nDBPort)
{
	flags=MYDB_NOTAUTH;

	if (connect(_szDBLogin,  _szDBPassword, _szDBHost,  _szDBName,  _nDBPort))
	{
		strcpy(szDBLogin,_szDBLogin);
		strcpy(szDBPassword,_szDBPassword);
		strcpy(szDBHost,_szDBHost);
		strcpy(szDBName,_szDBName);
		nDBPort=_nDBPort;
	}
	else
		flags|=MYDB_NONCONNECT|MYDB_NONINIT;

	return flags;
}

UINT MyDBConnection::myDBAuthorize(char* _szTrashLogin, char* _szTrashPassword)
{
	flags |=MYDB_NOTAUTH;
	// if client was disconnected by timeout
	if (!myDBConnection.connected())
		if (!connect(szDBLogin,  szDBPassword, szDBHost,  szDBName,  nDBPort))
			return flags;



	mysqlpp::Query myAuthQuery(&myDBConnection);
	try
	{
		myAuthQuery<<"SELECT id FROM users WHERE login="<<mysqlpp::quote_only
			<<_szTrashLogin<<"AND password="<<mysqlpp::quote_only<<_szTrashPassword;


		mysqlpp::UseQueryResult result = myAuthQuery.use();
		mysqlpp::Row row= result.fetch_row();

		if (!row.empty())
		{
			flags &= ~MYDB_NOTAUTH;
			strcpy(szTrashLogin,_szTrashLogin);
			strcpy(szTrashPassword,_szTrashPassword);
		}

	}
	catch(const mysqlpp::Exception &ex)
	{
		cerr<<"My DB query error: "<<ex.what()<<endl;
		flags |= MYDB_DBERR;
		return flags;
	}

	return flags;
}

MYDBPOINT* MyDBConnection::myDBFindPoints(POINT userPt, double distance, int& n)
{
	MYDBPOINT* places = 0;

	if (!flags)
	{
		// if client was disconnected by timeout
		if (!myDBConnection.connected())
			if (!connect(szDBLogin,  szDBPassword, szDBHost,  szDBName,  nDBPort))
				return 0;

		mysqlpp::Query myFindQuery(&myDBConnection);
		try
		{
			myFindQuery.precision(10);
			myFindQuery<<"SELECT ID, xCrd, yCrd, status, SQRT(POW(xCrd-"<<userPt.x<<
					",2)*111*111+POW(yCrd-"<<userPt.y<<
					",2)*78*78) AS dist FROM points HAVING dist<"<<distance<<" ORDER BY dist";

		    mysqlpp::StoreQueryResult result = myFindQuery.store();

		    n=result.num_rows();
		    if (n)
		    {
		    	places = new MYDBPOINT[n];
		    	for( int i = 0; i < n; ++i )
		    	{
		    		places[i].ID=result[i][0];
		    		places[i].x=result[i][1];
		    		places[i].y=result[i][2];
		    		places[i].status=result[i][3];
		    		places[i].dist=result[i][4];
		    	}
		    }
		    else
		    	places = 0;

		}
		catch(const mysqlpp::Exception &ex)
		{
			cerr<<"My DB query error: "<<ex.what()<<endl;
			flags |= MYDB_DBERR;
			return 0;
		}

		return places;
	}
	else
		cerr<<"Need good state for doing this operation";

	return 0;
}

UINT MyDBConnection::myDBUpdPoint(int nPointID, int nAction)
{
	if (!flags)
	{
		// if client was disconnected by timeout
		if (!myDBConnection.connected())
			if (!connect(szDBLogin,  szDBPassword, szDBHost,  szDBName,  nDBPort))
				return flags;

		mysqlpp::Query myUpdQuery(&myDBConnection);
		try
		{
			char pattern[]="INSERT logging SET user=\'%s\', point_ID=%d, action=%d";
			char buf[sizeof(pattern)/sizeof(char)+TR_LOGIN_LEN+22];
			sprintf(buf,pattern,szTrashLogin,
					nPointID,nAction);

			myUpdQuery.exec(buf);

		}
		catch(const mysqlpp::Exception &ex)
		{
			cerr<<"My DB query error: "<<ex.what()<<endl;
			return flags |= MYDB_DBERR;
		}
	}
	else
		cerr<<"Need good state for doing this operation";

	return flags;
}

UINT MyDBConnection::myDBNewPoint(POINT userPt)
{
	if (!flags)
	{
		// if client was disconnected by timeout
		if (!myDBConnection.connected())
			if (!connect(szDBLogin,  szDBPassword, szDBHost,  szDBName,  nDBPort))
				return flags;

		mysqlpp::Query myNewPtQuery(&myDBConnection);
		try
		{
			char pattern[]="INSERT points SET xCrd=%.10f, yCrd=%.10f, status=(SELECT rating FROM users WHERE login='%s')";
			char buf[sizeof(pattern)/sizeof(char)+22+TR_LOGIN_LEN];
			sprintf(buf,pattern,userPt.x,userPt.y, szTrashLogin);

			myNewPtQuery.exec(buf);
		}
		catch(const mysqlpp::Exception &ex)
		{
			cerr<<"My DB query error: "<<ex.what()<<endl;
			return flags |= MYDB_DBERR;
		}
	}
	else
		cerr<<"Need good state for doing this operation";

	return flags;
}

UINT MyDBConnection::myDBNewUser(char* _szTrashLogin, char* _szTrashPassword)
{
	flags |= MYDB_NOTAUTH;
	// if client was disconnected by timeout
	if (!myDBConnection.connected())
		if (!connect(szDBLogin,  szDBPassword, szDBHost,  szDBName,  nDBPort))
			return flags;

	mysqlpp::Query myNewUserQuery(&myDBConnection);
	try
	{
		myNewUserQuery<<"SELECT id FROM users WHERE login="<<mysqlpp::quote_only
			<<_szTrashLogin;

		mysqlpp::UseQueryResult res = myNewUserQuery.use();
		mysqlpp::Row row= res.fetch_row();

		if (row.empty())
		{
			char pattern[]="INSERT users SET login='%s', password='%s'";
			char buf[sizeof(pattern)/sizeof(char)+22+TR_LOGIN_LEN];
			sprintf(buf,pattern,_szTrashLogin,_szTrashPassword);

			mysqlpp::SimpleResult result=myNewUserQuery.execute(buf);
			if (result.rows())
				myDBAuthorize(_szTrashLogin, _szTrashPassword);
		}
		else
			cerr<<"\n*****USER_ALREADY_EXIST!!***** ";

	}
	catch(const mysqlpp::Exception &ex)
	{
		cerr<<"My DB query error: "<<ex.what()<<endl;
		return flags |= MYDB_DBERR;
	}

	return flags;
}
