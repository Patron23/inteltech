#pragma once
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")										// Разобраться с этим позже
#define MY_PORT 665


int main(int argc, char *argv[])
{
	// Переменные
	char buffer[1024];
	int winSockInit;
	WSADATA wsaData;

	// Инициализация
	winSockInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (winSockInit != 0)
	{
		std::cout << "WSAStartup failed: " << winSockInit;
		return -1;
	}
	else
	{
		std::cout << "WSAStartup end with: " << winSockInit;
	}


	/*WORD sockVer;
	WSADATA wsaData;
	int retVal;*/

	// Создание сокета
	SOCKET mySocket;
	if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// Ошибка. Вызываем код ошибки.
		std::cout
			<< "\n Error (creating socket). Last error: "
			<< WSAGetLastError();
		WSACleanup();

		return -1;
	}

	// Связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	local_addr.sin_addr.s_addr = 0;

	if (bind(mySocket, (sockaddr*)&local_addr, sizeof(local_addr)))
	{
		std::cout << "Error bind: " << WSAGetLastError();
		closesocket(mySocket);
		WSACleanup();
		return -2;
	}

	// Ожидаем подключения

	if (listen(mySocket, 0x100))
	{
		std::cout << "Error listen: " << WSAGetLastError();
		closesocket(mySocket);
		WSACleanup();
		return -3;
	}

	std::cout << "\nWaiting for connection...";

	// Извлекаем сообщение
	SOCKET clientSocket;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);

	while ((clientSocket = accept(mySocket, (sockaddr*)&client_addr, &client_addr_size)))
	{
		// Получаем имя хоста
		HOSTENT *hst;
		hst = gethostbyaddr((char *)
			&client_addr.sin_addr.s_addr, 4, AF_INET);
		// Выводим данные о клиенте
		std::cout
			<< "\n new connect! \n"
			<< (hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr);

		SOCKET mySock;
		mySock = ((SOCKET*)clientSocket)[0];
		char buff[20 * 1024];
		send(mySock, "hello there!", sizeof("hello there!"), 0);

		int bytes_recv;
		while ((bytes_recv =
			recv(mySock, &buff[0], sizeof(buff), 0))
			&& bytes_recv != SOCKET_ERROR)
		{
			send(mySock, &buff[0], bytes_recv, 0);
		}


		std::cout << "\n disconnect!";
		closesocket(mySock);
		return 0;
	}


	return NULL;
}