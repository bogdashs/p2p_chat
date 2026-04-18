//
// Created by Admin on 10.04.2026.
//

#include "UpdaterClass.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include "../Utils/utils.h"

#pragma comment(lib, "urlmon.lib")

void UpdaterClass::checkUpdates() {
    std::string VersionGit_url = "https://raw.githubusercontent.com/bogdashs/p2p_chat/refs/heads/master/version";
    std::string DownloadGit_url = "https://github.com/bogdashs/p2p_chat/releases/latest/download/P2Pchat-release.zip";

    std::string tempVersion = "Data\\check.tmp";
    std::string tempDownload = "Data\\P2Pchat.zip";
    setColor(11);
    print("[ИНФО] Проверка обновлений");
    setColor(7);

    if (URLDownloadToFileA(NULL,VersionGit_url.c_str(),tempVersion.c_str(),0,NULL) == S_OK) {

        std::ifstream file(tempVersion);
        std::string remoteVersion;
        file >> remoteVersion;
        remoteVersion.erase(remoteVersion.find_last_not_of('\n\r\t') + 1);
        file.close();
        DeleteFileA(tempVersion.c_str());

        if (!remoteVersion.empty() && remoteVersion != CURRENT_VERSION) {
            setColor(14);
            print("[ОБНОВЛЕНИЕ] Доступна новая версия: " + remoteVersion);
            setColor(7);
            std::string choice = input("Желаете обновиться? (y/n): ");

            if (choice == "y" || choice == "Y") {
                setColor(11);
                print("[ИНФО] Доступна новая версия\n");
                setColor(7);

                if (URLDownloadToFileA(NULL, DownloadGit_url.c_str(),tempDownload.c_str(),0,NULL) == S_OK) {
                    setColor(10);
                    print("[УСПЕХ] Версия скачена!");
                    Sleep(2000);
                    setColor(7);

                    std::string args = "p2p_chat.exe " + tempDownload;

                    ShellExecuteA(NULL,"open","Updater.exe",args.c_str(),NULL,SW_SHOW);

                    exit(0);

                } else {
                    setColor(13);
                    print("[УСПЕХ]");
                    system("pause");
                    exit(0);
                }

            }


        } else {
            setColor(11);
            print("[ИНФО] У вас установлена актуальная версия");
            setColor(7);
        }



    }

}
