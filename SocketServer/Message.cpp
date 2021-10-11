#include "pch.h"
#include "Message.h"

string GetLastErrorString(DWORD ErrorID = 0)
{
	if (!ErrorID)
		ErrorID = GetLastError();
	if (!ErrorID)
		return string();

	LPSTR pBuff = nullptr;
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pBuff, 0, NULL);
	string s(pBuff, size);
	LocalFree(pBuff);

	return s;
}

int Message::clientID = 0;

void Message::send(CSocket& s, int to, int from, int type, const string& data)
{
	Message m(to, from, type, data);
	m.send(s);
}

Message Message::send(int to, int type, const string& data)
{
	CSocket s;
	s.Create();
	if (!s.Connect("127.0.0.1", 12345))
	{
		throw runtime_error(GetLastErrorString());
	}
	Message m(to, clientID, type, data);
	m.send(s);
	if (m.receive(s) == MT_INIT)
	{
		clientID = m.header.to;
	}
	return m;
}



