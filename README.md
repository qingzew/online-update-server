# socket
##简介
这是一个简单的在线更新程序，无需停止server的运行，即可更新serve代码。在更新过中保持socket的open，不断接受client的请求并缓存，
待server重启成功后统一处理。
##使用说明
###环境
fedora

boost

g++
###server
生成可执行文件并执行：
```bash
 g++ -o server server.cpp -lboost_thread -lboost_system && ./server
```
###client
生成可执行文件并执行：
```bash
g++ -o client client.cpp && ./client
```
##执行结果
###cmd输出
```bash
[qingze@localhost socket]$ g++ -o client client.cpp && ./client
send message...
send message...
send message...
send message...
send message...
send message...
send message...
send message...
send message...
...
```
###查看日志
```bash
[root@localhost socket]# journalctl --since 15:07
-- Logs begin at Wed 2014-12-24 03:16:02 CST, end at Mon 2015-03-09 15:07:51 CST. --
Mar 09 15:07:51 localhost.localdomain myServer[787]: set signal ok...
```
###查看输出文件
```bash
[root@localhost log]# cat /tmp/output 
7f2340552700  / test Mon Mar  9 15:09:20 2015
7f2340552700  / test Mon Mar  9 15:09:21 2015
...
```
###更新程序
```bash
kill -SIGALRM PID
```
pid根据系统中server的进程ID替换
##帮助
如程序运行发生任何问题，请使`journalctl`命令查看系统日志
