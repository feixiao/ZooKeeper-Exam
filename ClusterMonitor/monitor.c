#include <stdio.h>  
#include <string.h>  
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "zookeeper.h"  
#include "zookeeper_log.h"  


enum WORK_MODE{MODE_MONITOR,MODE_WORKER} g_mode;
char g_host[512]= "localhost:2181";  

//watch function when child list changed
void zktest_watcher_g(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);
//show all process ip:pid
void show_list(zhandle_t *zkhandle,const char *path);
//if success,the g_mode will become MODE_MONITOR
void choose_mater(zhandle_t *zkhandle,const char *path);
//get localhost ip:pid
void getlocalhost(char *ip_pid,int len);

void print_usage();
void get_option(int argc,const char* argv[]);

/**********unitl*********************/  
void print_usage()
{
    printf("Usage : [monitor] [-h] [-m] [-s ip:port] \n");
    printf("        -h Show help\n");
    printf("        -m set monitor mode\n");
    printf("        -s server ip:port\n");
    printf("For example:\n");
    printf("monitor -m -s172.17.0.36:2181 \n");
}
 
void get_option(int argc,const char* argv[])
{
	extern char    *optarg;
	int            optch;
	int            dem = 1;
	const char    optstring[] = "hms:";
    
    //default    
    g_mode = MODE_WORKER;
    
	while((optch = getopt(argc , (char * const *)argv , optstring)) != -1 )
	{
		switch( optch )
		{
		case 'h':
			print_usage();
			exit(-1);
		case '?':
			print_usage();
			printf("unknown parameter: %c\n", optopt);
			exit(-1);
		case ':':
			print_usage();
			printf("need parameter: %c\n", optopt);
			exit(-1);
        case 'm':
                g_mode = MODE_MONITOR;
            break;
        case 's':
            strncpy(g_host,optarg,sizeof(g_host));
            break;
		default:
			break;
		}
	}
} 


void zktest_watcher_g(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)  
{  
    if(type == ZOO_CHILD_EVENT &&
       state == ZOO_CONNECTED_STATE ){
        choose_mater(zh,path);
        if(g_mode == MODE_MONITOR){
            show_list(zh,path);
        }
    }
}  
void getlocalhost(char *ip_pid,int len)
{
    char hostname[64] = {0};
    struct hostent *hent ;

    gethostname(hostname,sizeof(hostname));
    hent = gethostbyname(hostname);

    char * localhost = inet_ntoa(*((struct in_addr*)(hent->h_addr_list[0])));

    snprintf(ip_pid,len,"%s:%lld",localhost,getpid());
}

