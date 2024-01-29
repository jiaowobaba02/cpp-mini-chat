#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <locale>
#include <ctime>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

string username = "-";
string filename = "chat_history.txt";
string empty_note = "There is no message now, send a message";
string tmp_last_line = "this is the value of tmp_last_line";
// 全局变量，用于保存映射的内存地址和大小
char* file_data = nullptr;
off_t file_size = 0;

// 中断信号处理函数
void handle_interrupt(int signal) {
    // 解除内存映射
    if (file_data != nullptr && file_size > 0) {
        munmap(file_data, file_size);
    }

    // 退出程序
    exit(0);
}

string read_last_line() {
    static string last_line; // 使用静态变量来缓存最后一行消息
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        cerr << "无法打开文件。" << endl;
        exit(1);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        cerr << "无法获取文件大小。" << endl;
        exit(1);
    }

    if (file_size == 0) {
        last_line = empty_note;
    } else {
        if (file_size > 1024) {
            file_size = 1024; // 限制读取的文件大小为最后1KB
        }
        char* file_data = static_cast<char*>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
        if (file_data == MAP_FAILED) {
            cerr << "无法映射文件到内存。" << endl;
            exit(1);
        }

        char* newline_pos = strrchr(file_data, '\n');
        if (newline_pos != nullptr) {
            last_line = newline_pos + 1;
        } else {
            last_line = empty_note;
        }

        munmap(file_data, file_size);
    }

    close(fd);

    return last_line;
}

void write_to_last_line(const string& line) {
    int fd = open(filename.c_str(), O_RDWR);
    if (fd == -1) {
        cerr << "无法打开文件。" << endl;
        exit(1);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        cerr << "无法获取文件大小。" << endl;
        exit(1);
    }else {
        if (file_size > 1024) {
            file_size = 1024; // 限制读取的文件大小为最后1KB
        }
        char* file_data = static_cast<char*>(mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if (file_data == MAP_FAILED) {
            cerr << "无法映射文件到内存。" << endl;
            exit(1);
        }

        char* newline_pos = strrchr(file_data, '\n');
        if (newline_pos != nullptr) {
            string new_last_line = line;
            strcpy(newline_pos + 1, new_last_line.c_str());
        }

        munmap(file_data, file_size);
    }

    close(fd);
}

void receiveMessages() {
    while (true) {
        string last_line = read_last_line();
        if (last_line != tmp_last_line) {
            cout << last_line << endl;
        }
        tmp_last_line = last_line;
        sleep(0.1); // 注：实际这个量不怎么影响性能
    }
}

void sendMessages() {
    cin.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    while (true) {
        string message;
        getline(cin, message);

        // 将消息添加到聊天记录中
        write_to_last_line("-"+username+"-"+ ": " + message);
    }
}

int main() {
    setlocale(LC_ALL, "UTF-8"); // 设置终端的字符编码为UTF-8

    // 注册中断信号处理函数
    signal(SIGINT, handle_interrupt);
    write_to_last_line("    welcome");
    cout << "请输入用户名：";
    cin >> username;
    write_to_last_line("-"+username+"-joined the room");
    // 创建子进程来接收消息并显示
    pid_t pid = fork();
    if (pid == -1) {
        cerr << "无法创建子进程。" << endl;
        exit(1);
    } else if (pid == 0) {
        // 子进程
        receiveMessages();
    } else {
        // 父进程
        sendMessages();
    }

    return 0;
}
