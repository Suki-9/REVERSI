#pragma once

#include <cmath>
#include <drogon/drogon.h>

/* ----- constant declaration ----- */
#define TABLE_NAME "game_instance"

/* ----- macros declaration -----*/
#define DB_RUN_WITH_VALUE( sql, ... ) (drogon::app().getDbClient("default")->execSqlSync(sql, __VA_ARGS__))
#define DB_RUN( sql ) (drogon::app().getDbClient("default")->execSqlSync(sql))

#define DB_INSERT_QUERY( ... ) std::string("INSERT INTO ") + TABLE_NAME + "(" + [](){std::vector<std::string> opv = {__VA_ARGS__}; return ArgsModifier::join(opv);}() + ")VALUES(" + []() {std::vector<std::string> opv = {__VA_ARGS__}; return ArgsModifier::fmt_gen(opv);}() + ");"
#define DB_UPDATE_QUERY( option, ... ) std::string("UPDATE ") + TABLE_NAME + " SET " + []() {std::vector<std::string> opv = {__VA_ARGS__}; return ArgsModifier::join_fmt(opv);}() + ArgsModifier::op(option) + ";"
#define DB_SELECT_QUERY( option, ... ) std::string("SELECT ") + []() {std::vector<std::string> opv = {__VA_ARGS__}; return ArgsModifier::join(opv);}() + " FROM " + TABLE_NAME + ArgsModifier::op(option) + ";"

/* ----- prototype declaration ----- */
namespace ArgsModifier {
  std::string join(std::vector<std::string> &v);
  std::string join_fmt(std::vector<std::string> &v);
  std::string fmt_gen(std::vector<std::string> &v);
  std::string op(const std::string &t);
}

namespace VarConverter {
  std::string arr2str_int(const int a[], const int &len);
  int* str2arr_int(const std::string &s, int a[]);
  std::string arr2str_string(std::string a[], const int &len);
  std::string* str2arr_string(const std::string &s, std::string a[]);
}
