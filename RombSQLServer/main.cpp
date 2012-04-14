/*
 * Author: root
 * Web site: www.it.chenet.info
 * Title: Multithread TCP server
 * Platform: *NIX
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iosfwd>
#include <ios>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;
/* Порт на котором сервер будет принимать соединения */
#define LPORT 4567

void client_thread( int * sock ) {
	char welcome_message[] = "Hello World!\nFrom it.chenet.info\n\n";
	send( *sock, welcome_message, strlen(welcome_message), 0 );
	close( *sock );
	delete sock;
}


int main( void ) {
	
	return 0;
	
	int lsock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if (lsock > 0) {
		printf( "Сокет успешно создан\n");
	} else {
		perror( "Неудалось создать сокет" );
		return -1;
	}
	
	struct sockaddr_in addr;
	addr.sin_port        = htons( LPORT );
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_family      = AF_INET;
	
	
	int stat;
	stat = bind(lsock, (struct sockaddr*)&addr, sizeof(addr));
	if (stat != 0) {
		perror( "Неудалось связать сокет с локальным адресом" );
		return -1;
	}
	
	
	stat = listen( lsock, 3 );
	if (stat != 0) {
		perror( "Ошибка при вызове listen()" );
		return -1;
	}
	
	
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