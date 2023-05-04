#pragma once
#include <iostream>
#include <mysql.h>
#include <chrono>

using namespace std;
using namespace chrono;
class MysqlConn
{
public:
	//��ʼ�����ӣ��ŵ����캯������
	MysqlConn();
	//�ͷ�����
	~MysqlConn();

	//�������ݿ�,�˿�Ĭ����3306
	bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306);
	//�������ݿ⣬��Ҫִ����ɾ�Ĳ���
	bool update(string sql);
	//��ѯ���ݿ⣬��ѯsql���
	bool query(string sql);
	//����������е��ֶ�ֵ�������β�ѯ�Ľ��������m_result�У�nextÿ��ȡһ������
	bool next();
	//�õ�������е��ֶΣ�next()�õ�����һ��
	string value(int index);


	//�������
	bool transaction();
	//�ύ����
	bool commit();
	//����ع�
	bool rollback();


	//��ʼ�����Ӵ�����ʱ���
	void refreshAliveTime();
	//�������Ӵ�����ʱ��
	long long getAliveTime();

private:
	void freeResult(); //�ͷŲ�ѯ������

	MYSQL* m_conn =nullptr;  //���ӵ��ڴ�
	MYSQL_RES* m_result = nullptr;  //������ڴ�
	MYSQL_ROW m_row = nullptr;  //��ʵ������Ǵ�ŵ�ָ�룬ָ�����m_result���ڴ��ַ�����Բ���Ҫ�ͷţ���Ϊ�ͷ���m_result

	steady_clock::time_point m_alivetime; //��ʼʱ���


};

