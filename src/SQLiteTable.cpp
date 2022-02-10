#include "SQLiteTable.h"

SQLiteTable::SQLiteTable(const std::string &name) : tableName(name) {}

SQLiteTable::SQLiteTable() {}

void SQLiteTable::InsertEle(const std::string &name, const std::string &type,
                            const std::string &defVal) {
  table.insert(
      std::map<std::string, DBColumn>::value_type(name, {type, defVal}));
  keyStrUpdated = false;
}

void SQLiteTable::SetEle(const std::string &key, const std::string val) {
  table[key].val = val;
}

void SQLiteTable::SetColNum(const std::string &key,size_t colNum)
{
    table.at(key).colNum=colNum;
}
void SQLiteTable::SetName(const std::string &name) { tableName = name; }
const std::string SQLiteTable::GetColumnType(const std::string &colName) const {
  return table.at(colName).type;
}

const size_t SQLiteTable::GetColumNum(const std::string &colName) const{
 return table.at(colName).colNum;
}
std::vector<std::string> SQLiteTable::GetColumnNames() {
  std::vector<std::string> names;
  for (const std::pair<std::string, DBColumn> &col : table) {
    names.push_back(col.first);
  }
  return names;
}
const std::string &SQLiteTable::GetName() const { return this->tableName; }
const std::string &SQLiteTable::GetKeyStr() const {
  if (!keyStrUpdated) {
    keyStr = "";
    bool isFirst = true;
    for (const std::pair<std::string, DBColumn> &ele : table) {
      if (isFirst) {
        isFirst = false;
      } else {
        keyStr += ',';
      }
      keyStr += ele.first;
    }
  }
  return keyStr;
}
const std::map<std::string, DBColumn> &SQLiteTable::GetTable() const {
  return this->table;
}

const std::string &SQLiteTable::GetVal(const std::string &key) {
  // revisit why cant make function const type
  return table[key].val;
}

void MavTable::Init() {
  InsertEle("Time", SQL::TEXT);
  InsertEle("Response", SQL::TEXT);
  InsertEle("Cell", SQL::INT);
  InsertEle("Val", SQL::REAL);
}

// Note can speed up by not storing values in map at all and just writing them
// to DB
void MavTable::SetValues(double time, const std::string &resp, size_t cell,
                         double val) {
  SetEle("Time", std::to_string(time));
  SetEle("Response", resp);
  SetEle("Cell", std::to_string(cell));
  SetEle("Val", std::to_string(val));
}

void SliceTable::Init() {
  InsertEle("Time", SQL::TEXT);
  InsertEle("Response", SQL::TEXT);
  InsertEle("Val", SQL::REAL);
}

void SliceTable::SetValues(double time, const std::string &resp, double val) {
  SetEle("Time", std::to_string(time));
  SetEle("Response", resp);
  SetEle("Val", std::to_string(val));
}
