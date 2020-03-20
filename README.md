#SSR-qt5

Qt 编写的 SSR 客户端，调用 ssrr-local 和 socks2http。
目前功能仅供自用。

功能：
1. 点击[更新订阅]菜单，可通过调用 python 脚本更新订阅
2. 可显示服务器列表，单击可显示服务器信息，双击服务器可杀死已有 ssrr-local 进程，并启动新的 ssrr-local 进程连接选定服务器
3. 启动时会调用 socks2http，退出时会杀死 ssrr-local 进程和 socks2http 进程
4. 有托盘图标，点击托盘图标会显示或隐藏界面，右键图标有[退出]选项，右键菜单有服务器列表，点击可杀死已有 ssrr-local 进程并开启新进程连接对应服务器
5. 点击[设置]菜单可设置订阅链接、socks 端口、http 端口

安装：
需先安装 [shadowsocksr-libev](https://github.com/shadowsocksrr/shadowsocksr-libev)，见 [AUR:shadowsocksrr-libev-git](https://aur.archlinux.org/packages/shadowsocksrr-libev-git)。

感谢：
- [shadowsocksr-libev](https://github.com/shadowsocksrr/shadowsocksr-libev)
- electron-ssr
- shadowsocks-qt5
