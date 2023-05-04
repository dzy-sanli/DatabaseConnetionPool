#pragma once
#include <queue>
#include <mutex>
#include<condition_variable>
#include "MysqlConn.h"
using namespace std;
class ConnectionPool   //  懒汉模式，C++11是线程安全的
{
public:
	ConnectionPool(const ConnectionPool& obj) = delete;  //防止拷贝构造
	ConnectionPool & operator=(const ConnectionPool& obj) = delete; //防止赋值，移动可以
	static ConnectionPool * getConnectPool(); // 获取静态局部对象，线程安全

	~ConnectionPool();





	shared_ptr<MysqlConn> getConnection();

private:
	ConnectionPool();
	 

	bool parseJsonFile();   //解析连接信息的Json文件
	void produceConnection(); //生产连接
	void recycleConnection(); //销毁连接
	void addConnection();// 增加连接，说明连接数较多
	void deleteConnection(); // 删除连接，说明连接数较多
	
	//------------------------------------------要解析的json文件成员开始，数据连接的属性信息以及数据库连接池的属性
	string m_ip;
	string m_user;
	string m_passwd;
	string m_dbName;
	unsigned short m_port;

	int m_minSize;
	int m_maxSize;

	int m_timeout;  //用户最大等待连接时长
	int m_maxIdleTime;  //空闲连接最长存活时间
	//------------------------------------------要解析的json文件成员结束


	mutex m_mutexQ; //互斥锁，锁连接队列
	condition_variable m_cond;//条件变量

	queue<MysqlConn *> m_connectionQ; //数据库连接队列




};

//加载配置文件，连接指定的数据库，json

