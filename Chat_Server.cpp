#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<ctime>
#include<vector>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

const int max_len = 100;  //�������˽��ܵ������Ϣ����
//char* recvBuf = new char[max_len];  //���ջ�����
//int msg_len;  //���յ�����Ϣ����
int client_num = 0;
vector<SOCKET> clients;

//��ȡ��ǰϵͳ���ں�ʱ��
char* get_time()
{
	time_t now = time(0); // �� now ת��Ϊ�ַ�����ʽ 
	char* dt = ctime(&now);
	return dt;
}

//��ӡ��־
void print_log(char* recvBuf)
{
	recvBuf = recvBuf + 3;
	cout << "[log]" << get_time() << ">>>" << recvBuf << endl;
}

//��ͻ��˽���
DWORD WINAPI Server_Handle(LPVOID lparam)
{
	SOCKET Client_Socket = (SOCKET)(LPVOID)lparam;
	//��������
	//���û��������ӳɹ���ʾ
	string success_flag = "��ӭ����Ⱥ��";
	success_flag = success_flag + "��" + "����������û���:";
	send(Client_Socket, success_flag.c_str(), success_flag.length(), 0);  //c_str()��������ָ��

	char* recvBuf = new char[max_len];  //�½����ջ�����
	int recv_len;  //������Ϣ�ĳ���
	//���տͻ������ӳɹ�����Ϣ
	recv_len = recv(Client_Socket, recvBuf, max_len, 0);
	//���յ��ַ������ӽ��������������
	recvBuf[recv_len] = 0x00;
	//��־
	print_log(recvBuf);

	//��ȡ���û����û���
	string user;
	int start = -1, end = -1;
	for (int i = 0; i < recv_len; i++) 
	{
		if (recvBuf[i] == ' ') 
		{
			if (start == -1)
				start = i;
			else 
			{
				end = i;
				break;
			}

		}
	}
	for (int i = start + 1; i < end; i++)
	{
		user = user + recvBuf[i];
	}

	//֪ͨ�����û���user����Ⱥ��
	char* t = get_time();
	string t1 = t;
	string attention = t1 + ">�û� " + user + " ����Ⱥ��";
	for (int i = 0; i < client_num; i++) {
		if (Client_Socket != clients[i])
			send(clients[i], attention.c_str(), attention.length(), 0);
	}

	//ת���׶�,����user����Ϣ����ת����Ⱥ�ĵ������û�
	while (1)
	{
		//������Ϣ
		recv_len = recv(Client_Socket, recvBuf, max_len, 0);
		if (recv_len > 0)
		{
			recvBuf[recv_len] = 0x00;
			//�����û�����Ϣ
			string message;
			for (int i = 0; i < recv_len; i++)
				message = message + recvBuf[i];

			char op = message[1];  //Э�鲿�֣�������һ��

			//��message���д���ȥ��Э�鲿��
			message = message.substr(3, message.length() - 3);

			//�û�������Ϣ�ڷ���˵���־��¼
			//��������Ϣ
			if (op == '1')
			{
				cout << "[log]" << get_time() << "�û� " << recvBuf + 3;
				cout << endl;
			}
			//user�˳�
			else
			{
				print_log(recvBuf);
			}

			//���û�user�˳�����Ϣת���������û�
			if (op == '2')
			{
				for (int i = 0; i < client_num; i++)
				{
					if (Client_Socket != clients[i])
					{
						char* t = get_time();
						string t1 = t;
						message = t1 + ">" + message;
						send(clients[i], message.c_str(), message.length(), 0);
					}
				}
				//�رյ�ǰ�߳�
				break;
			}
			//ת����Ϣ
			else
			{
				for (int i = 0; i < client_num; i++)
					if (Client_Socket != clients[i])
					{
						char* t = get_time();
						string t1 = t;
						message = t1 + ">" + message;
						send(clients[i], message.c_str(), message.length(), 0);
					}
			}
		}
		else
			break;
	}
	return 0;
}

