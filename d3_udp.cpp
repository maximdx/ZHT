#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <netdb.h>
#include "d3_transport.h"


int d3_svr_accept_udp( int sock ) {
	/* no need to accept connection in udp. */	
	return sock;
}

int d3_makeConnection_udp() {
	int sock;

	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( sock < 0 ) {
		printf( "Error occurred when creating the socket:%d\n", sock);
		printf( "%s\n", strerror(errno) );
		close( sock );
		return -1;
	}
	
	return sock;
}

int d3_svr_makeSocket_udp( int port ) {
	int sock;
	struct sockaddr_in fromAddr;

	memset( &fromAddr, 0, sizeof(fromAddr) );
	fromAddr.sin_family = AF_INET;
	fromAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	fromAddr.sin_port = htons( port );
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( sock < 0 ) {
		printf( "Error occurred when creating the socket:%d to the server port:%d\n", sock, port );
		printf( "%s\n", strerror(errno) );
		close( sock );
		return -1;
	}
	if( bind( sock, (struct sockaddr*)&fromAddr, sizeof(fromAddr) ) < 0 ) {
		printf( "Error occurred when binding the socket:%d to the server port:%d\n", sock, port );
		printf( "%s\n", strerror(errno) );
		close( sock );
		return -1;
	}

	return sock;
}

int d3_svr_recv_udp( int sock, void *buffer, size_t size, int flags, sockaddr_in *toAddr ) {
	unsigned int addrLen;
	int recvLen;
	char cnf[16] = "confirmed";
		
	memset( buffer, 0, size );
	memset( toAddr, 0, sizeof(sockaddr_in) );
//	printf( "buffer size: %d\n", sizeof(buffer) );
//	printf( "size argument: %d\n", size );
	addrLen = sizeof(toAddr);
	recvLen = recvfrom( sock, buffer, size, 0, (struct sockaddr*)toAddr, &addrLen) ;
	if( recvLen < 0) {
		printf( "d3_svr_recv_udp recvfrom failed, error: %s\n", strerror(errno) );
		close( sock );
		return -1;
	}
/*
	if( sendto( sock, cnf, 16, 0, (struct sockaddr*)toAddr, sizeof(sockaddr_in) ) != 16 ) {
		printf( "d3_svr_recv_udp feedback sendto failed, error: %s\n", strerror(errno) );
		close( sock );
		return -1;
	}
*/
	return recvLen;
}

struct sockaddr_in d3_make_sockaddr_in( const char *rmserver, int port ) {
	struct sockaddr_in toAddr;
	struct hostent *hinfo;

	hinfo = gethostbyname( rmserver );
	memset( &toAddr, 0, sizeof(toAddr) );
	toAddr.sin_family = AF_INET;
	toAddr.sin_addr =  *(struct in_addr *)(hinfo->h_addr);
	toAddr.sin_port = htons( port );

	return toAddr;
}

int d3_send_data_udp( int sock, void *buffer, size_t size, int flags, const char *rmserver, int port ) {
	struct sockaddr_in toAddr;
	struct sockaddr_in fromAddr;
	struct hostent *hinfo;
	char *buf;
//	char *recvbuf = (char *)malloc(size*sizeof(char));
	unsigned int fromLen;
	char cnf[16];
	
	buf = (char *) buffer;
	hinfo = gethostbyname( rmserver );
	memset( &toAddr, 0, sizeof(toAddr) );
	toAddr.sin_family = AF_INET;
	toAddr.sin_addr =  *(struct in_addr *)(hinfo->h_addr);
	toAddr.sin_port = htons( port );
	if( sendto( sock, buf, strlen(buf), 0, (struct sockaddr*)&toAddr, sizeof(toAddr) ) != strlen(buf) ) {
		printf( "d3_send_data_udp sendto failed. error:" );
		printf( "%s\n", strerror(errno) );
		close( sock );
		return -1;
	}
/*
	fromLen = sizeof(fromAddr);
	if( recvfrom( sock, cnf, 16, 0, (struct sockaddr*)&fromAddr, &fromLen ) < 0 ) {
                printf( "d3_send_data feedback recvfrom failed, error: %s\n", strerror(errno) );
		close( sock );
		return -1;
	}
//	printf( "strlen buf: %d\n", strlen(buf) );
//	printf( "size : %d\n", size );
//  	printf( "int size: %d; char size: %d; int32_t size: %d\n", sizeof(int), sizeof(char), sizeof(int32_t) );

//	free( recvbuf );
*/
	return size;
}

int d3_send_via_sockaddr_in( int sock, void *buffer, size_t size, int flags, sockaddr_in *toAddr) {
	char *buf;
	buf = (char *)buffer;
        if( sendto( sock, buf, strlen(buf), 0, (struct sockaddr*)toAddr, sizeof(sockaddr_in) ) != strlen(buf) ) {
	        printf( "d3_send_via_sockaddr_in sendto failed. error:" );
	        printf( "%s\n", strerror(errno) );
	        close( sock );
		return -1;
	}
	return size;
}

int d3_closeConnection_udp( int sock ) {
    return close( sock );
}
