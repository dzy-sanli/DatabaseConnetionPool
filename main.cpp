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
		`phoneNum` varchar(11) NOT NULL COMMENT '�绰����(����)',
		`psword` varchar(20) NOT NULL COMMENT '����',
		`email` varchar(20) NOT NULL COMMENT '����',
		`power` int NOT NULL COMMENT 'Ȩ���ȼ�',
	
	*/
	string sql = "insert into user values('0000011','dzy','man',23);";

	//cout << "ִ��:" <<  conn.update(sql) <<endl;
	

	sql = "select * from user;";
	conn.query(sql);
	while (conn.next())
	{
		std::cout << conn.value(0) << " "
			<< conn.value(1) << " "
			<< conn.value(2) << " "
			<< conn.value(3) << endl;
	}
	std::cout << "���ӽ���" << endl;
}

// ���߳�
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
	conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //���һ����������ݿ����ӣ��������û�ͬʱʹ����ͬ���û���
	steady_clock::time_point begin = steady_clock::now();
	op1(0, 5000);
	steady_clock::time_point end = steady_clock::now();
	auto lenght = end - begin;
	std::cout << " test01 ���߳�,��ʹ�����ӳص�ʱ��" << lenght.count()<<"����  " << lenght.count() / 1000000 << "���� "<<endl;
	
#else
	MysqlConn conn;
	conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //���һ����������ݿ����ӣ��������û�ͬʱʹ����ͬ���û���
	ConnectionPool* pool = ConnectionPool::getConnectPool();
	steady_clock::time_point begin = steady_clock::now();
	op2(pool,0, 5000);
	steady_clock::time_point end = steady_clock::now();
	auto lenght = end - begin;
	std::cout << " test01 ���߳�,ʹ�����ӳص�ʱ��" << lenght.count() << "����  " << lenght.count() / 1000000 << "���� " << endl;
#endif
	return;
}

void test02()
{
#if 0
	MysqlConn conn;
	conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //���һ����������ݿ����ӣ��������û�ͬʱʹ����ͬ���û���
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
	std::cout << " test02 ���߳�,��ʹ�����ӳص�ʱ��" << lenght.count() << "����  " << lenght.count() / 1000000 << "���� " << endl;
#else
	//MysqlConn conn;
	//conn.connect("root", "123456", "connectionpooldb", "127.0.0.1"); //���һ����������ݿ����ӣ��������û�ͬʱʹ����ͬ���û���
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
	std::cout << " test01 ���߳�,ʹ�����ӳص�ʱ��" << lenght.count() << "����  " << lenght.count() / 1000000 << "���� " << endl;
#endif
}



int main()
{
	//1.���̲߳�ʹ�����ӳ�
	//test01();  //���Խ��:test01 ���߳�,��ʹ�����ӳص�ʱ��15646545100����  15646����
	//2.���߳�ʹ�����ӳ�
	//test01();  //���Խ��:test01 ���߳�,ʹ�����ӳص�ʱ��5986835000����  5986����
	//3.���̲߳�ʹ�����ӳ�
	test02();  //���Խ����test02 ���߳�,��ʹ�����ӳص�ʱ��3896139500����  3896����
	////4.���߳�ʹ�����ӳ�
	//test02(); //���Խ����test01 ���߳�,ʹ�����ӳص�ʱ��2452881900����  2452����

	//
	//testquery();
	//system("pause");


	return 0;
}
