#pragma once
#include <iostream>
#include <mysql.h>
#include <chrono>

using namespace std;
using namespace chrono;
class MysqlConn
{
public:
	//初始化连接，放到构造函数即可
	MysqlConn();
	//释放连接
	~MysqlConn();

	//连接数据库,端口默认是3306
	bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306);
	//更新数据库，主要执行增删改操作
	bool update(string sql);
	//查询数据库，查询sql语句
	bool query(string sql);
	//遍历结果集中的字段值，将单次查询的结果集放在m_result中，next每次取一行数据
	bool next();
	//得到结果集中的字段，next()得到的是一个
	string value(int index);


	//事物操作
	bool transaction();
	//提交事务
	bool commit();
	//事务回滚
	bool rollback();


	//初始化连接创建的时间点
	void refreshAliveTime();
	//计算连接存活的总时长
	long long getAliveTime();

private:
	void freeResult(); //释放查询的数据

	MYSQL* m_conn =nullptr;  //连接的内存
	MYSQL_RES* m_result = nullptr;  //结果集内存
	MYSQL_ROW m_row = nullptr;  //其实这个就是存放的指针，指向的是m_result的内存地址，所以不需要释放，因为释放了m_result

	steady_clock::time_point m_alivetime; //起始时间点


};

