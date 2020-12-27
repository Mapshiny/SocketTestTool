# SocketTestTool
一个基于QT的跨平台socket测试工具

### 背景
前几天想找一款linux环境下的socket测试工具，找了好久才找到，而且那个测试工具还存在点bug，不太好使，于是花了一周左右自己写了一个基于QT的SocketTestTool，目前支持测试tcp和udp两种协议。

## QT库

刚学会使用QT不久，发现QT的有好多已经写好的库， 代码里使用的有QTcpServer、QTcpSocket、QUdpSocket、QNetworkDatagram，这些库都是继承自QAbstractSocket，写起来难度很低，大部分时间都花在了数据类型的转换上，socket的连接，写数据等等参考文档就能写出来，难度不大。

## 结尾

自己代码写的还是比较烂，自己Ubuntu环境下的QT creator暂时还不能写简体中文，一直想解决这个问题，但是都没有动手去解决，所以代码里注释基本没有；还存在的一个问题是QT项目的打包，目前还没有打包过，如果想用我写的这个测试工具，请自行打包项目。
