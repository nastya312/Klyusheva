#pragma once
#include "pch.h"

enum Address//������������ �������
{
	BROADCAST=-1,//����������������� �����
	MIDDLEWARE=0,//��������� � ���� �������
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
	int send_to; // ����������
	int send_from; // �����������
	int message_type; // ��� ���������
	int message_size; // ������ ���������
};

class Message 
{
private:
	MessageHeader message_header = {0}; // ���������
	string message_data = ""; // ������������ ������ 

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

	void send(CSocket& s)//�������� ���������
	{
		s.Send(&message_header, sizeof(MessageHeader));
		// �������� ������ �� ������������ �����
		// ����� � �������, ������ ������ 

		if (message_header.message_size)
		{
			s.Send(message_data.c_str(), message_header.message_size+1);
		}
	}

	int receive(CSocket& s)//��������� ���������
	{
		if (!s.Receive(&message_header, sizeof(MessageHeader)))
			// ��������� ������ �� ������.
		{
			return MT_NODATA;
		}

		if (message_header.message_size)
		{
			vector <char> v(message_header.message_size+1);
			s.Receive(&v[0], (int)message_header.message_size+1);
			message_data = string(&v[0], message_header.message_size);

			// �����������, ������� ������� ����� ������ �� ������ szCString 
			// � ����������� ��������, ��������� � unLength

			//char* pBuff = new char[message_header.message_size + 1];
			//s.Receive(pBuff, message_header.message_size + 1);
			//pBuff[message_header.message_size] = '\0';
			//message_data = pBuff;
            //delete[] pBuff;//������ ������ ������
		}
		return message_header.message_type;
	}

	static void send(CSocket& s, int to, int from, int type = MT_DATA, const string& data = "") {
		Message m(to, from, type, data);
		m.send(s);
	}

};




