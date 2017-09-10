# gorlanHttp
一个小httpsever，核心代码不过300行。

## 目的

为了加深对于linux下编程的熟悉，与shell语言的熟悉，所以选择实现这个只有简单功能的httpserver

## 编译

    chmod u+x build.sh
    ./bulid.sh

新生成的目录output下即为便于移植的sever端。

拷贝目录output到主机

    chmod u+x server_ctl.sh
    ./server_ctl.sh

即可实现sever端的自动化启停。
