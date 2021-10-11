#pragma once

class Session
{
private:
	int id;//������������� ������
	string name;
	queue<Message> messages;//������� ���������
	clock_t  time;// ��������� ����� ���������� �������
	CRITICAL_SECTION  cs;// ����������� ������ ��� ������ ������� �� �������������� �������������
public:
	
	Session()//����������� �� ��������
	{
		InitializeCriticalSection(&cs);
	}

	Session(int ID, clock_t t)// ����������� � �����������
		:id(ID), time(t) 
	{
		InitializeCriticalSection(&cs);
	}

	~Session() // ����������, ������� ������ �� ������������ ������
	{
		DeleteCriticalSection(&cs);
	}
	
	int getId()//��������� �������������� ������
	{
		return id;
	}

	void setId(int i)//������������ ��������������
	{
		id = i;
	}
	
	clock_t getTime()//����� ��������� ����������
	{
		return time;
	}
	
	void setTime(clock_t t) //������������ ������� ��������� ����������
	{
		time = t;
	}

	void add(Message& m)//��������� ����������� � �������
	{
		EnterCriticalSection(&cs);
		messages.push(m);
        LeaveCriticalSection(&cs);
	}

	void send(CSocket& s)//��������� ������������ 
	{
		EnterCriticalSection(&cs);
		if (messages.empty())
		{
			Message::send(s, id, MIDDLEWARE, MT_NODATA);
		}
		else
		{
			messages.front().send(s);//front() - ������ �� ������ � ���������� �������
			messages.pop();//���������� ��������
		}
		LeaveCriticalSection(&cs);
	}
};