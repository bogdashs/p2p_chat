#include <winsock2.h>
#include <ws2tcpip.h>
#include <natupnp.h>
#include <comutil.h>
#include <string>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"oleaut32.lib")

std::string


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



}



int main () {




}