void choose_mater(zhandle_t *zkhandle,const char *path)
{
    struct String_vector procs;
    int i = 0;
    /*
    函数原型：
        ZOOAPI int zoo_get_children(zhandle_t *zh, const char *path, int watch,
                            struct String_vector *strings);
    函数功能：
        同步的获取路径下面的子节点。
    参数说明：
        zh：zookeeper的句柄，由zookeeper_init得到。
        path：节点名称，就是一个类似于文件系统写法的路径。
        watch：设置为0，则无作用。设置为非0时，服务器会通知节点已经发生变化。
        strings：子节点的路径值。
    */
    int ret = zoo_get_children(zkhandle,path,1,&procs);
        
    if(ret != ZOK || procs.count == 0){
        fprintf(stderr,"failed to get the children of path %s!\n",path);
    }else{
        char master_path[512] ={0};
        char ip_pid[64] = {0};
        int ip_pid_len = sizeof(ip_pid);
        
        char master[512]={0};
        char localhost[512]={0};
        
        getlocalhost(localhost,sizeof(localhost));
        
        strcpy(master,procs.data[0]);
        for(i = 1; i < procs.count; ++i){
            if(strcmp(master,procs.data[i])>0){
                printf("master:%s",master);
                strcpy(master,procs.data[i]);
            }
        }

        sprintf(master_path,"%s/%s",path,master);
    /*
    函数原型：
        ZOOAPI int zoo_get(zhandle_t *zh, const char *path, int watch, char *buffer,   
                   int* buffer_len, struct Stat *stat);
    函数功能：
        同步的获取节点数据。
    参数说明：
         zh：zookeeper的句柄，由zookeeper_init得到。
         path：节点名称，就是一个类似于文件系统写法的路径。
         watch：设置为0，则无作用。设置为非0时，服务器会通知节点已经发生变化。
         buffer：保存服务器返回的数据。
         buffer_len：buffer的长度。
         stat：返回节点的stat消息。
    */
        ret = zoo_get(zkhandle,master_path,0,ip_pid,&ip_pid_len,NULL);
        if(ret != ZOK){
            fprintf(stderr,"failed to get the data of path %s!\n",master_path);
        }else if(strcmp(ip_pid,localhost)==0){
            g_mode = MODE_MONITOR;
        }
        
    }

    // 释放内存
    for(i = 0; i < procs.count; ++i){
        free(procs.data[i]);
        procs.data[i] = NULL;
    }

}
void show_list(zhandle_t *zkhandle,const char *path)
{

    struct String_vector procs;
    int i = 0;
    char localhost[512]={0};

    getlocalhost(localhost,sizeof(localhost));

    int ret = zoo_get_children(zkhandle,path,1,&procs);
        
    if(ret != ZOK){
        fprintf(stderr,"failed to get the children of path %s!\n",path);
    }else{
        char child_path[512] ={0};
        char ip_pid[64] = {0};
        int ip_pid_len = sizeof(ip_pid);
        printf("--------------\n");
        printf("ip\tpid\n");
        for(i = 0; i < procs.count; ++i){
            sprintf(child_path,"%s/%s",path,procs.data[i]);
            ret = zoo_get(zkhandle,child_path,0,ip_pid,&ip_pid_len,NULL);
            if(ret != ZOK){
                fprintf(stderr,"failed to get the data of path %s!\n",child_path);
            }else if(strcmp(ip_pid,localhost)==0){
                printf("%s(Master)\n",ip_pid);
            }else{
                printf("%s\n",ip_pid);
            }
        }
    }
    // 释放内存
    for(i = 0; i < procs.count; ++i){
        free(procs.data[i]);
        procs.data[i] = NULL;
    }
}

