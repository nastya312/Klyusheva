// SocketClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SocketClient.h"
#include "../SocketServer/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int client_id; //идентификация клиента

void connect(CSocket& cs)//устновка соединения
{
    cs.Create();
    cs.Connect(_T("127.0.0.1"), 12345); //хост и порт
}

void disconnect(CSocket& cs)//разрыв соединения
{
    cs.Close();
}


void ProcessMessages()
{
	while (true)
	{
        CSocket s;
        connect(s);
        Message m;
        Message::send(s, MIDDLEWARE, client_id, MT_GETDATA);
        if (m.receive(s) == MT_DATA)  //сообщение,если есть текст
        {
           cout << "Message from " << m.getheader().send_from << ": " << m.getdata() << endl;
        }
        disconnect(s);
        Sleep(100);
	}
}


void Client()
{
	AfxSocketInit();// Инициализация сокетов и подключения
    CSocket client;
    connect(client);
    Message::send(client, 0, 0, MT_INIT);
    Message m;
    if (m.receive(client) == MT_CONFIRM) {
       client_id = m.getheader().send_to;
        cout << "id is " << client_id << endl;
        thread t(ProcessMessages);
        t.detach(); //отсоединение объекта от потока
    }
    else {
        cout << "error" << endl;
        return;
    }
    disconnect(client);

    while (true) // Передача и прием сообщений
    {
        cout << "1. Send Message\n2. Exit\n" << endl;
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            //изначально бродкаст
            int ClientID = BROADCAST;
            cout << "1. Only for one client\n2. For All Clients\n" << endl;
            int choice2;
            cin >> choice2;
            if (choice2 == 1) {
                cout << "\nEnter ID of client\n";
                cin >> ClientID;
            }

            cout << "\nEnter your Message\n";
            string str;
            cin.ignore();
            getline(cin, str);
            connect(client);
            Message::send(client, ClientID, client_id, MT_DATA, str);
            if (m.receive(client) == MT_CONFIRM) {
                cout << "\nsucces\n" << endl;
            }
            else {
                cout << "\nerror\n" << endl;
            }
            disconnect(client);

            break;
        }

        case 2: {
            connect(client);
            Message::send(client, MIDDLEWARE, client_id, MT_EXIT);
            if (m.receive(client) == MT_CONFIRM) {
                cout << "\nsucces\n" << endl;
            }
            else {
                cout << "\nerror\n" << endl;
            }
            disconnect(client);
            return;
        }
        default:
            break;
        }
    }

}

CWinApp theApp;

int main()
{
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
            std::cout << "Client is starting ...\n" << endl;

            time_t rawtime = time(NULL);
            struct tm timeinfo;
            char buffer[1024];
            localtime_s(&timeinfo, &rawtime);
            asctime_s(buffer, &timeinfo);

            cout << "Hello, world!" << endl;
            cout << "Current Datetime: " << string(buffer) << endl;

            std::cout << "Client is ready for work ...\n" << endl;
			Client();
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
