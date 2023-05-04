#include "MysqlConn.h"
#include <json/json.h>
MysqlConn::MysqlConn()
{
	m_conn = mysql_init(nullptr); // 申请一片内存。初始化
	mysql_set_character_set(m_conn, "utf8");
}

MysqlConn::~MysqlConn()
{
	if (m_conn != nullptr)
	{
		mysql_close(m_conn); //释放连接内存
	}
	freeResult(); //释放结果集内存
}
bool MysqlConn::connect(string user, string passwd, string dbName, string ip, unsigned short port)
{
	MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr,0);
	//if (ptr != nullptr)
	//{
	//	cout << "连接成功！！！" << endl;
	//}
	return ptr != nullptr;

}

bool MysqlConn::update(string sql)
{
	if (mysql_query(m_conn, sql.c_str())) //返回的是0就是执行成功，非0失败
	{
		//cout << mysql_error(m_conn) << endl;
		return false;
	}
	return true;
}


bool MysqlConn::query(string sql)
{
	freeResult();
	if (mysql_query(m_conn, sql.c_str())) //返回的是0就是执行成功，非0失败
	{
		//cout << mysql_error(m_conn) << endl;
		//cout << "query 执行失败!" << endl;
		return false;
	}
	m_result = mysql_store_result(m_conn);
	return true;
}

bool MysqlConn::next()
{
	if (m_result != nullptr)
	{
		m_row = mysql_fetch_row(m_result);  //取出数据，按照行存储，相当于字符串数组
		if (m_row != nullptr)
		{
			return true;
		}
	}
	return false;
}

string MysqlConn::value(int index) //操作对象是  m_row，其中存在的都是字符串数组
{
	int Count = mysql_num_fields(m_result); //  有多少行数据 
	if (index >= Count || index < 0)
	{
		return string();
	}
	char* val = m_row[index];
	unsigned int length = mysql_fetch_lengths(m_result)[index]; //获取字符串长度
	return string(val,length);
}


bool MysqlConn::transaction()
{
	return mysql_autocommit(m_conn, false);
}

bool MysqlConn::commit()
{
	return mysql_commit(m_conn);
}

bool MysqlConn::rollback()
{
	return mysql_rollback(m_conn);;
}

void MysqlConn::refreshAliveTime()
{
	m_alivetime = steady_clock::now();
}

long long MysqlConn::getAliveTime()
{
	nanoseconds res = steady_clock::now() - m_alivetime; //纳秒级别
	milliseconds millsec = duration_cast<milliseconds>(res); //转换为毫秒，会损失精度
	return millsec.count(); //多少个毫秒
}

void MysqlConn::freeResult()
{
	if (m_result)
	{
		mysql_free_result(m_result);
		m_result = nullptr;
	}
}
