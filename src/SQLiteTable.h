#pragma once
#include<string>
#include<map>

namespace SQL {
	const std::string INT = "INT";
	const std::string REAL = "REAL";
	const std::string KEY = "KEY";
	const std::string TEXT = "TEXT";
}
struct DBColumn {
	std::string type;
	std::string val;
};

class SQLiteTable {
protected:
	std::map<std::string, DBColumn> table;
	std::string tableName;
	mutable std::string keyStr;
	mutable bool keyStrUpdated = false;

public:
	SQLiteTable(const std::string &name);
	SQLiteTable();
	//Add column schema to table
	void InsertEle(const std::string& name, const std::string& type, const std::string &defVal = " ");
	//Set the value of a column, must do before every write
	void SetEle(const std::string& key, const std::string val);
	void SetName(const std::string& name);
  const std::string GetColumnType(const std::string &colName);
	const std::string& GetName() const;
	const std::string& GetKeyStr() const;
	const std::map<std::string, DBColumn>& GetTable() const;
	const std::string& GetVal(const std::string &key) ;
};

//SQL table specificall for map data
class MavTable :public SQLiteTable {
private:
public:
	void Init();
	void SetValues(double time, const std::string& resp, size_t cell, double val);
};

//SQL table for bar graph viewer
class SliceTable :public SQLiteTable {
private:
public:
	void Init();
	void SetValues(double time, const std::string& resp, double val);
};
