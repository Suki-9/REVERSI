#include "db_module.h"

std::string ArgsModifier::join(std::vector<std::string> &v) {
  for (char i = 1; i < v.size(); v[0] += "," + v[i++]){}
  return v[0];
}

std::string ArgsModifier::join_fmt(std::vector<std::string> &v) {
  v[0] += "=$1";
  for (char i = 1; i < v.size(); v[0] += "," + v[i++] + "=$" + std::to_string(i+1)) {}
  return v[0];
}

std::string ArgsModifier::fmt_gen(std::vector<std::string> &v) {
  v[0] = "$1";
  for (char i = 2; i <= v.size(); v[0] += ",$" + std::to_string(i++)){}
  return v[0];
}

std::string ArgsModifier::op(const std::string &t) {
  return ("" - t[0]) ? " WHERE " + t : "";
}

std::string VarConverter::arr2str_int(const int a[], const int &len) {
  std::string s = "{" + std::to_string(a[0]);
  for (char i = 1; i - len; s += "," + std::to_string(a[i++])) {}
  return s + "}";
}

int* VarConverter::str2arr_int(const std::string &s, int a[]) {
  int i = 0, j = 0;
  for (char c:s) if (c-'{' && c-'}' && c-'\'') c == ',' ? i++,j=0 : a[i] += (c - '0') * std::pow(10, j);
  return a;
}

std::string VarConverter::arr2str_string(std::string a[], const int &len) {
  a[0] = "{'" + a[0];
  for (char i = 1; i - len; a[0] += "','" + a[i++]){}
  return a[0] + "'}";
}

std::string* VarConverter::str2arr_string(const std::string &s, std::string a[]) {
  int i = 0;
  for (char c:s) if (c-'{' && c-'}' && c-'\'') c == ',' ? i++ : (a[i] += c,0);
  return a;
}
