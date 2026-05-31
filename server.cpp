#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

// Hàm tính SHA256 để đối soát dữ liệu thực tế nhận được
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

// Hàm đảm bảo nhận đủ số byte yêu cầu từ Socket
int recv_all(SOCKET s, char* buffer, int size) {
    int total = 0;
    while (total < size) {
        int n = recv(s, buffer + total, size - total, 0);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);
    cout << "Server dang cho ket noi..." << endl;

    SOCKET client_fd = accept(server_fd, NULL, NULL);
    cout << "Client da ket noi!" << endl;
    
    bool is_correct = false;
    while (!is_correct) {
        cout << "\n==========================================" << endl;
        cout << "--- DOI NHAN DU LIEU KIEM TRA ---" << endl;

        // 1. Nhận mã Hash "chuẩn" từ Client (64 ký tự hex)
        char header_hash[65] = {0};
        if (recv_all(client_fd, header_hash, 64) == -1) break;
        header_hash[64] = '\0';
        
        // 2. Nhận kích thước file thực tế
        long long fileSize = 0;
        if (recv_all(client_fd, (char*)&fileSize, sizeof(fileSize)) == -1) break;

        // 3. Nhận dữ liệu file và lưu vào received.txt
        ofstream out("received.txt", ios::binary | ios::trunc);
        char buffer[4096];
        long long totalReceived = 0;
        while (totalReceived < fileSize) {
            int to_recv = (fileSize - totalReceived > 4096) ? 4096 : (int)(fileSize - totalReceived);
            int chunk = recv(client_fd, buffer, to_recv, 0);
            if (chunk <= 0) break;
            out.write(buffer, chunk);
            totalReceived += chunk;
        }
        out.close();

        // 4. Tính toán Hash thực tế và đối soát
        string actual_hash = sha256_file("received.txt");
        
        cout << "\n[DOI SOAT HE THONG]" << endl;
        cout << "-> Hash chuan: " << header_hash << endl;
        cout << "-> Hash thuc te: " << actual_hash << endl;

        if (actual_hash == string(header_hash)) {
            cout << "\n==> KET QUA: CHINH XAC! (Khop 100%)" << endl;
            send(client_fd, "OK", 2, 0);
            is_correct = true;
        } else {
            cout << "\n==> KET QUA: SAI HASH! Du lieu da bi thay doi." << endl;
            send(client_fd, "ER", 2, 0);
        }
    }

    if (is_correct) {
        cout << "\nHoan thanh! Dang mo file kiem tra..." << endl;
        system("notepad received.txt");
    }

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    system("pause");
    return 0;
}