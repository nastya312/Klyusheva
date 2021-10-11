#pragma once
#include "pch.h"

enum Address//перечисление адресов
{
	BROADCAST=-1,//широковещательный канал
	MIDDLEWARE=0,//посредник в виде сервера
};

enum MessageTypes
{
	MT_INIT,
	MT_EXIT,
	MT_GETDATA,
	MT_DATA,
	MT_NODATA,
	MT_CONFIRM
};

//enum MessageRecipients
//{
//	MR_BROKER = 10,
//	MR_ALL = 50,
//	MR_USER = 100
//};

struct MessageHeader
{
	int send_to; // получатель
	int send_from; // отправитель
	int message_type; // тип сообщения
	int message_size; // размер сообщения
};

class Message 
{
private:
	MessageHeader message_header = {0}; // заголовок
	string message_data = ""; // передаваемые данные 

public:
	
	Message() {}
	Message(int to, int from, int type = MT_DATA, const string& data = "")
	{
		message_header = {to, from, type, (int)data.length()};
		message_data = data;
	}
	
	MessageHeader getheader()
	{
		return message_header;
	}

	string getdata()
	{
		return message_data;
	}

	void send(CSocket& s)//отправка сообщения
	{
		s.Send(&message_header, sizeof(MessageHeader));
		// отправка данных на подключенный сокет
		// буфер с данными, размер данных 

		if (message_header.message_size)
		{
			s.Send(message_data.c_str(), message_header.message_size+1);
		}
	}

	int receive(CSocket& s)//получение сообщения
	{
		if (!s.Receive(&message_header, sizeof(MessageHeader)))
			// получение данных от сокета.
		{
			return MT_NODATA;
		}

		if (message_header.message_size)
		{
			vector <char> v(message_header.message_size+1);
			s.Receive(&v[0], (int)message_header.message_size+1);
			message_data = string(&v[0], message_header.message_size);

			// Конструктор, который создает новую строку из строки szCString 
			// с количеством символов, указанных в unLength

			//char* pBuff = new char[message_header.message_size + 1];
			//s.Receive(pBuff, message_header.message_size + 1);
			//pBuff[message_header.message_size] = '\0';
			//message_data = pBuff;
            //delete[] pBuff;//против утечек памяти
		}
		return message_header.message_type;
	}

	static void send(CSocket& s, int to, int from, int type = MT_DATA, const string& data = "") {
		Message m(to, from, type, data);
		m.send(s);
	}

};




