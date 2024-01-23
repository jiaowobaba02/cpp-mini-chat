#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <locale>
#include <ctime>

using namespace std;

string username = "-";
string filename = "chat_history.txt";
string empty_note = "There is no message now, send a message";
string tmp_last_line = "this is the value of tmp_last_line";

string read_last_line() {
    ifstream infile(filename);
    string line;
    vector<string> lines;
    while (getline(infile, line)) {
        lines.push_back(line);
    }
    if (!lines.empty()) {
        return lines.back();
    } else {
        return empty_note;
    }
}

void write_to_last_line(const string& line) {
    /*ofstream outfile(filename, ios::app);
    outfile.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    outfile << line << '\n';
    outfile.close();*/
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    ifstream infile(filename);
    string tmp_filename = filename + ".tmp";
    string tmp_line;
    ofstream outfile(tmp_filename);
    bool first_line = true;

    // Read all lines from the file
    while (getline(infile, tmp_line)) {
        // Write all lines except the last one to the temporary file
        if (!first_line) {
            outfile << tmp_line << '\n';
        }
        first_line = false;
    }

    // Write the new last line to the temporary file

    outfile.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    outfile <<"["<<t->tm_year + 1900 << "/" << t->tm_mon + 1<< "/" << t->tm_mday << " "<<t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec <<"]"<< line << '\n';
    // Close the input and output files
    infile.close();
    outfile.close();

    // Rename the temporary file to the original filename
    if (rename(tmp_filename.c_str(), filename.c_str()) != 0) {
        throw runtime_error("Failed to rename temporary file.");
    }
}

void receiveMessages() {
    while (true) {
        string last_line = read_last_line();
        if (last_line != tmp_last_line) {
            cout << last_line << endl;
        }
        tmp_last_line = last_line;
        sleep(0.5); // 暂停0.5秒钟，避免过于频繁地读取聊天记录
    }
}

void sendMessages() {
    cin.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    while (true) {
        string message;
        //cout << "请输入您的消息：";
        //cin.ignore(); // 忽略之前的换行符
        getline(cin, message);

        // 将消息添加到聊天记录中
        write_to_last_line(username + ": " + message);
    }
}

int main() {
    setlocale(LC_ALL, "UTF-8"); // 设置终端的字符编码为当前环境的默认编码
    cout.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    cout << "您还没有用户名，请输入一个用户名：" << endl;
    cin >> username;
    string greeting = "-" + username + "-" + " joined the room";
    write_to_last_line(greeting);
    username = "-" + username + "-";
    pid_t pid = fork();

    if (pid == -1) {
        cerr << "无法创建子进程。" << endl;
        return 1;
    } else if (pid == 0) {
        // 子进程负责接收消息并显示
        receiveMessages();
    } else {
        // 父进程负责发送消息
        sendMessages();
        wait(NULL); // 等待子进程结束
    }

    return 0;
}
