#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<ctime>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

//接收缓冲区，由于发送时要完成协议内容，所以发送消息用string定义
const int max_len = 100;
char* recvBuf = new char[max_len];
string user;  //用户名

//获取当前系统日期和时间
char* get_time()
{
	time_t now = time(0); // 把 now 转换为字符串形式 
	char* dt = ctime(&now);
	return dt;
}

//用户登录，与服务器握手
void WINAPI Client_Login(LPVOID lparam)
{
	//服务器端Socket
	SOCKET Server_Socket = (SOCKET)(LPVOID)lparam;

	//接收服务器连接成功提示
	int recv_len = recv(Server_Socket, recvBuf, max_len, 0);
	if (recv_len > 0) {
		recvBuf[recv_len] = 0x00;
		cout << recvBuf;
	}

	//输入自己的用户名，用于在聊天室显示
	getline(cin, user);

	//客户连接成功后告知服务器
	string success_flag = "(0)用户 " + user + " 连接成功";
	send(Server_Socket, success_flag.c_str(), success_flag.length(), 0);
	return;
}

//向服务器发送信息
DWORD WINAPI client_send(LPVOID lparam)
{
	SOCKET Server_Socket = (SOCKET)(LPVOID)lparam;

	//用户动作
	while (1)
	{
		cout << ">>>";
		string message;
		getline(cin, message);

		//用户退出
		if (message == "exit")
		{
			cout << "走好！" << endl;
			string end = "(2)用户 " + user + " 离开群聊";
			send(Server_Socket, end.c_str(), end.length(), 0);
			break;
		}

		//用户发消息
		message = "(1)" + user + "：" + message;
		send(Server_Socket, message.c_str(), message.length(), 0);
	}
	return 0;
}

//接收服务端转发的信息
DWORD WINAPI client_recv(LPVOID lparam)
{
	SOCKET Server_Socket = (SOCKET)(LPVOID)lparam;

	//接收转发
	while (1)
	{
		int recv_len = recv(Server_Socket, recvBuf, max_len, 0);
		if (recv_len > 0)
		{
			recvBuf[recv_len] = 0x00;
			cout << recvBuf;
		}
		cout << endl << ">>>";
	}
	return 0;
}

int main()
{
	int state1, state2;  //存各种函数的返回值

	WORD wVersionRequested = MAKEWORD(2, 2); //调用者希望使用的socket的最高版本
	WSADATA wsaData;  //可用的Socket的详细信息，通过WSAStartup函数赋值
	//初始化Socket DLL，协商使用的Socket版本；初始化成功则返回0，否则为错误的int型数字
	state1 = WSAStartup(wVersionRequested, &wsaData);
	if (state1 == 0)
		cout << "客户端初始化Socket DLL成功！" << endl;
	else
		cout << "客户端初始化Socket DLL失败！" << endl;

	//定义服务端套接字，一会儿连它
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addrSrv;  //定义服务器端地址
	addrSrv.sin_family = AF_INET;  //IPv4地址类型
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //服务器地址为本机的公网ip，这里直接用127.0.0.1标识符即可
	addrSrv.sin_port = htons(1234);   //服务器的端口号为1234

	//向一个特定的socket发出建连请求，成功返回0，否则返回SOCKET_ERROR
	//参数 服务器端socket描述符、服务器地址、地址长度
	state2 = connect(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	if (state2 == 0)
		cout << "连接服务器成功！" << endl;
	else
		cout << "连接服务器失败！" << endl;

	//用户登录，与服务器握手
	Client_Login((LPVOID)sockSrv);


	//用户创建两个线程：一个接收信息，一个发送信息
	DWORD sendThreadId;
	HANDLE sendThread = CreateThread(NULL, NULL, client_send, LPVOID(sockSrv), 0, &sendThreadId);

	DWORD recvThreadId;
	HANDLE recvThread = CreateThread(NULL, NULL, client_recv, LPVOID(sockSrv), 0, &recvThreadId);

	//一个线程结束，则都结束
	WaitForSingleObject(sendThread, INFINITE);

	//结束使用Socket，释放Socket资源
	closesocket(sockSrv);
	WSACleanup();
	return 0;
}