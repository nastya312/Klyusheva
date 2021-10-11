// SocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SocketServer.h"
#include "Message.h"
#include "Session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int maxClientID = 0;
map<int, shared_ptr<Session>> sessions;

void TimeOut()// отключение неактивных пользователей (30 сек)
{
    while (true)
    {
        for (auto i = sessions.begin(); i != sessions.end();) {
            if (sessions.find(i->first) != sessions.end()) {
                if (double(clock() - i->second->getTime()) > 30000) {
                    cout << "Client " << i->first << " has been disconnected" << endl;
                    i = sessions.erase(i);
                }
                else
                    i++;
            }
        }
        Sleep(1000);
    }
}


int createID() //создние id из времени
{
    time_t rawtime = time(NULL);
    struct tm timeinfo;
    char buffer[1024];
    localtime_s(&timeinfo, &rawtime);
    asctime_s(buffer, &timeinfo);

    int id = timeinfo.tm_hour * pow(10, 4) +
        timeinfo.tm_min * pow(10, 2) +
        timeinfo.tm_sec;

    return id;
}


void ProcessClient(SOCKET hSOCK) //работа с клиентом
{
    CSocket s;
    //присоединение сокета
    s.Attach(hSOCK);
    Message m;

    switch (m.receive(s))
    {
    case MT_INIT://регистрация клиента
    {
        int ID = createID();
        sessions[ID] = make_shared<Session>(ID, clock());
        cout << "Client " << ID << " connect\n";
        Message::send(s, ID, MIDDLEWARE, MT_CONFIRM);
        break;
    }
    case MT_EXIT://завершение сессии 
    {
        sessions.erase(m.getheader().send_from);
        cout << "Client " << m.getheader().send_from << " disconnect\n";
        Message::send(s, m.getheader().send_from, MIDDLEWARE, MT_CONFIRM);
        break;
    }
    case MT_GETDATA: //запрос сообщения
    {
        if (sessions.find(m.getheader().send_from) != sessions.end())
        {
            sessions[m.getheader().send_from]->send(s);
            sessions[m.getheader().send_from]->setTime(clock());
        }
        break;
    }
    default: //отправка
    {
        if (sessions.find(m.getheader().send_from) != sessions.end())
        {
            if (sessions.find(m.getheader().send_to) != sessions.end())
            {
                sessions[m.getheader().send_to]->add(m);
            }
            else if (m.getheader().send_to == BROADCAST)
            {
                for (auto i : sessions) 
                {
                    if (i.first != m.getheader().send_from)
                        i.second->add(m);
                }

            }
            Message::send(s, m.getheader().send_from, MIDDLEWARE, MT_CONFIRM);
            sessions[m.getheader().send_from]->setTime(clock());
        }
        break;
    }
    }
}


void Server()
{
	AfxSocketInit();

	CSocket Server;
	Server.Create(12345);

    thread thr(TimeOut);
    thr.detach();

	while (true)
	{
        Server.Listen();
		CSocket s;
		Server.Accept(s);
		thread t(ProcessClient, s.Detach());
		t.detach();
	}
}

CWinApp theApp;

int main()
{
    std::cout << "Server is starting ...\n" << endl;

    std::cout << "Server is listening ...\n" << endl;

    createID();
    int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: code your application's behavior here.
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			Server();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}