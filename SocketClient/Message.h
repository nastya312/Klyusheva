#pragma once
#include "pch.h"

enum Address//������������ �������
{
	BROADCAST = -1,//����������������� �����
	MIDDLEWARE = 0,//��������� � ���� �������
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

struct MessageHeader//���������
{
	int to;//���
	int from;//��
	int type;//���
	int size_msg;//������
};

class Message
{
private:
	MessageHeader header_msg;
	string data_msg;
public:

	Message()///����������� ��� ����������
	{
		header_msg = { 0 };
		data_msg = "";
	}
	Message(int to, int from, int type = MT_DATA, const string& data = "")//����������� � �����������
	{
		header_msg.from = from;
		header_msg.to = to;
		header_msg.type = type;
		header_msg.size_msg = data.length();
		data_msg = data;
	}

	MessageHeader getheader() //������ ���������
	{
		return header_msg;
	}
	string getdata()//������ ���������
	{
		return data_msg;
	}

	void send(CSocket& s)//�������� ���������
	{
		s.Send(&header_msg, sizeof(MessageHeader));
		if (header_msg.size_msg != 0)//�������� ���������, ���� ��� �� ������
		{
			s.Send(data_msg.c_str(), header_msg.size_msg + 1);
		}
	}

	int receive(CSocket& s)//��������� ���������
	{
		s.Receive(&header_msg, sizeof(MessageHeader));
		if (header_msg.size_msg != 0)
		{
			char* pBuff = new char[header_msg.size_msg + 1];
			s.Receive(pBuff, header_msg.size_msg + 1);
			pBuff[header_msg.size_msg] = '\0';
			data_msg = pBuff;
			delete[] pBuff;//������ ������ ������
		}
		return header_msg.type;
	}

	static void SendMessage(CSocket& s, int to, int from, int type = MT_DATA, const string& data = "") {
		Message m(to, from, type, data);
		m.send(s);
	}

};