int main()
{
	int state1, state2, state3;  //�����������ֵ

	WORD wVersionRequested = MAKEWORD(2, 2); //������ϣ��ʹ�õ�socket����߰汾
	WSADATA wsaData;  //���õ�Socket����ϸ��Ϣ��ͨ��WSAStartup������ֵ
	//��ʼ��Socket DLL��Э��ʹ�õ�Socket�汾����ʼ���ɹ��򷵻�0������Ϊ�����int������
	state1 = WSAStartup(wVersionRequested, &wsaData);
	//�жϳ�ʼ��dll�Ƿ�ɹ�
	if (state1 == 0)
	{
		cout << "[log]" << get_time() << ">>>" << "��������ʼ��Socket DLL�ɹ�" << endl;
	}
	else
	{
		cout << "[error]" << get_time() << ">>>" << "��������ʼ��Socket DLLʧ�ܣ�" << endl;
	}

	//����һ��Socket�����󶨵�һ���ض��Ĵ�������
	//���� AF_INET��IPv4��ַ���ͣ�SOCK_STREAM����ʽ�׽��֣�IPPROTO_TCP��ʹ��TCP�����Э��
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addrSrv;  //ָ�򱾵ؽ�����ʼ��ַ��ָ�룬��߰���IP��ַ�Ͷ˿ںţ���תΪSOCKETADDR���͵�ָ�룬SOCKETADDR����SOCKETADDR_IN�ķ�װ���ѳ���ip��ַ����֮��ĳ�Ա��������װ��һ���ˣ�
	addrSrv.sin_family = AF_INET;  //IPv4��ַ����
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //���ñ���ipΪ������ip
	addrSrv.sin_port = htons(1234);   //�˿ں�Ϊ1234
	//��һ�����ص�ַ�󶨵�ָ����Socket����ȷ�򷵻�0�����򷵻�SOCKET_ERROR
	//���� sockSrv��socket�����������Ｔ���������׽��ֶ˿ڣ�namelen����ַ���ȣ�Ϊsockaddr�ṹ�ĳ���
	state2 = bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	//�жϰ󶨱��ص�ַ�Ƿ�ɹ�
	if (state2 == 0)
	{
		cout << "[log]" << get_time() << ">>>" << "�������󶨱��ص�ַ�ɹ�" << endl;
	}
	else
	{
		cout << "[error]" << get_time() << ">>>" << "�������󶨱��ص�ַʧ�ܣ�" << endl;
	}

	//ʹsocket�������״̬������Զ�������Ƿ���
	//���� socketSrv��socket��������backlog�����ӵȴ����е���󳤶ȣ�����Ϊ20
	state3 = listen(sockSrv, 20);
	if (state3 == 0)
	{
		cout << "[log]" << get_time() << ">>>" << "��ʼ����......" << endl;
	}
	else
	{
		cout << "[error]" << get_time() << ">>>" << "����ʧ�ܣ�" << endl;
	}

	while (1)
	{
		SOCKADDR_IN addrClient;   //�ͻ��˵�ַ
		int len = sizeof(SOCKADDR);   //��ַ����
		//����һ���ض�socket����ȴ������е��������󣬴˺������к��������״̬��ֱ�����������������ӳɹ����������ӵ�socket�����������󷵻�INVALID_SOCKET
		//���� sockSrv��������socket��������addrClient���ͻ��˵�ַ��ʼλ��ָ�룺len����ַ����
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		//���û������
		clients.push_back(sockConn);
		client_num++;

		//��־��Ϣ
		cout << "[log]" << get_time() << ">>>" << "�������˿ڣ�" << addrClient.sin_port << endl;

		//�����̣߳����ڴ�����Ϣ
		DWORD handleThreadId;
		HANDLE handleThread = CreateThread(NULL, NULL, Server_Handle, LPVOID(sockConn), 0, &handleThreadId);
	}

	closesocket(sockSrv);  //�رշ�����socket
	WSACleanup();  //����ʹ��Socket���ͷ�Socket DLL��Դ
	return 0;
}