int main(int argc, const char *argv[])  
{  
    int timeout = 30000;  
    char path_buffer[512];  
    int bufferlen=sizeof(path_buffer);  
  
    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN); // 设置日志级别,避免出现一些其他信息  

    get_option(argc,argv);
    /*
    函数声明：
        ZOOAPI zhandle_t *zookeeper_init(const char *host, watcher_fn fn,
              int recv_timeout, const clientid_t *clientid, void *context, int flags);
    
    函数功能：
        创建新的zookeeper会话和Handle，因为是异步调用除非ZOO_CONNECTED_STATE事件接收到否则不能认为连接成功。
    
    参数说明：
        host：zookeepe服务的地址，类似"127.0.0.1:3000,127.0.0.1:3001,127.0.0.1:3002"。
        watcher_fn：全局的watcher回调函数，有事件发生时会被调用。
        recv_timeout： 会话的超时时间。
        clientid: 之前建立过连接，现在要重新连的客户端（client）ID。如果之前没有，则为0。
        context: 配合zhandle_t实例使用的handback对象，可以通过zoo_get_context获取。
        flag: 设置为0，zookeeper开发团队保留以后使用。
     */
    zhandle_t* zkhandle = zookeeper_init(g_host,zktest_watcher_g, timeout, 0, (char *)"Monitor Test", 0);  

    if (zkhandle ==NULL)  
    {  
        fprintf(stderr, "Error when connecting to zookeeper servers...\n");  
        exit(EXIT_FAILURE);  
    }  
    
    char path[512]="/Monitor";
    
    /*
    函数声明：
       ZOOAPI int zoo_exists(zhandle_t *zh, const char *path, int watch, struct Stat *stat);
       
    函数功能：
       同步监视一个zookeeper节点（node）是否存在。

    参数说明：
        zh：zookeeper的句柄，由zookeeper_init得到。
        path：节点名称，就是一个类似于文件系统写法的路径。
        watch：设置为0，则无作用。设置为非0时，
        stat：返回节点的stat消息。
    
    返回值：
        ZOK，ZNONODE，ZNOAUTH，ZBADARGUMENTS，ZINVALIDSTATE，ZMARSHALLINGERROR。
        ZOK表示操作成功，
        ZNONODE表示该节点不存在，
        ZNOAUTH表示客户端（client）无权限，
        ZINVALIDSTATE表示存在非法的参数，后两者暂略（TODO）。
    */
    int ret = zoo_exists(zkhandle,path,0,NULL); 
    if(ret != ZOK){ 
    /*
    函数声明：
        ZOOAPI int zoo_create(zhandle_t *zh, const char *path, const char *value,
            int valuelen, const struct ACL_vector *acl, int flags,
            char *path_buffer, int path_buffer_len);
    功能说明：
        创建一个同步的zookeeper节点。
    参数说明：
        zh：zookeeper的句柄，由zookeeper_init得到。
        path：节点名称，就是一个类似于文件系统写法的路径。
        value：欲存储到该节点的数据。如果不存储数据，则设置为NULL。
        valuelen：欲存储的数据的长度。如果不存储数据，则设置为-1.
        acl：初始的ACL节点，ACL不能为空。比如设置为&ZOO_OPEN_ACL_UNSAFE。
        flags：一般设置为0.ZOO_EPHEMERAL表示自动会删除当会话不存在的时候，ZOO_SEQUENCE表示在路径之后会添加一个自动累加的数据。
        path_buffer：将由新节点填充的路径值。可设置为NULL。
        path_buffer_len：path_buffer的长度。
    返回值：
        ZOK，ZNONODE，ZNODEEXISTS，ZNOAUTH，ZNOCHILDRENFOREPHEMERALS，ZBADARGUMENTS，ZINVALIDSTATE，ZMARSHALLINGERROR。
        ZOK表示操作成功，
        ZNONODE表示该节点不存在，
        ZNODEEXISTS表示节点已经存在，
        ZNOAUTH表示客户端（client）无权限，
        ZNOCHILDRENFOREPHEMERALS表示不能够创建临时（ephemeral）节点的子节点（children），
        ZINVALIDSTATE表示存在非法的参数，后两者暂略。
    */
        ret = zoo_create(zkhandle,path,"1.0",strlen("1.0"),  
                          &ZOO_OPEN_ACL_UNSAFE,0,  
                          path_buffer,bufferlen);  
        if(ret != ZOK){
            fprintf(stderr,"failed to create the path %s!\n",path);
        }else{
            printf("create path %s successfully!\n",path);
        }
    }
  
    if(ret == ZOK && g_mode == MODE_WORKER){
        // 如果是MODE_WORKER模式
        char localhost[512]={0};
        getlocalhost(localhost,sizeof(localhost));
        
        char child_path[512];
        sprintf(child_path,"%s/proc-",path);
        ret = zoo_create(zkhandle,child_path,localhost,strlen(localhost),  
                          &ZOO_OPEN_ACL_UNSAFE,ZOO_SEQUENCE|ZOO_EPHEMERAL,  
                          path_buffer,bufferlen);  
        if(ret != ZOK){
            fprintf(stderr,"failed to create the child_path %s,buffer:%s!\n",child_path,path_buffer);
        }else{
            printf("create child path %s successfully!\n",path_buffer);
        }
        choose_mater(zkhandle,path);
    }
    
    if(g_mode == MODE_MONITOR){
        show_list(zkhandle,path);
    }
    
    getchar();
    
    zookeeper_close(zkhandle); 

    return 0;
}  
