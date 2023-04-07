#pragma once
#include "SQLiteTable.h"
#include "sqlite3.h"
#include <functional>
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

struct WhereClause {
  std::string val;
  bool isAnd;
};

class SQLiteDB {
protected:
  sqlite3 *db;

private:
  std::map<std::string, WhereClause> filters;
  std::string name;

public:
  std::string BuildSelect(const SQLiteTable &table,
                          const std::vector<std::string> &columnNames);
  // addClauses as param
  std::string BuildWhere(const SQLiteTable &table);
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

  template <typename T>
  std::vector<T> GetDistinctColumnValues(const SQLiteTable &table,
                                         const std::string &columnName);
  /*template<>
      std::vector<std::string> GetDistinctColumnValues(const SQLiteTable &table,
                                         const std::string &columnName);
  template<>
      std::vector<double> GetDistinctColumnValues(const SQLiteTable &table,
                                         const std::string &columnName);
  */

  //std::vector<std::string>
      // GetDistinctColumnValues(const SQLiteTable &table,
      //                        const std::string &columnName);
      double GetDBColumn(const SQLiteTable &table,
                         const std::string &columnName);
  template <typename T, typename... Args>
  void GetDBColumns(SQLiteTable &table, std::vector<std::string> &columnNames,
                    T callback, const Args &...args);

  void AddWhereClause(const std::string &colName, const std::string &val,
                      bool isAnd = true);

  // Wrap in Begin/End to limit number of "Write" transactions
  bool Begin();
  bool End();
  void SetName(const std::string &_name);
  bool Close();
};

template <typename T, typename... Args>
void SQLiteDB::GetDBColumns(SQLiteTable &table,
                            std::vector<std::string> &columnNames, T callback,
                            const Args &...args) {
  std::string SQLSelect = BuildSelect(table, columnNames);
  SQLSelect += BuildWhere(table);
  SQLSelect += ";";
  // std::cout << "SQL Where " + SQLSelect;
  sqlite3_stmt *stmt = nullptr;
  int rc = sqlite3_prepare_v2(db, SQLSelect.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    printf("error:%s ", sqlite3_errmsg(db));
  }

  int res = sqlite3_step(stmt);

  switch (res) {
  case SQLITE_ROW:
    // printf("Duplicate row in table\n");
    for (size_t i = 0; i < columnNames.size(); i++) {
      const std::string name = columnNames[i];
      const std::string type = table.GetColumnType(name);
      std::string val = std::string(
          reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
      table.SetEle(name, val);
    }
    break;
  case SQLITE_DONE:
    break;
  }
  // extract this to function

  sqlite3_finalize(stmt);
}
