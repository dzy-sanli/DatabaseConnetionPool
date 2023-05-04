#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<memory>
#include"MysqlConn.h"
#include"ConnectionPool.h"
using namespace std;

void testquery()
{
	MysqlConn conn;
	conn.connect("root", "123456", "connectionpooldb", "127.0.0.1");
	/*
		`phoneNum` varchar(11) NOT NULL COMMENT '电话号码(主键)',
		`psword` varchar(20) NOT NULL COMMENT '密码',
		`email` varchar(20) NOT NULL COMMENT '邮箱',
		`power` int NOT NULL COMMENT '权力等级',
	
	*/
	string sql = "insert into user values('0000011','dzy','man',23);";

	//cout << "执行:" <<  conn.update(sql) <<endl;
	

	sql = "select * from user;";
	conn.query(sql);
	while (conn.next())
	{
		std::cout << conn.value(0) << " "
			<< conn.value(1) << " "
			<< conn.value(2) << " "
			<< conn.value(3) << endl;
	}
	std::cout << "连接结束" << endl;
}

// 单线程
void op1(int begin, int end)
{
	
	for (int i = begin; i < end; ++i)
	{
		MysqlConn conn;
		char sqlUpdate[1024] = "";
		conn.connect("root", "123456", "connectionpooldb", "127.0.0.1");
		sprintf(sqlUpdate, "insert into user values(%d,'dzy','man',23);", i);
		conn.update(sqlUpdate);
		
	}
	//string sqlQuery = "select * from user;";
	//conn.query(sqlQuery);

}
void op2(ConnectionPool * pool,int begin, int end)
{
	
	for (int i = begin; i < end; ++i)
	{
		shared_ptr<MysqlConn> conn = pool->getConnection();
		char sqlUpdate[1024] = "";
		conn->connect("root", "123456", "connectionpooldb", "127.0.0.1");
		sprintf(sqlUpdate, "insert into user values(%d,'dzy','man',23);", i);
		conn->update(sqlUpdate);

	}
	//string sqlQuery = "select * from user;";
	//conn.query(sqlQuery);

}

void test01()
{
#if 1
	MysqlConn conn;
	conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //添加一个额外的数据库连接，避免多个用户同时使用相同的用户名
	steady_clock::time_point begin = steady_clock::now();
	op1(0, 5000);
	steady_clock::time_point end = steady_clock::now();
	auto lenght = end - begin;
	std::cout << " test01 单线程,不使用连接池的时间" << lenght.count()<<"纳秒  " << lenght.count() / 1000000 << "毫秒 "<<endl;
	
#else
	MysqlConn conn;
	conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //添加一个额外的数据库连接，避免多个用户同时使用相同的用户名
	ConnectionPool* pool = ConnectionPool::getConnectPool();
	steady_clock::time_point begin = steady_clock::now();
	op2(pool,0, 5000);
	steady_clock::time_point end = steady_clock::now();
	auto lenght = end - begin;
	std::cout << " test01 单线程,使用连接池的时间" << lenght.count() << "纳秒  " << lenght.count() / 1000000 << "毫秒 " << endl;
#endif
	return;
}

void test02()
{
#if 0
	MysqlConn conn;
	conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //添加一个额外的数据库连接，避免多个用户同时使用相同的用户名
	steady_clock::time_point begin = steady_clock::now();
	thread t1(op1, 0, 1000);
	thread t2(op1, 1000, 2000);
	thread t3(op1, 2000, 3000);
	thread t4(op1, 3000, 4000);
	thread t5(op1, 4000, 5000);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	steady_clock::time_point end = steady_clock::now();
	auto lenght = end - begin;
	std::cout << " test02 多线程,不使用连接池的时间" << lenght.count() << "纳秒  " << lenght.count() / 1000000 << "毫秒 " << endl;
#else
	//MysqlConn conn;
	//conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //添加一个额外的数据库连接，避免多个用户同时使用相同的用户名
	ConnectionPool* pool = ConnectionPool::getConnectPool();
	steady_clock::time_point begin = steady_clock::now();
	//op2(pool, 1000, 2000);
	thread t1(op2, pool, 0,1000);
	thread t2(op2, pool,1000, 2000);
	thread t3(op2, pool,2000, 3000);
	thread t4(op2, pool,3000, 4000);
	thread t5(op2, pool,4000, 5000);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	steady_clock::time_point end = steady_clock::now();
	auto lenght = end - begin;
	std::cout << " test01 多线程,使用连接池的时间" << lenght.count() << "纳秒  " << lenght.count() / 1000000 << "毫秒 " << endl;
#endif
}



int main()
{
	//1.单线程不使用连接池
	//test01();  //测试结果:test01 单线程,不使用连接池的时间15646545100纳秒  15646毫秒
	//2.单线程使用连接池
	//test01();  //测试结果:test01 单线程,使用连接池的时间5986835000纳秒  5986毫秒
	//3.多线程不使用连接池
	test02();  //测试结果：test02 多线程,不使用连接池的时间3896139500纳秒  3896毫秒
	////4.多线程使用连接池
	//test02(); //测试结果：test01 多线程,使用连接池的时间2452881900纳秒  2452毫秒

	//
	//testquery();
	//system("pause");


	return 0;
}
