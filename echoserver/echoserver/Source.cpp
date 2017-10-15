#include <iostream>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

//using namespace std;

#define DEF_PORT 54000


int main(int argc[], char* argv)
{
	// Инициализация winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "\ncan't initialize winsock! Exit..." << std::endl;
		return -1;
	}
	else
	{
		std::cout << "\nInitialize winsock - ok!";
	}

	// Создаем сокет
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET)
	{
		std::cerr << "\ncan't create a socket! Exit..." << std::endl;
		return -2;
	}
	else
	{
		std::cout << "\nSocket is created!";
	}

	// Привязать сокет к IP адресу; порт.
	sockaddr_in addr;
	addr.sin_family	 = AF_INET;
	addr.sin_port = htons(DEF_PORT);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(servSock, (sockaddr*)&addr, sizeof(addr));

	// Прослушивание сокет
	listen(servSock, SOMAXCONN);

	// Ожидаем подключение
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(servSock, (sockaddr*)&client, &clientSize);
	
	char host[NI_MAXHOST];		// client name
	char service[NI_MAXSERV];	// port

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << "\nConnected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << std::endl << host << " connected on port "
			<< ntohs(client.sin_port) << std::endl;
	}


	// Закрываем прослушивание
	closesocket(servSock);


	// While loop: ок & Echo message 

	char buf[4096];

	while (true)
	{

		ZeroMemory(buf, 4096);

		// ждем инфы
		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		std::cout << "\n bytes recv = " << bytesReceived;
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "\nError in recv(). Quitting" << std::endl;
			break;															// ищем ошибку где-то тут. Тк вылезает эта ошибка.
		}
		
		if (bytesReceived == 0)
		{
			std::cout << "\nClient disconnected " << std::endl;
			break;
		}

		// отправляет эхо
		send(clientSocket, buf, bytesReceived + 1, 0);


	}
	// close
	closesocket(clientSocket);


	// shutdown
	WSACleanup();

	std::cin >> buf;		//key
	return NULL;
}