/*
 * mytrashserver.h
 *
 *  Created on: 04.11.2010
 *      Author: bro_
 */

#ifndef MYTRASHSERVER_H_
#define MYTRASHSERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "mydbconnection.h"

typedef unsigned int UINT;

#define BUFFER_SIZE 4096

const char* protoCommands[]= {
		"AUTH",
		"REG",
		"ADDPT",
		"ACCPT",
		"GETPTS",
		"GETSTATE",
		"DISC"
};
const int protoSize=sizeof(protoCommands)/sizeof(char*);

class myTrashServer
{
    int nSocket, nNewSocket, nPort, state;
    UINT clilen;
    char buffer[BUFFER_SIZE];
    sockaddr_in serv_addr, cli_addr;
    MyDBConnection con;

    bool myRead()
    {
		bzero(buffer,BUFFER_SIZE);
		state = read(nNewSocket,buffer,BUFFER_SIZE-1);
		bool f=true;
		if (state<0)
		{
			cerr<<"Error reading ";
			f=false;
		}
		int i=strlen(buffer)-2;
		if (buffer[i]=='\r'||buffer[i]=='\n')
			buffer[i]=0;
		else if (buffer[i+1]=='\r'||buffer[i+1]=='\n')
			buffer[i+1]=0;
		return f;
    }

    bool myWrite()
    {
    	state = write(nNewSocket,buffer,strlen(buffer)+1);
		if (state<0)
		{
			cerr<<"Error writting "<<buffer;
			close(nNewSocket);
			return false;
		}
		return true;
    }

    int switchFunc()
    {
    	char buf[16];
    	int i;
    	for(i=0;buffer[i]!=' '&&buffer[i];++i);

    	strncpy(buf,buffer,i);
    	buf[i]=0;

    	for(int i=0;i<protoSize;++i)
    		if (!strcmp(buf,protoCommands[i]))
    			return i+2;

    	return -1;
    }

    bool sayHello()
    {
    	bool f=true;
		do
		{
			if (myRead())
			{
				buffer[5]=0;
				if (!strcmp(buffer,"HELLO"))
				{
					buffer[0]='1';
					buffer[1]=0;
					con.Connect("trashuser","pK37z", "192.168.253.1","points_db",3406);
				}
				else
				{
					buffer[0]='0';
					buffer[1]=0;
				}

				if (!myWrite())
					f=false;
			}
		}
		while(*buffer=='0');
		return f;
    }

    void auth(char* buf)
    {
    	int i=0;
    	char TRLogin[TR_LOGIN_LEN], TRPassword[TR_PW_LEN];
		for(i=0;buf[i]!=' ';++i);
		strncpy(TRLogin,buf,i);
		TRLogin[i]=0;
		buf+=i+1;

		strcpy(TRPassword,buf);

		*buffer=!(con.myDBAuthorize(TRLogin,TRPassword)&MYDB_NOTAUTH)+'0';
		buffer[1]=0;
    }

    void reg(char* buf)
    {
    	int i=0;
    	char TRLogin[TR_LOGIN_LEN], TRPassword[TR_PW_LEN];
		for(i=0;buf[i]!=' ';++i);
		strncpy(TRLogin,buf,i);
		TRLogin[i]=0;
		buf+=i+1;

		strcpy(TRPassword,buf);

		*buffer=!(con.myDBNewUser(TRLogin,TRPassword)&MYDB_NOTAUTH)+'0';
		buffer[1]=0;
    }

    void addpt(char* buf)
    {
    	int i=0;
    	POINT userPt; char fastbuf[1024];
		for(i=0;buf[i]!=' ';++i);
		strncpy(fastbuf,buf,i);
		fastbuf[i]=0;
		userPt.x=atof(fastbuf);

		buf+=i+1;

		strcpy(fastbuf,buf);
		userPt.y=atof(fastbuf);

		*buffer=!(con.myDBNewPoint(userPt)&(MYDB_NOTAUTH|MYDB_DBERR))+'0';
		buffer[1]=0;
    }

    void accpt(char* buf)
    {
    	int i=0;
		int id, action;
		char fastbuf[1024];

		for(i=0;buf[i]!=' ';++i);
		strncpy(fastbuf,buf,i);
		fastbuf[i]=0;
		id=atoi(fastbuf);

		buf+=i+1;

		strcpy(fastbuf,buf);
		action=atoi(fastbuf);

		*buffer=!(con.myDBUpdPoint(id,action)&(MYDB_NOTAUTH|MYDB_DBERR))+'0';
		buffer[1]=0;
    }

	void getpts(char* buf)
	{
		int i=0;
		double dist;
		POINT userPt; char fastbuf[1024];

		for(i=0;buf[i]!=' ';++i);
		strncpy(fastbuf,buf,i);
		fastbuf[i]=0;
		userPt.x=atof(fastbuf);

		buf+=i+1;
		for(i=0;buf[i]!=' ';++i);
		strncpy(fastbuf,buf,i);
		fastbuf[i]=0;
		userPt.y=atof(fastbuf);

		buf+=i+1;
		strcpy(fastbuf,buf);
		dist=atof(fastbuf);

		int n;n=0;
		MYDBPOINT* points;
		points=con.myDBFindPoints(userPt,dist,n);

		buf=buffer;
		int sumLen;
		sumLen=sprintf(buf,"%d ",n);
		if (!n)
			return;

		for (i=0; i<n;++i)
		{
			buf+=strlen(buf);
			int rowLen;
			rowLen=sprintf(fastbuf,"%d %.10f %.10f %f %.5f\n",
					points[i].ID, points[i].x, points[i].y, points[i].status, points[i].dist);

			if (rowLen+sumLen>BUFFER_SIZE-4)
				break;

			sumLen+=rowLen;
			strcat(buf,fastbuf);
		}

		delete [] points;
	}

public:

    myTrashServer()
    {
    	nSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (nSocket < 0)
           exit(0);
        bzero((char *) &serv_addr, sizeof(serv_addr));
        nPort = 13370;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(nPort);
        if (bind(nSocket, (sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0)
        	exit(0);
        listen(nSocket,5);
    }


    void accept()
    {
    	clilen = sizeof(cli_addr);

    	while(1)
    	{
			nNewSocket = ::accept(nSocket, (sockaddr *) &cli_addr, &clilen);
			if (nNewSocket < 0)
			  exit(0);

			pid_t pid = fork();
			if (pid == 0)
			{
				close(nSocket);
				bool flag=sayHello();

				while(flag&&(flag=myRead()))
				{
					int k=switchFunc();
					char* buf=buffer+strlen(protoCommands[k-2])+1;
					switch(k)
					{
					case 2:
						auth(buf);
						break;
					case 3:
						reg(buf);
						break;
					case 4:
						addpt(buf);
						break;
					case 5:
						accpt(buf);
						break;
					case 6:
						getpts(buf);
						break;
					case 7:
						sprintf(buffer,"%u ",con.myState());
						break;
					case 8:
						strcpy(buffer, "BYE!");
						flag=false;
						break;
					case -1:
						strcpy(buffer, "Nothing to do");
						break;
					}

					flag=myWrite();
				}

				const char* block=con.getLogin();
				block=block?block:"NOT AUTH";
				cout<<"Client "<<block<<" was disconnected\n";

				exit(0);
			}
			else
			{
				close(nNewSocket);
			}
    	}
    	close(nSocket);
    }
};

#endif /* MYTRASHSERVER_H_ */
