#include <iostream>
#include <stdio.h>
#include <mysql.h>
#include <strings.h>
#include <string.h>
using namespace std;

class DataBase
{
public:
	DataBase();
	~DataBase();
	bool Connect();
	bool Insert(const char* i);
	bool Delete(const char* d);
	bool Update(const char* u);
	bool Select(const char* s);
private:
	MYSQL* _conn;
};
