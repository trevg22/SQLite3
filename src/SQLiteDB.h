#pragma once
#include "SQLiteTable.h"
#include "sqlite3.h"
#include <map>
#include <string>
#include <vector>
namespace SQLSchema {
const std::string id = "cid";
const std::string name = "name";
const std::string type = "type";
const std::string notnull = "notnul";
const std::string def_val = "dflt_value";
const std::string primary_key = "pk";
} // namespace SQLSchema

struct WhereClause{
  std::string val;
  bool isAnd;
};

class SQLiteDB {
  private:
    std::map<std::string,WhereClause> filters;
    std::string name;
    sqlite3 *db;

  public:
    bool Open();
    // Add table schema to database
    bool AddTable(const SQLiteTable &table);
    // Insert a row of table values
    bool InsertTableRow(const SQLiteTable &table);
    // Execute a SQL statment
    bool Exec(const std::string &str);
    // Remove table from database
    bool DeleteTable(const SQLiteTable &table);

    bool UpdateTableSchema(SQLiteTable &table);


    std::vector<std::string>
    GetDistinctColumnValues(const SQLiteTable &table,
                            const std::string &columnName);
    double GetDBColum(SQLiteTable &table,const std::string &columnName);

    void AddWhereClause(const std::string &colName,const std::string &val,bool isAnd=true);
    // Wrap in Begin/End to limit number of "Write" transactions
    bool Begin();
    bool End();
    void SetName(const std::string &_name);
    bool Close();
};
