/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Большаков Роман <rombolshak@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "RServer.h"
#include "RException.h"
/* Порт на котором сервер будет принимать соединения */
#define LPORT 4567
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
namespace RSQL
{
	void client_thread( int * sock ) {
		send( *sock, "Welcome!\n", 9, 0 );
		char *buf = new char[32768];
		while (true)
		{
			read(*sock, buf, 32767);
			string query(buf);		
		}
		//close( *sock );
		//delete sock;
	}
void RServer::Start()
{
	int lsock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if (lsock <= 0) throw RServerException(SocketFail);
	
	struct sockaddr_in addr;
	addr.sin_port        = htons( LPORT );
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_family      = AF_INET;
	
	
	int stat;
	stat = bind(lsock, (struct sockaddr*)&addr, sizeof(addr));
	if (stat != 0) throw RServerException(BindFail);
	
	stat = listen( lsock, 3 );
	if (stat != 0) throw RServerException(ListenFail);
	
	
	int tmp_sock = 0;
	pthread_t tid;
	
	do {
		tmp_sock = accept( lsock, NULL, NULL );
		if (tmp_sock > 0) {
			int * client_socket = new int(tmp_sock);
			pthread_create( &tid, NULL, (void *(*)(void*))&client_thread, (void*)client_socket );
		}
		sleep( 1 );
	} while( true );
}

}
