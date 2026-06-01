# File Transfer Integrity Verification using SHA256

## Giới thiệu

Đây là chương trình Client-Server sử dụng TCP Socket trên Windows để truyền file và kiểm tra tính toàn vẹn dữ liệu bằng thuật toán SHA256.

Mục tiêu của chương trình:

* Truyền file từ Client đến Server.
* Đảm bảo nội dung file nhận được giống hoàn toàn với file gốc.
* Phát hiện mọi thay đổi dù chỉ 1 ký tự hoặc 1 byte.
* Yêu cầu gửi lại nếu dữ liệu không khớp.

---

# Công nghệ sử dụng

* C++
* WinSock2
* OpenSSL SHA256
* TCP Socket

---

# Cấu trúc Project

```text
Project/
│
├── client.cpp
├── server.cpp
├── README.md
│
└── received.txt
```

Trong đó:

* `client.cpp`: Chương trình gửi file.
* `server.cpp`: Chương trình nhận file và kiểm tra hash.
* `received.txt`: File được Server lưu sau khi nhận.

---

# Nguyên lý hoạt động

## Bước 1: Tính SHA256 của file gốc

Client mở file:

```text
D:\Study\word\testcode.txt
```

và tính:

```text
SHA256(file_goc)
```

Ví dụ:

```text
2cf24dba5fb0a...
```

Hash này được xem là "chuẩn tham chiếu".

---

## Bước 2: Kết nối Server

Client kết nối đến:

```text
IP   : 127.0.0.1
Port : 12345
```

Server lắng nghe tại cổng:

```text
12345
```

---

## Bước 3: Gửi dữ liệu

Client gửi theo thứ tự:

### Header 1

Hash chuẩn

```text
64 ký tự SHA256
```

Ví dụ:

```text
2cf24dba5fb0a...
```

### Header 2

Kích thước file

```cpp
long long fileSize
```

### Payload

Toàn bộ nội dung file.

---

## Bước 4: Server nhận dữ liệu

Server nhận:

* Hash chuẩn
* Kích thước file
* Nội dung file

Sau đó lưu thành:

```text
received.txt
```

---

## Bước 5: Đối soát SHA256

Server tính:

```text
SHA256(received.txt)
```

Sau đó so sánh:

```text
Hash chuẩn
=
Hash thực tế
?
```

---

# Kết quả

## Trường hợp đúng

```text
Hash chuẩn = Hash thực tế
```

Server gửi:

```text
OK
```

Client hiển thị:

```text
KET QUA: CHINH XAC!
```

---

## Trường hợp sai

```text
Hash chuẩn ≠ Hash thực tế
```

Server gửi:

```text
ER
```

Client hiển thị:

```text
KET QUA: SAI NOI DUNG!
```

và yêu cầu gửi lại.

---

# Luồng dữ liệu

```text
CLIENT
   |
   |-- SHA256 gốc
   |-- File Size
   |-- File Data
   |
   v
SERVER
   |
   |-- Lưu file
   |-- Tính SHA256
   |-- So sánh Hash
   |
   +--> OK
   |
   +--> ER
```

---

# Thuật toán kiểm tra toàn vẹn

```text
File gốc
    |
    v
SHA256
    |
    v
Hash chuẩn
    |
    +-------------------+
                        |
                        v
                So sánh với
                Hash thực tế
                        |
                        v
               File nhận được
```

Nếu chỉ thay đổi:

* 1 ký tự
* 1 byte
* 1 dòng

thì SHA256 sẽ thay đổi hoàn toàn.

---

# Cách chạy chương trình

## Bước 1

Biên dịch Server

```bash
g++ server.cpp -o server.exe -lws2_32 -lssl -lcrypto
```

## Bước 2

Biên dịch Client

```bash
g++ client.cpp -o client.exe -lws2_32 -lssl -lcrypto
```

## Bước 3

Chạy Server

```bash
./server.exe
```

Kết quả:

```text
Server dang cho ket noi...
```

## Bước 4

Chạy Client

```bash
./client.exe
```

Kết quả:

```text
Client da ket noi!
```

---

# Kịch bản kiểm thử

## Test 1: File đúng

Không sửa file.

Kết quả:

```text
KET QUA: CHINH XAC!
```

---

## Test 2: File bị sửa

Sửa nội dung file trước khi gửi.

Ví dụ:

```text
Hello World
```

thành:

```text
Hello World 123
```

Kết quả:

```text
KET QUA: SAI HASH!
```

---

# Hạn chế hiện tại

* Chỉ hỗ trợ 1 Client.
* Chưa gửi tên file.
* Chưa mã hóa dữ liệu.
* Chưa có giao diện GUI.
* Chưa hỗ trợ truyền nhiều file.

---

# Hướng phát triển

* Gửi tên file.
* Gửi nhiều file.
* Hỗ trợ thư mục.
* Mã hóa AES.
* Đa luồng (Multi-thread).
* Truyền file qua mạng LAN.
* Giao diện GUI bằng Qt.
* Thanh tiến trình (Progress Bar).

---

# Tác giả

Project thực hiện nhằm nghiên cứu:

* TCP Socket Programming
* SHA256 Hash Verification
* Data Integrity Checking
* Client-Server Architecture
