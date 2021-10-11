#pragma once
#include "pch.h"

enum Address//перечисление адресов
{
	BROADCAST = -1,//широковещательный канал
	MIDDLEWARE = 0,//посредник в виде сервера
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

struct MessageHeader//заголовки
{
	int to;//для
	int from;//от
	int type;//тип
	int size_msg;//размер
};

class Message
{
private:
	MessageHeader header_msg;
	string data_msg;
public:

	Message()///конструктор без параметров
	{
		header_msg = { 0 };
		data_msg = "";
	}
	Message(int to, int from, int type = MT_DATA, const string& data = "")//конструктор с парамтерами
	{
		header_msg.from = from;
		header_msg.to = to;
		header_msg.type = type;
		header_msg.size_msg = data.length();
		data_msg = data;
	}

	MessageHeader getheader() //геттер заголовка
	{
		return header_msg;
	}
	string getdata()//геттер сообщения
	{
		return data_msg;
	}

	void send(CSocket& s)//отправка сообщения
	{
		s.Send(&header_msg, sizeof(MessageHeader));
		if (header_msg.size_msg != 0)//отправка сообщения, если оно не постое
		{
			s.Send(data_msg.c_str(), header_msg.size_msg + 1);
		}
	}

	int receive(CSocket& s)//получение сообщения
	{
		s.Receive(&header_msg, sizeof(MessageHeader));
		if (header_msg.size_msg != 0)
		{
			char* pBuff = new char[header_msg.size_msg + 1];
			s.Receive(pBuff, header_msg.size_msg + 1);
			pBuff[header_msg.size_msg] = '\0';
			data_msg = pBuff;
			delete[] pBuff;//против утечек памяти
		}
		return header_msg.type;
	}

	static void SendMessage(CSocket& s, int to, int from, int type = MT_DATA, const string& data = "") {
		Message m(to, from, type, data);
		m.send(s);
	}

};
