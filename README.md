ZooKeeper-Exam
==============

####ZooKeeper练习代码
* ClusterMonitor  ： 集群监控和Master选举
* Config : 远程配置管理，根据[inifile2](https://github.com/Winnerhust/inifile2)修改而成，主要修改了open函数，其他不变
* Notify : 分布式通知/协调
* NameService: 命名服务
* Lock: 分布式锁
* Queue: 分布式队列
* mymetaq: 负载均衡，实现了生产者负载均衡，消费者负载均衡尚未完成

更多详情，请转到[原作者的博客](http://blog.csdn.net/qq910894904/article/details/40835105)


####准备工作

+ 安装部署zookeeper
  
  1： 需要Java运行环境
  2： 下载zookeeper-3.4.8.tar.gz,然后解压到/opt
  3： 将/opt/zookeeper-3.4.8/conf目录下面的zoo_sample.cfg修改为zoo.cfg
  4： 运行bin/zkServer.sh start,我们以standalone模式运行

+ 编译客户端的C库

  在/opt/zookeeper-3.4.8/src/c目录下面，执行如下命令
  1： ./configure
  2：  make
  3： make install
  4： 将动态库的路径/usr/local/lib添加到/etc/ld.so.conf文件中，
  		即添加如下内容：
        	/usr/local/lib
  5：使修改生效/sbin/ldconfig -v
  
+ 注： zookeeper.h有详细的参数说明（位置/usr/local/include/zookeeper），看不懂可以参考里面的说明

#### [ClusterMonitor  ： 集群监控和Master选举](http://blog.csdn.net/qq910894904/article/details/40834083)

+ 背景说明参考[文章](http://blog.csdn.net/qq910894904/article/details/40834083)
+ 

#### [Config : 远程配置管理](http://blog.csdn.net/qq910894904/article/details/40833747)


	create /Conf "" 
     create /Conf/test.ini "[COMMON]\nDB=mysql\nPASSWD=root\n"

        
./testcase -r -p /Conf/test.ini -s localhost:2181

#### [NameService: 命名服务](http://blog.csdn.net/qq910894904/article/details/40833859)

#### [分布式通知/协调](http://blog.csdn.net/qq910894904/article/details/40833981)


#### [集群监控和Master选举](http://blog.csdn.net/qq910894904/article/details/40834083)


#### [分布式锁](http://blog.csdn.net/qq910894904/article/details/40834397)


#### [](http://blog.csdn.net/qq910894904/article/details/40834609)


### 参考资料

+ [ 《API常用函数功能与参数详解》](http://blog.csdn.net/poechant/article/details/6675431)