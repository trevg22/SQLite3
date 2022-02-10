#include "SQLiteDB.h"
#include <iostream>

bool SQLiteDB::Open() {
    int rc = sqlite3_open(name.c_str(), &db);
    return rc == SQLITE_OK ? true : false;
}

bool SQLiteDB::AddTable(const SQLiteTable &table) {
    const std::string SQLCreate =
        "CREATE TABLE IF NOT EXISTS " + table.GetName() + " (";

    std::string SQLCmd;

    bool isFirst = true;
    // Note:: implement a stream in table class so dont have to expose private
    // table?????
    for (const std::pair<std::string, DBColumn> &ele : table.GetTable()) {
        if (isFirst) {
            isFirst = false;
        } else {
            SQLCmd += ',';
        }
        SQLCmd += ele.first + " " + ele.second.type + " " + ele.second.val;
    }
    SQLCmd += "); ";
    SQLCmd = SQLCreate + SQLCmd;
    return Exec(SQLCmd);
}
// Refactor later to not have to create string each time
bool SQLiteDB::InsertTableRow(const SQLiteTable &table) {
    std::string values;
    const std::string SQLInsert =
        "INSERT INTO " + table.GetName() + " (" + table.GetKeyStr() + ")";

    bool isFirst = true;
    for (const std::pair<std::string, DBColumn> &ele : table.GetTable()) {
        if (isFirst) {
            isFirst = false;
        } else {
            values += ',';
        }
        // replace with sqlite3_bind to handle quoting of text types
        if (ele.second.type == SQL::TEXT) {
            values += "'" + ele.second.val + "'";
        } else {
            values += ele.second.val;
        }
    }

    std::string SQLCmd = SQLInsert + " VALUES (" + values + ");";
    return Exec(SQLCmd);
}

bool SQLiteDB::Exec(const std::string &str) {
    char *errorMsg = nullptr;
    int err = sqlite3_exec(this->db, str.c_str(), nullptr, nullptr, &errorMsg);
    if (err != SQLITE_OK) {
        std::cout << errorMsg;
        sqlite3_free(errorMsg);
        return false;
    }
    return true;
}

bool SQLiteDB::DeleteTable(const SQLiteTable &table) {
    const std::string SQLDrop = "DROP TABLE IF EXISTS " + table.GetName() + ';';
    return Exec(SQLDrop);
}

// add move sematic things
// Update table to reflect an existing schema
bool SQLiteDB::UpdateTableSchema(SQLiteTable &table) {
    const std::string SQLTableInfo =
        "pragma table_info(" + table.GetName() + ")";
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, SQLTableInfo.c_str(), -1, &stmt, nullptr);
    // replace with check_ok()
    if (rc != SQLITE_OK) {
        printf("error:%s ", sqlite3_errmsg(db));
        return false;
    }

    size_t rowCount=0;
    // one row at a time
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int numCols = sqlite3_column_count(stmt);
        std::string name;
        std::string type;
        for (int col = 0; col < numCols; col++) {
            if (sqlite3_column_name(stmt, col) == SQLSchema::name) {
                name = std::string(reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, col)));
            } else if (sqlite3_column_name(stmt, col) == SQLSchema::type) {
                type = std::string(reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, col)));
            }
        }
        if (!name.empty() && !type.empty()) {
            table.InsertEle(name, type);
            table.SetColNum(name,rowCount);
            rowCount++;
            //std::cout <<"name"<<name<<"type"<<type<<"\n";
        } else {
            // schema incomplete
            return false;
        }
    }
    sqlite3_finalize(stmt);
    return true;
}

//rename to colNames
std::string SQLiteDB::BuildSelect(const SQLiteTable &table,const std::vector<std::string> &columNames)
{
    std::string SQLSelect="SELECT "; 
    bool first=true;
    for(const std::string &col:columNames)
    {
        if(first)
        {
            first=false;
        }
        else{
            SQLSelect+=",";
        }
        SQLSelect+=col;
    }
    SQLSelect+=" FROM "+ table.GetName();
    return SQLSelect;
}

std::string SQLiteDB::BuildWhere(const SQLiteTable &table)
{
    bool isFirst=true;
    std::string SQLWhere;
    for (const std::pair<std::string,WhereClause> clause:filters) {
        
        if(isFirst)
        {
          isFirst=false;
          SQLWhere+=" Where ";
        }
        else{
          if(clause.second.isAnd==true)
          {
            SQLWhere+=" AND ";
          }
          else{
            SQLWhere+=" OR ";
          }
        }

        SQLWhere +=clause.first+ "=";
        const std::string colType = table.GetColumnType(clause.first);
        //std::cout<<"name "<<clause.first<<"type "<<colType<<"\n";
        if (colType == SQL::TEXT) {
            SQLWhere += "\"" + clause.second.val + "\"";
        } else {
            SQLWhere += clause.second.val;
        }
    }
    return SQLWhere;
}
// needs to be changed to respect true type of the column
std::vector<std::string>
SQLiteDB::GetDistinctColumnValues(const SQLiteTable &table,
                                  const std::string &columnName) {
    const std::string SQLSelectDist =
        "SELECT DISTINCT " + columnName + " FROM " + table.GetName() + BuildWhere(table);
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, SQLSelectDist.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        printf("error:%s ", sqlite3_errmsg(db));
    }

    std::vector<std::string> values;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        values.push_back(
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return values;
}
double SQLiteDB::GetDBColumn(const SQLiteTable &table, const std::string &columnName) {
    std::string SQLSelect = "SELECT " + columnName + " FROM " + table.GetName()+" ";
    bool isFirst=true;
    for (const std::pair<std::string,WhereClause> clause:filters) {
        
        if(isFirst)
        {
          isFirst=false;
          SQLSelect+=" Where ";
        }
        else{
          if(clause.second.isAnd==true)
          {
            SQLSelect+=" AND ";
          }
          else{
            SQLSelect+=" OR ";
          }
        }

        SQLSelect +=clause.first+ "=";
        const std::string colType = table.GetColumnType(clause.first);
        //std::cout<<"name "<<clause.first<<"type "<<colType<<"\n";
        if (colType == SQL::TEXT) {
            SQLSelect += "\"" + clause.second.val + "\"";
        } else {
            SQLSelect += clause.second.val;
        }
    }
    SQLSelect += ";";
    //std::cout << "SQL Where " + SQLSelect;
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, SQLSelect.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        printf("error:%s ", sqlite3_errmsg(db));
    }

    int res = sqlite3_step(stmt);

    switch (res) {
    case SQLITE_ROW:
        //printf("Duplicate row in table\n");
        break;
    case SQLITE_DONE:
        break;
    }
    double val = sqlite3_column_double(stmt, 0);

    sqlite3_finalize(stmt);
    return val;
}

void SQLiteDB::AddWhereClause(const std::string &colName,
                              const std::string &val, bool isAnd) {
    filters[colName] = {val, isAnd};
}

bool SQLiteDB::Begin() { return Exec("BEGIN TRANSACTION;"); }
bool SQLiteDB::End() { return Exec("END TRANSACTION;"); }
void SQLiteDB::SetName(const std::string &_name) { name = _name; }
bool SQLiteDB::Close() { return sqlite3_close(db); }
