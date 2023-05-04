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
        unique_lock<mutex> locker(m_mutexQ);//条件变量绑定 队列锁，解锁在作用域结束
        while (m_connectionQ.size() >= m_minSize)
        {
            m_cond.wait(locker); //够了就阻塞
        }
        addConnection();
        m_cond.notify_all();  //生产者生产后唤醒消费者
    }
}

void ConnectionPool::recycleConnection() //杀死连接队列中，多余且连接时长大于m_maxIdleTime 的连接（实际上不是杀死，修改后又放到队尾）
{
    while (true)
    {
        //this_thread::sleep_for(chrono::seconds(1));  //每个一秒，检查一次,duration类型
        this_thread::sleep_for(chrono::microseconds(500));  //每500毫秒，检查一次

        unique_lock<mutex> locker(m_mutexQ);//条件变量绑定 队列锁
        while (m_connectionQ.size() > m_minSize)  //只有当连接池连接个数大于最小，才会释放
        {
            MysqlConn* conn = m_connectionQ.front(); //取第一个看看要不要删除
            if (conn->getAliveTime() >= m_maxIdleTime) //可以删除
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
    conn->refreshAliveTime(); //一连接就需要刷新
    m_connectionQ.push(conn);
}

shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connectionQ.empty())
    {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))  //如果是因为时长到达就返回状态cv_status::timeout
        {
            if (m_connectionQ.empty())
            {
                continue;
            }
        }
    }
    /*
        假如现在已经取出一个连接并使用了，是不是还得换回来，
        1. 可以提供一个接口，在使用完毕后再次插入队列
        2.使用智能指针，shared管理，
    
    */

    shared_ptr<MysqlConn> connptr(m_connectionQ.front(), [this](MysqlConn* conn) {
        conn->refreshAliveTime(); //连接还回来，就跟新起始时间点
        lock_guard<mutex> locker(m_mutexQ);
        m_connectionQ.push(conn);//共享资源，需要加锁 
        });//指定删除器
    m_connectionQ.pop();
    m_cond.notify_all();// 消费者消费了唤醒生产者
    return connptr;
}

ConnectionPool::ConnectionPool()
{
    if (!parseJsonFile())
    {
        return;
    }
    for (int i = 0; i < m_minSize; ++i)  //初始化 m_minSize个数据库连接
    {
        addConnection();
    }
    thread producer(&ConnectionPool::produceConnection, this); // 检测连接是否够用，非静态成员函数
    thread recycler(&ConnectionPool::recycleConnection, this);  //检测连接是否多余
    //让这两个线程脱离，并间断地进行检测
    producer.detach();  
    recycler.detach();
}
