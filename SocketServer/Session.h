#pragma once

class Session
{
private:
	int id;//идентификатор сессии
	string name;
	queue<Message> messages;//очередь сообщений
	clock_t  time;// ѕоследнее врем€ активности клиента
	CRITICAL_SECTION  cs;//  ритическа€ секци€ дл€ защиты очереди от одновременного использовани€
public:
	
	Session()//конструткор по умолчаню
	{
		InitializeCriticalSection(&cs);
	}

	Session(int ID, clock_t t)//  онструктор с параметрами
		:id(ID), time(t) 
	{
		InitializeCriticalSection(&cs);
	}

	~Session() // деструктор, который следит за уничтожением сессии
	{
		DeleteCriticalSection(&cs);
	}
	
	int getId()//получение идентификатора сессии
	{
		return id;
	}

	void setId(int i)//установление идентификатора
	{
		id = i;
	}
	
	clock_t getTime()//врем€ последней активности
	{
		return time;
	}
	
	void setTime(clock_t t) //установление времени послденей активности
	{
		time = t;
	}

	void add(Message& m)//сообщени€ добавл€ютс€ в очередь
	{
		EnterCriticalSection(&cs);
		messages.push(m);
        LeaveCriticalSection(&cs);
	}

	void send(CSocket& s)//сообщени€ отправл€ютс€ 
	{
		EnterCriticalSection(&cs);
		if (messages.empty())
		{
			Message::send(s, id, MIDDLEWARE, MT_NODATA);
		}
		else
		{
			messages.front().send(s);//front() - ссылка на первый в контейнере элемент
			messages.pop();//извлечение элемента
		}
		LeaveCriticalSection(&cs);
	}
};