ZooKeeper-Exam
==============
####准备工作

+ 安装部署zookeeper
  
  + 1： 需要Java运行环境
  + 2： 下载zookeeper-3.4.8.tar.gz,然后解压到/opt
  + 3： 将/opt/zookeeper-3.4.8/conf目录下面的zoo_sample.cfg修改为zoo.cfg
  + 4： 运行bin/zkServer.sh start,我们以standalone模式运行

+ 编译客户端的C库

  在/opt/zookeeper-3.4.8/src/c目录下面，执行如下命令
  
  + 1： ./configure
  + 2：  make
  + 3： make install
  + 4： 将动态库的路径/usr/local/lib添加到/etc/ld.so.conf文件中，
  		即添加如下内容：
        	/usr/local/lib
  + 5：使修改生效/sbin/ldconfig -v
  
+ 注： zookeeper.h有详细的参数说明（位置/usr/local/include/zookeeper），看不懂可以参考里面的说明

#### [ClusterMonitor  ： 集群监控和Master选举](http://blog.csdn.net/qq910894904/article/details/40834083)

+ 背景说明参考[文章](http://blog.csdn.net/qq910894904/article/details/40834083)。
+ 在ClusterMonitor目录下面执行make，编译生成monitor程序。
+ 启动多个monitor程序，然后杀死master进程，我们可以看到新的Master产生。

#### [NameService: 命名服务](http://blog.csdn.net/qq910894904/article/details/40833859)
+ 背景说明参考[文章](http://blog.csdn.net/qq910894904/article/details/40833859)。
+ 在NameService目录下面执行make，编译生成nameservice程序。
+ 启动多个nameservice程序，如下：

        服务提供者：
            ./nameservice -m provider -n query_bill 
        服务消费者：
            ./nameservice -m consumer -n query_bill 
        服务监控者：
            ./nameservice -m monitor -n query_bill 
            
        这样服务提供着发生变化的时候，服务消费者能够获取到通知。

#### [分布式通知/协调](http://blog.csdn.net/qq910894904/article/details/40833981)
+ 背景说明参考[文章](http://blog.csdn.net/qq910894904/article/details/40833981)。
+ 在Notify目录下面执行make，编译生成notify程序。
+ 启动多个notify程序，如下：

        ./notify -p /Notify
        然后使用zkCli.sh往节点/Notify写入数据
        [zk: localhost:2181(CONNECTED) 4] set /Notify 1.0
        
        notify会获取到数据的变化。
        
#### [分布式锁](http://blog.csdn.net/qq910894904/article/details/40834397)
+ 背景说明参考[文章](http://blog.csdn.net/qq910894904/article/details/40834397)。
+ 在Lock目录下面执行make，编译生成mylock程序。
+ 启动两个mylock程序，如下：
    
        ./mylock
        输出
            create path /Lock successfully!
            create path /Lock successfully!
            get lock of /Lock.
            self path is /Lock/lock-0000000000.
            do something....

        ./mylock
        create path /Lock successfully!
        #当上面的程序退出，输出如下内容
        get lock of /Lock.
        self path is /Lock/lock-0000000001.
        do something....



#### [Config : 远程配置管理](http://blog.csdn.net/qq910894904/article/details/40833747)

#### [分布式队列](http://blog.csdn.net/qq910894904/article/details/40834609)


### 参考资料

+ [ 《API常用函数功能与参数详解》](http://blog.csdn.net/poechant/article/details/6675431)