# cpp-mini-chat
一个用C++和ssh实现的简单的聊天软件
## 需要环境：
- Linux
- g++编译器
- openssh-server
- 内网穿透软件（如:ngrok）

## 已知问题：
> Q:我编译成功，但是运行时报如下错误： `terminate called after throwing an instance of 'std::runtime_error'
  what():  locale::facet::_S_create_c_locale name not valid
Aborted (core dumped)
`

 *A:可能是缺库*

 > Q:我发现了这个程序容易漏消息

*A:这个还没有解决，可以尝试缩短刷新时间，影响不大*
> 注：由于内存映射并不会用，所以最新版暂时删除
## 搭建：
- 在你所要搭建的Linux上的用户主目录（例：/home/user）将本仓库的main.cpp复制到那里，之后运行 `g++ ./main.cpp -o chat`
- > 注：你可能需要自行建立 `chat_history.txt` 文件
- 启用ssh服务
- 将服务器通过内网穿透软件公开的公网
- 让别人通过ssh进入后输入 `./chat` 这样就能聊天了

### 预实现：
- [x] 无历史记录
- [x] 支持房间加密 ~因为ssh协议就是加密的~
- [ ] 支持群聊命令
- [ ] 支持私信
- [ ] 支持在浏览器端预览
- [ ] 支持传输文件
- [ ] 支持终端新消息响铃
- [x] 支持终端颜色

> 会持续更新
