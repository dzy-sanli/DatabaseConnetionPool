#include "ConnectionPool.h"
#include "ConnectionPool.h"
#include <json/json.h>
#include <fstream>
#include <thread>
using namespace Json;

ConnectionPool* ConnectionPool::getConnectPool()
{
    static ConnectionPool pool;
    return &pool;
}

ConnectionPool::~ConnectionPool()
{
    while (!m_connectionQ.empty())
    {
        MysqlConn* conn = m_connectionQ.front();
        m_connectionQ.pop();
        delete conn;
    }
}

bool ConnectionPool::parseJsonFile()
{
    ifstream ifs("dbconf.json");
    Reader rd;
    Value root;
    rd.parse(ifs, root);
    if (root.isObject())
    {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();

        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();
        return true;
    }
    return false;
}

void ConnectionPool::produceConnection() 
{
    while (true)
    {
        unique_lock<mutex> locker(m_mutexQ);//���������� �����������������������
        while (m_connectionQ.size() >= m_minSize)
        {
            m_cond.wait(locker); //���˾�����
        }
        addConnection();
        m_cond.notify_all();  //��������������������
    }
}

void ConnectionPool::recycleConnection() //ɱ�����Ӷ����У�����������ʱ������m_maxIdleTime �����ӣ�ʵ���ϲ���ɱ�����޸ĺ��ַŵ���β��
{
    while (true)
    {
        //this_thread::sleep_for(chrono::seconds(1));  //ÿ��һ�룬���һ��,duration����
        this_thread::sleep_for(chrono::microseconds(500));  //ÿ500���룬���һ��

        unique_lock<mutex> locker(m_mutexQ);//���������� ������
        while (m_connectionQ.size() > m_minSize)  //ֻ�е����ӳ����Ӹ���������С���Ż��ͷ�
        {
            MysqlConn* conn = m_connectionQ.front(); //ȡ��һ������Ҫ��Ҫɾ��
            if (conn->getAliveTime() >= m_maxIdleTime) //����ɾ��
            {
                m_connectionQ.pop();
                delete conn;
            }
            else
            {
                break;
            }
            m_cond.wait(locker);
        }
        addConnection();
    }
}

void ConnectionPool::addConnection()
{
    MysqlConn* conn = new MysqlConn;
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);
    conn->refreshAliveTime(); //һ���Ӿ���Ҫˢ��
    m_connectionQ.push(conn);
}

shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connectionQ.empty())
    {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))  //�������Ϊʱ������ͷ���״̬cv_status::timeout
        {
            if (m_connectionQ.empty())
            {
                continue;
            }
        }
    }
    /*
        ���������Ѿ�ȡ��һ�����Ӳ�ʹ���ˣ��ǲ��ǻ��û�������
        1. �����ṩһ���ӿڣ���ʹ����Ϻ��ٴβ������
        2.ʹ������ָ�룬shared����
    
    */

    shared_ptr<MysqlConn> connptr(m_connectionQ.front(), [this](MysqlConn* conn) {
        conn->refreshAliveTime(); //���ӻ��������͸�����ʼʱ���
        lock_guard<mutex> locker(m_mutexQ);
        m_connectionQ.push(conn);//������Դ����Ҫ���� 
        });//ָ��ɾ����
    m_connectionQ.pop();
    m_cond.notify_all();// �����������˻���������
    return connptr;
}

ConnectionPool::ConnectionPool()
{
    if (!parseJsonFile())
    {
        return;
    }
    for (int i = 0; i < m_minSize; ++i)  //��ʼ�� m_minSize�����ݿ�����
    {
        addConnection();
    }
    thread producer(&ConnectionPool::produceConnection, this); // ��������Ƿ��ã��Ǿ�̬��Ա����
    thread recycler(&ConnectionPool::recycleConnection, this);  //��������Ƿ����
    //���������߳����룬����ϵؽ��м��
    producer.detach();  
    recycler.detach();
}
