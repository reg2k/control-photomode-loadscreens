#include "PhotoServer.h"

#include "Logger.h"
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <random>

static httplib::Server* g_server;
static std::string g_photoDir;

namespace PhotoServer
{
    DWORD WINAPI StartServer(LPVOID lpParameter) {
        g_server->listen_after_bind();
        return 0;
    }

    // Given a directory, returns the list of files in it.
    std::vector<std::string> GetFileList(const char* dir, const char* pattern = "*") {
        std::vector<std::string> fileList;
        HANDLE hFind;
        WIN32_FIND_DATAA findData;

        char findStr[MAX_PATH];
        snprintf(findStr, MAX_PATH, "%s\\%s", dir, pattern);

        hFind = FindFirstFileA(findStr, &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!strcmp(findData.cFileName, ".") || !strcmp(findData.cFileName, "..")) continue;
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

                fileList.emplace_back(findData.cFileName);

            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
        return fileList;
    }

    // Select a random file from within the specified directory.
    std::string SelectRandomFile(const char* dir) {
        std::vector<std::string> fileList = GetFileList(dir, "*.png");

        if (fileList.size() > 0) {
            // get random int
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distr(0, (int)fileList.size() - 1);
            int idx = distr(gen);

            return fileList[idx];
        }
        else {
            return "";
        }
    }

    int Start(std::string photoDir) {
        using namespace httplib;

        g_photoDir = photoDir;
        g_server = new Server();
        g_server->Get("/random_photo", [](const Request& req, Response& res) {
            std::string filename = SelectRandomFile(g_photoDir.c_str());
            if (filename.length() > 0) {
                _LOG("PhotoServer: Serving photo: %s", filename.c_str());
                std::ifstream file(g_photoDir + "\\" + filename, std::ios::binary);
                std::istreambuf_iterator<char> begin(file), end;
                std::vector<char> fileData(begin, end);
                res.set_content(fileData.data(), fileData.size(), "image/png");
            }
            else {
                _LOG("PhotoServer: No photos available! Please check whether the photo directory is correct.");
                res.set_content("Error: No photos available.", "text/plain");
            }
        });

        int port = g_server->bind_to_any_port("127.0.0.1");

        CreateThread(NULL, 0, StartServer, NULL, 0, NULL);
        return port;
    }
};