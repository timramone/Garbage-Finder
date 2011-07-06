///*
// * testmain.cpp
// *
// *  Created on: 04.11.2010
// *      Author: bro_
// */
//  // cout<<"\E[31m"<<" asdfdsa"<<"\E[0m\n";
//
//#include "mydbconnection.h"
//#include <iomanip>
//
//#include <stdio.h>
//#include <termios.h>
//#include <unistd.h>
//
//#include <stdlib.h>
//#include <string.h>
//
//static struct termios stored_settings;
//
//void set_keypress(void)
//{
// struct termios new_settings;
//
// tcgetattr(0,&stored_settings);
//
// new_settings = stored_settings;
//
// /* Disable canonical mode, and set buffer size to 1 byte */
// new_settings.c_lflag &= (~ICANON);
// new_settings.c_cc[VTIME] = 0;
// new_settings.c_cc[VMIN] = 1;
//
// tcsetattr(0,TCSANOW,&new_settings);
// return;
//}
//
//void reset_keypress(void)
//{
// tcsetattr(0,TCSANOW,&stored_settings);
// return;
//}
//
//int mygetch( )
//{
//	//set_keypress();
//	//int ch = getc(stdin);
//	//reset_keypress();
//	char buf;
//	buf=cin.get();
//	cin.ignore(256, ' ');
//	return buf;
//}
//
//
//void connect(MyDBConnection& con)
//{
//	system("clear");
//	cout<<"Use defaults? [y]";
//	char DBLogin[DB_LOGIN_LEN+1], DBPassword[DB_PW_LEN+1], DBHost[DB_HOST_LEN+1], DBName[DB_NAME_LEN+1];
//	int DBPort;
//
//	if (tolower(mygetch())=='n')
//	{
//		cout<<"Input DBLogin, DBPassword, DBHost, DBName, DBPort \n";
//		cin>>DBLogin>>DBPassword>>DBHost>>DBName>>DBPort;
//	}
//	else
//	{
//		strcpy(DBLogin,"trashuser");
//		strcpy(DBPassword,"pK37z");
//		strcpy(DBHost,"192.168.253.1");
//		strcpy(DBName,"points_db");
//		DBPort=3406;
//	}
//
//	cout<<"\nTrying to connect with parameters: "<<DBLogin<<" "<<DBPassword<<" "<<DBHost<<" "<<
//			DBName<<" "<<DBPort<<endl;
//
//	if (!(con.Connect(DBLogin, DBPassword, DBHost, DBName, DBPort)&MYDB_NONCONNECT))
//		cout<<"Successful";
//	else
//		cout<<"Fail";
//
//	mygetch();
//}
//
//
//void create(MyDBConnection& con)
//{
//	system("clear");
//	cout<<"Input login and password for trash-account: ";
//	char TRLogin[TR_LOGIN_LEN+1], TRPassword[TR_PW_LEN+1];
//
//	cin>>TRLogin>>TRPassword;
//	if (!(con.myDBNewUser(TRLogin,TRPassword)&MYDB_NOTAUTH))
//		cout<<"Successful";
//	else
//		cout<<"Fail";
//
//	mygetch();
//}
//
//void auth(MyDBConnection& con)
//{
//	system("clear");
//	cout<<"Input login and password for trash-account: ";
//	char TRLogin[TR_LOGIN_LEN+1], TRPassword[TR_PW_LEN+1];
//
//	cin>>TRLogin>>TRPassword;
//	if (!(con.myDBAuthorize(TRLogin,TRPassword)&MYDB_NOTAUTH))
//		cout<<"Successful";
//	else
//		cout<<"Fail!";
//
//	mygetch();
//}
//
//void newPoint(MyDBConnection& con)
//{
//	system("clear");
//	cout<<"Input x-coordinate, y-coordinate: ";
//	POINT userPt;
//
//	cin.precision(10);
//	cin>>userPt.x>>userPt.y;
//	if (!con.myDBNewPoint(userPt))
//		cout<<"Successful";
//	else
//		cout<<"Fail!";
//
//	mygetch();
//}
//
//void acceptpt(MyDBConnection& con)
//{
//	system("clear");
//	cout<<"Input id, action: ";
//	int id, action;
//
//	cin>>id>>action;
//	if (!con.myDBUpdPoint(id,action))
//		cout<<"Successful";
//	else
//		cout<<"Fail!";
//
//	mygetch();
//}
//
//void find(MyDBConnection& con)
//{
//	system("clear");
//	cout<<"Input your x-coordinate, y-coordinate and maximal distance: ";
//	POINT userPt; double dist;
//
//	cin.precision(10);
//	cin>>userPt.x>>userPt.y>>dist;
//	int n=0;
//	MYDBPOINT* points=con.myDBFindPoints(userPt,dist,n);
//
//	if (points)
//	{
//		for(int i=0;i<n;++i)
//			cout<<"\nID: "<<points[i].ID<<"\tX="<<points[i].x<<"\tY="<<points[i].y<<"\tdistance="
//				<<points[i].dist;
//
//		delete [] points;
//	}
//	else
//		cout<<"Fail!!";
//
//	mygetch();
//}
//
//int main()
//{
//	MyDBConnection con;
//	bool menu_flag=true;
//
//
//	do
//	{
//		system("clear");
//		cout<<"*******************************************************************\n";
//		cout<<"************* THIS IS A TRASHSERVER TEST APPLICATION **************\n";
//		cout<<"*******************************************************************\n\n";
//		cout<<"            MENU:            \t\t\tStatus:\n\n";
//		cout<<"\t1. Connect to Database     \t\t"<<(!(con.myState()&MYDB_NONINIT)?"MYDB_NONINIT=0\n":
//				"\E[31mMYDB_NONINIT=1\E[0m\n");
//		cout<<"\t2. Authorize on server.    \t\t"<<(!(con.myState()&MYDB_NOTAUTH)?"MYDB_NOTAUTH=0\n":
//				"\E[31mMYDB_NOTAUTH=1\E[0m\n");
//		cout<<"\t3. Create new server-user  \t\t"<<(!(con.myState()&MYDB_NONCONNECT)?"MYDB_NONCONNECT=0\n":
//				"\E[31mMYDB_NONCONNECT=1\E[0m\n");
//		cout<<"\t4. Add new point into DB   \t\t"<<(!(con.myState()&MYDB_DBERR)?"MYDB_DBERR=0\n":
//				"\E[31mMYDB_DBERR=1\E[0m\n");
//		cout<<"\t5. Accept point info\n";
//		cout<<"\t6. Find points around another point\n";
//		cout<<"\t7. Exit\n";
//
//		switch(mygetch())
//		{
//		case '1':
//			connect(con);
//			break;
//		case '2':
//			auth(con);
//			break;
//		case '3':
//			create(con);
//			break;
//		case '4':
//			newPoint(con);
//			break;
//		case '5':
//			acceptpt(con);
//			break;
//		case '6':
//			find(con);
//			break;
//		case '7':
//			menu_flag=false;
//			break;
//		}
//	}
//	while(menu_flag);
//
//	return 0;
//}
//
//

