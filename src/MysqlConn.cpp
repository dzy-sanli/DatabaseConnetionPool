#include "MysqlConn.h"
#include <json/json.h>
MysqlConn::MysqlConn()
{
	m_conn = mysql_init(nullptr); // ����һƬ�ڴ档��ʼ��
	mysql_set_character_set(m_conn, "utf8");
}

MysqlConn::~MysqlConn()
{
	if (m_conn != nullptr)
	{
		mysql_close(m_conn); //�ͷ������ڴ�
	}
	freeResult(); //�ͷŽ�����ڴ�
}
bool MysqlConn::connect(string user, string passwd, string dbName, string ip, unsigned short port)
{
	MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr,0);
	//if (ptr != nullptr)
	//{
	//	cout << "���ӳɹ�������" << endl;
	//}
	return ptr != nullptr;

}

bool MysqlConn::update(string sql)
{
	if (mysql_query(m_conn, sql.c_str())) //���ص���0����ִ�гɹ�����0ʧ��
	{
		//cout << mysql_error(m_conn) << endl;
		return false;
	}
	return true;
}


bool MysqlConn::query(string sql)
{
	freeResult();
	if (mysql_query(m_conn, sql.c_str())) //���ص���0����ִ�гɹ�����0ʧ��
	{
		//cout << mysql_error(m_conn) << endl;
		//cout << "query ִ��ʧ��!" << endl;
		return false;
	}
	m_result = mysql_store_result(m_conn);
	return true;
}

bool MysqlConn::next()
{
	if (m_result != nullptr)
	{
		m_row = mysql_fetch_row(m_result);  //ȡ�����ݣ������д洢���൱���ַ�������
		if (m_row != nullptr)
		{
			return true;
		}
	}
	return false;
}

string MysqlConn::value(int index) //����������  m_row�����д��ڵĶ����ַ�������
{
	int Count = mysql_num_fields(m_result); //  �ж��������� 
	if (index >= Count || index < 0)
	{
		return string();
	}
	char* val = m_row[index];
	unsigned int length = mysql_fetch_lengths(m_result)[index]; //��ȡ�ַ�������
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
	nanoseconds res = steady_clock::now() - m_alivetime; //���뼶��
	milliseconds millsec = duration_cast<milliseconds>(res); //ת��Ϊ���룬����ʧ����
	return millsec.count(); //���ٸ�����
}

void MysqlConn::freeResult()
{
	if (m_result)
	{
		mysql_free_result(m_result);
		m_result = nullptr;
	}
}
