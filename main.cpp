#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <winsock2.h>
#include <ws2tcpip.h>
#include <natupnp.h>
#include <comutil.h>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
#include <windows.h>
#include <limits>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")


std::string input(std::string promt) {
    std::string value;
    std::cout << promt;
    std::getline(std::cin, value);
    return value;

}
void print(std::string promt) {
    std::cout << promt << std::endl;
}

std::string EncDec(std::string data,std::string key) {
    std::string output = data;
    for (int i = 0; i < data.size(); i++) {
        output[i] = data[i] ^ key[i % key.length()];
    }
    return output;
}


std::string getLocalIP() {
    char hostname[256];

    if (gethostname(hostname,sizeof(hostname)) == SOCKET_ERROR) return "127.0.0.1";

    struct addrinfo hints = {}, *res;

    hints.ai_family = AF_INET;
    if (getaddrinfo(hostname,NULL,&hints,&res) != 0) return "127.0.0.1";

    char ipString[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&((struct sockaddr_in*)res->ai_addr)->sin_addr,ipString,INET_ADDRSTRLEN);

    freeaddrinfo(res);
    return std::string(ipString);

}

bool openPortUPnP(int port) {

    HRESULT hr = CoInitialize(NULL);
    IUPnPNAT* pNat = NULL;
    hr = CoCreateInstance(CLSID_UPnPNAT,NULL,CLSCTX_ALL,IID_IUPnPNAT,(void**)&pNat);
    if (FAILED(hr) || !pNat) return false;

    IStaticPortMappingCollection* pMappings = NULL;
    hr = pNat->get_StaticPortMappingCollection(&pMappings);
    if (SUCCEEDED(hr) && pMappings) {

        IStaticPortMapping* pMap = NULL;

        BSTR bstrProto = SysAllocString(L"TCP");
        BSTR bstrDesc = SysAllocString(L"P2P_Ghost_Chat");

        std::string localIp = getLocalIP();

        _bstr_t bstrInternalIP = localIp.c_str();

        hr = pMappings->Add(port,bstrProto,port,bstrInternalIP,VARIANT_TRUE,bstrDesc,&pMap);
        SysFreeString(bstrProto);
        SysFreeString(bstrDesc);
        return SUCCEEDED(hr);
    }

    return false;

}

void receiverThread(SOCKET s,std::string key) {

    char buffer[4096];
    while (true) {
        int bytes = recv(s, buffer, sizeof(buffer) -1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';

        std::string decrypted = EncDec(std::string(buffer), key);

        std::cout << "\n[ДРУГ]: " << decrypted << "\n" <<std::endl;
    }

}



int main () {

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);


    // setlocale(LC_ALL, "Russian");
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    std::cout << "--- P2P GHOST CHAT ---\n";

    std::string roomKey = "my_secret_key";

    std::cout << "1. Создать руму\n2. Присоединиться\nВыбор: ";

    int mode; std::cin >> mode;

    // std::cin.ignore((std::numeric_limits<std::streamsize>::max)(),'\n');

    SOCKET s = socket(AF_INET,SOCK_STREAM,0);

    int port;
    std::cout << "Введите порт: "; std::cin >> port;

    if (mode == 1) {

        openPortUPnP(port);
        print("[UPnP] Порт " + std::to_string(port) + " открыт!");

        sockaddr_in addr = {AF_INET,htons(port),INADDR_ANY};

        bind(s,(sockaddr*)&addr,sizeof(addr));

        listen(s,1);

        print("[ОЖИДАНИЕ] Ждем подключения...\n");

        SOCKET client_socket = accept(s,NULL,NULL);

        // if (client_socket == INVALID_SOCKET) {
        //     print("[ОШИБКА] accept, код: "+ std::to_string(WSAGetLastError())+"\n");
        //     return 1;
        // }
        // bind(s,(sockaddr*)&addr,sizeof(addr));
        //
        // listen(s,1);
        //
        // SOCKET client_socket = accept(s,NULL,NULL);
        closesocket(s);
        s = client_socket;
        } else {
            std::string targetIP;
            std::cout << "Введите IP друга: "; std::cin >> targetIP;
            sockaddr_in addr;

            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);

            inet_pton(AF_INET,targetIP.c_str(),&addr.sin_addr);
            if (connect(s,(sockaddr*)&addr,sizeof(addr)) != 0) {
                print("Ошибка подключения!");
                return 1;
            }
        }
        print("[УСПЕХ] Соединение установлено!\n");
        std::thread(receiverThread,s,roomKey).detach();

        std::string msg;
        std::getline(std::cin,msg);

        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(),'\n');

        while (true) {

            std::cout << ">> " << std::flush;
            if (!std::getline(std::cin,msg)) break;

            if (msg.empty()) continue;

            std::string encMsg = EncDec(msg,roomKey);

            send(s,encMsg.c_str(),encMsg.length(),0);

        }

    WSACleanup();
    return 0;

}