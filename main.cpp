#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
//#include <sys/types.h> 经过测试不需要该库
//#include <sys/wait.h> 经过测试不需要该库
#include <locale>
#include <ctime>
#include <sys/ioctl.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>

using namespace std;

string username = "-";
string filename = "chat_history.txt";
string empty_note = "There is no message now, send a message";
string tmp_last_line = "this is the value of tmp_last_line";
static string last_line;
string message;
int color;
struct winsize w;
int lines;
string line;
vector<string> lines_read;

void instructon(){
    
}

void scrollOutput(int lines) {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size); // 获取终端窗口的大小

    std::cout << "\x1b[" << size.ws_row - lines << "H"; // 将光标向上移动指定行数
}

string read_last_line() {
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        lines_read.push_back(line);
    }
    infile.close();
    if (!lines_read.empty()) {
        return lines_read.back();
        lines_read.clear();
    } else {
        return empty_note;
    }
    return 0;
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
    //先写入tmp文件->重命名
    while (getline(infile, tmp_line)) {
        if (!first_line) {
            outfile << tmp_line << '\n';
        }
        first_line = false;
    }


    outfile.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    outfile <<"["<<t->tm_year + 1900 << "/" << t->tm_mon + 1<< "/" << t->tm_mday << " "<<t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec <<"]"<< line << '\n';
    infile.close();
    outfile.close();

    if (rename(tmp_filename.c_str(), filename.c_str()) != 0) {
        throw runtime_error("Failed to rename temporary file.");
    }
}

void receiveMessages() {
    while (true) {
        string last_line = read_last_line();
        if (last_line != tmp_last_line) {
            //TODO!让消息在倒数第二行滚动，而不是倒数第一行
            scrollOutput(0);
            std::cout << "\033[1A\033[K" << last_line << "\033[999B" << std::endl;
            cout<<"\033["<<to_string(color)<<"m"<<username<<":\033[0m";
            cout << "\033[" << 500<< ";" << username.length()+2 << "H";
        }
        tmp_last_line = last_line;
        this_thread::sleep_for(chrono::milliseconds(10)); // 暂停0.001秒钟，避免过于“频繁”地读取聊天记录
    }
}

void sendMessages() {
    cin.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    while (true) {
        //cout << "请输入您的消息：";
        //cin.ignore(); // 忽略之前的换行符
        
        
        
        getline(cin, message);

        // 将消息添加到聊天记录中
        write_to_last_line("\033[" +to_string(color) + "m-"+username+"-\033[0m"+ ": " + message);
    }
}

int main() {
    setlocale(LC_ALL, "UTF-8"); // 设置终端的字符编码为当前环境的默认编码
    cout.imbue(locale("zh_CN.UTF-8")); // 设置流的字符编码为 UTF-8
    
    cout << "您还没有用户名，请输入一个用户名：" << endl;
    cin >> username;
    unsigned long long hash = 5356 ;
    for (char c : username){
    	hash = ((hash << 5)+hash)+c;
    }
    std::srand(hash);
    color=rand()%6+31;
    write_to_last_line("\033["+to_string(color)+"m"+"-"+username+"-"+"\033[0m joined the room");
    this_thread::sleep_for(chrono::milliseconds(100));
    /*pid_t pid = fork();

    if (pid == -1) { //已弃用
        cerr << "无法创建子进程。" << endl;
        return 1;
    } else if (pid == 0) {
        // 子进程负责接收消息并显示
        receiveMessages();
    } else {
        // 父进程负责发送消息
        sendMessages();
        wait(NULL); // 等待子进程结束
    }*/
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> finished = false;
    thread t(receiveMessages);
    thread s(sendMessages);
    
    t.join();
    s.join();

    return 0;
}
