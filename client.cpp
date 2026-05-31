#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <winsock2.h>
#include <openssl/sha.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

string sha256_file(const string &filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return "";
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    char buffer[4096];
    while (file.good()) {
        file.read(buffer, sizeof(buffer));
        SHA256_Update(&ctx, buffer, file.gcount());
    }
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &ctx);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Loi ket noi den Server!" << endl;
        return 1;
    }

    // Luu y: Thay đổi đường dẫn này đúng với file của bạn
    string filename = "D:\\Study\\word\\testcode.txt"; 

    // Chốt Hash gốc ngay khi bắt đầu chương trình
    string original_hash = sha256_file(filename);
    if (original_hash == "") {
        cout << "Khong tim thay file " << filename << endl;
        return 1;
    }

    cout << "--- HE THONG DA KHOI TAO ---" << endl;
    cout << "[MUC TIEU] Hash goc: " << original_hash << endl;

    bool success = false;
    int attempt = 1;

    do {
        cout << "\n--- LAN TRUYEN THU " << attempt << " ---" << endl;
        cout << "Bam ENTER de gui file di doi soat..." << endl;
        cin.get(); 

        ifstream file(filename, ios::binary);
        file.seekg(0, ios::end);
        long long fileSize = file.tellg();
        file.seekg(0, ios::beg);

        // 1. Gửi mã Hash chuẩn (luôn gửi bản gốc)
        send(sock, original_hash.c_str(), 64, 0);
        
        // 2. Gửi kích thước hiện tại của file
        send(sock, (char*)&fileSize, sizeof(fileSize), 0);

        // 3. Gửi nội dung file hiện tại
        char buffer[4096];
        while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
            send(sock, (char*)buffer, (int)file.gcount(), 0);
        }
        file.close();

        // 4. Đợi phản hồi
        cout << "[LOG] Dang doi Server kiem tra..." << endl;
        char response[3] = {0};
        recv(sock, response, 2, 0);
        
        if (strcmp(response, "OK") == 0) {
            cout << ">> KET QUA: CHINH XAC! File da khop ban dau." << endl;
            success = true;
        } else {
            cout << ">> KET QUA: SAI NOI DUNG! Hay sua file ve dung ban goc." << endl;
            attempt++;
        }
    } while (!success);

    closesocket(sock);
    WSACleanup();
    system("pause");
    return 0;
}