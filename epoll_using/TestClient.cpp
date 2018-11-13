//
// Created by faiz on 18-10-30.
//

#include "CSocket.h"

#include <string.h>

int main(int argc, char *argv[])
{

    const char *ip = "192.168.149.79";
    CSocket *socket = new CSocket();
    socket->setAddr(ip, 9999);
    socket->Connect();
    std::cout << "client connect server...\n";
    if(!socket->Send("ABCDEFGHIJ", 10))
    {
        std::cout <<"send err\n";
    }
    char recvbuf[256];
    memset(recvbuf,0, sizeof recvbuf);
    if(!socket->Recv(recvbuf, 16, 0))
    {
        std::cout <<"recv err\n";
    }

    std::cout << recvbuf << std::endl;
    getchar();
    return 0;
}
