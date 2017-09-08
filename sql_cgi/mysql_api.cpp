#include "mysql_api.h"

DataBase::DataBase()
{
	_conn = mysql_init(NULL);
}

DataBase::~DataBase()
{
	mysql_close(_conn);
}

bool DataBase::Connect()
{
	if(mysql_real_connect(_conn, "127.0.0.1", "lei", "lei", "proxy", 3306, NULL, 0) == NULL)
	{
		fprintf(stderr, "Failed to connect to Database, Error: %s\n", mysql_error(_conn));
		return false;
	}

	return true;
}

bool DataBase::Insert(const char* i)
{
	if(strncasecmp(i, "insert into ", strlen("insert into ")))
	  return false;
	
	if(mysql_real_query(_conn, i, strlen(i)))
	  return false;
	else
	  return true;
}

bool DataBase::Delete(const char* d)
{
	if(strncasecmp(d, "delete from ", strlen("delete from ")))
	  return false;
	
	if(mysql_real_query(_conn, d, strlen(d)))
	  return false;
	else
	  return true;
}

bool DataBase::Update(const char* u)
{
	if(strncasecmp(u, "update ", strlen("update ")))
	  return false;

	if(mysql_real_query(_conn, u, strlen(u)))
	  return false;
	else
	  return true;
}

bool DataBase::Select(const char* s)
{
	if(strncasecmp(s, "select ", strlen("select ")))
	  return false;

	if(mysql_real_query(_conn, s, strlen(s)))
		return false;

	//获取数据集
	MYSQL_RES* result = NULL;
	if((result = mysql_store_result(_conn)) == NULL)
	  return false;

	//获取列数并打印各字段的名称
	int fields = mysql_num_fields(result);
	
	MYSQL_FIELD* field = NULL;
	for(int i = 0; i < fields; ++i)
	{
		field = mysql_fetch_field_direct(result, i);
		printf("%-20s\t", field->name);
	}
	cout << endl;

	//获取行数
	int rows = mysql_num_rows(result);
	MYSQL_ROW row = NULL; //MYSQL_ROW 是指针
	while(row = mysql_fetch_row(result))
	{
		for(int i = 0; i < fields; ++i)
		  printf("%-20s\t", row[i]);
		cout << endl;
	}

	return true;
}
