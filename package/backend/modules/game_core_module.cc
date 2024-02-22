#include <drogon/drogon.h>

#include "game_core_module.h"

/* ----- use modules include -----*/
#include "db_module.h"
#include "logger_module.h"


const InstanceId create_instance(
  const std::string& pass,
  const PlayerId& player
) {
  try {
    std::string p[3]{player, "", create_id()};
    DB_RUN_WITH_VALUE(
      DB_INSERT_QUERY("create_at", "player_ids", /*"board",*/ "pass", "instance_id"),
      getTimeStamp(), VarConverter::arr2str_string(p, 2), /*VarConverter::arr2str_int(b, 64),*/ pass, p[2]
    );
    return p[2];

  } catch (const drogon::orm::DrogonDbException &e) {
    ERR_LOGGER("GameCoreModule", e.base().what());
    return "";
  };
}

const Instance* search_Instance(
  Instance& instance,
  const std::string& pass,
  const InstanceId& instance_id
) {
  if (pass != "") {
    auto result = DB_RUN(DB_SELECT_QUERY("pass = '" + pass + "'", "pass", "is_matched", "player_ids", "instance_id"));
    if (result.size()) {
      VarConverter::str2arr_string(result[0]["player_ids"].as<std::string>(), instance.player_ids);
      instance.instance_id = result[0]["instance_id"].as<std::string>();
      instance.is_matched = result[0]["is_matched"].as<bool>();
      instance.pass = result[0]["pass"].as<std::string>();
    }
  }

  if(instance_id != "") {
    auto result = DB_RUN(DB_SELECT_QUERY("instance_id = '" + instance_id + "'", "pass", "is_matched", "player_ids", "instance_id"));
    if (result.size()) {
      VarConverter::str2arr_string(result[0]["player_ids"].as<std::string>(), instance.player_ids);
      instance.instance_id = result[0]["instance_id"].as<std::string>();
      instance.is_matched = result[0]["is_matched"].as<bool>();
      instance.pass = result[0]["pass"].as<std::string>();
    }
  }
  return &instance;
}

const std::string create_id() {
  std::string i;
  int x;
  for(long int t = getTimeStamp(); t; t /= 36) {
    x = t % 36;
    i = (char)(x < 10 ? '0' + x : 'A' + x - 10) + i;
  }
  return i += (char)('A' + rand() % 26), i += (char)('A' + rand() % 26);
}

const bool verify_pass(const std::string &pass) {
  return (pass.length() == PASS_LENGTH && !DB_RUN(DB_SELECT_QUERY("pass = '" + pass + "'", "pass")).size());
}

const bool verify_instance_id(const std::string& instance_id) {
  auto rows = DB_RUN(DB_SELECT_QUERY("instance_id = '" + instance_id + "'", "is_matched"));
  return (rows.size() && rows[0]["is_matched"].as<bool>());
}

long int getTimeStamp() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

const int update_board(int &p, std::shared_ptr<InstanceState> i, PlayerId &n) {
  int c = i->player_ids[0] == n ? 1 : 2;
  int t_ = -8, u_ = 8, l_ = -1, r_ = 1;
  int t = 0;

  if (i->board[p]) return 0;
  for (const int &put: verify_put_len(p, c, t_,      i->board)) t += i->board[put] = c;
  for (const int &put: verify_put_len(p, c, t_ + l_, i->board)) t += i->board[put] = c;
  for (const int &put: verify_put_len(p, c, t_ + r_, i->board)) t += i->board[put] = c;
  for (const int &put: verify_put_len(p, c, u_,      i->board)) t += i->board[put] = c;
  for (const int &put: verify_put_len(p, c, u_ + l_, i->board)) t += i->board[put] = c;
  for (const int &put: verify_put_len(p, c, u_ + r_, i->board)) t += i->board[put] = c;
  for (const int &put: verify_put_len(p, c, l_,      i->board)) t += i->board[put] = c;
  for (const int &put: verify_put_len(p, c, r_,      i->board)) t += i->board[put] = c;
  if (t) i->board[p] = c;

  return t ? t / c : 0;
}

const bool verify_put(int &p, std::shared_ptr<InstanceState> i, PlayerId &n) {
  int c = i->player_ids[0] == n ? 1 : 2;
  int t_ = -8, u_ = 8, l_ = -1, r_ = 1;
  int t = 0;

  if (i->board[p]) return false;
  for (const int &put: verify_put_len(p, c, t_,      i->board)) t++;
  for (const int &put: verify_put_len(p, c, t_ + l_, i->board)) t++;
  for (const int &put: verify_put_len(p, c, t_ + r_, i->board)) t++;
  for (const int &put: verify_put_len(p, c, u_,      i->board)) t++;
  for (const int &put: verify_put_len(p, c, u_ + l_, i->board)) t++;
  for (const int &put: verify_put_len(p, c, u_ + r_, i->board)) t++;
  for (const int &put: verify_put_len(p, c, l_,      i->board)) t++;
  for (const int &put: verify_put_len(p, c, r_,      i->board)) t++;

  return (bool)t;
}

std::vector<int> verify_put_len(int &p, int &c, int m, int *b) {
  std::vector<int> v;
  int cursor = p + m;
  while (cursor < 64 && 0 <= cursor && b[cursor] == (c - 1 ? 1 : 2)) {
    v.push_back(cursor);
    cursor += m;
  }
  if (!(cursor - p + m && b[cursor] == c)) v.clear();
  return std::move(v);
}

void init_board(int *b) {
  b[27] = b[36] = 1;
  b[28] = b[35] = 2;
}
