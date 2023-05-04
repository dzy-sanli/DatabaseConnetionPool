#pragma once
#include <queue>
#include <mutex>
#include<condition_variable>
#include "MysqlConn.h"
using namespace std;
class ConnectionPool   //  ����ģʽ��C++11���̰߳�ȫ��
{
public:
	ConnectionPool(const ConnectionPool& obj) = delete;  //��ֹ��������
	ConnectionPool & operator=(const ConnectionPool& obj) = delete; //��ֹ��ֵ���ƶ�����
	static ConnectionPool * getConnectPool(); // ��ȡ��̬�ֲ������̰߳�ȫ

	~ConnectionPool();





	shared_ptr<MysqlConn> getConnection();

private:
	ConnectionPool();
	 

	bool parseJsonFile();   //����������Ϣ��Json�ļ�
	void produceConnection(); //��������
	void recycleConnection(); //��������
	void addConnection();// �������ӣ�˵���������϶�
	void deleteConnection(); // ɾ�����ӣ�˵���������϶�
	
	//------------------------------------------Ҫ������json�ļ���Ա��ʼ���������ӵ�������Ϣ�Լ����ݿ����ӳص�����
	string m_ip;
	string m_user;
	string m_passwd;
	string m_dbName;
	unsigned short m_port;

	int m_minSize;
	int m_maxSize;

	int m_timeout;  //�û����ȴ�����ʱ��
	int m_maxIdleTime;  //������������ʱ��
	//------------------------------------------Ҫ������json�ļ���Ա����


	mutex m_mutexQ; //�������������Ӷ���
	condition_variable m_cond;//��������

	queue<MysqlConn *> m_connectionQ; //���ݿ����Ӷ���




};

//���������ļ�������ָ�������ݿ⣬json

