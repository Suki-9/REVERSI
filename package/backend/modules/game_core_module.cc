#include "game_core_module.h"


const InstanceId InstanceOperator::create(
  const std::string& pass,
  const PlayerId& player
) {
  try {
    std::string p[3]{player, create_id(), create_id()};
    DB_RUN_WITH_VALUE(
      DB_INSERT_QUERY("create_at", "player_ids", "pass", "instance_id"),
      getTimeStamp(), VarConverter::arr2str_string(p, 2), pass, p[2]
    );
    return p[2];
  } catch (const drogon::orm::DrogonDbException &e) {
    ERR_LOGGER("GameCoreModule", e.base().what());
    return "";
  }
}

const Instance* InstanceOperator::search(
  Instance& instance,
  const std::string& pass,
  const InstanceId& instance_id
) {
  if (pass != "") {
    auto result = DB_RUN(DB_SELECT_QUERY("pass = '" + pass + "' AND is_matched = false", "pass", "is_matched", "player_ids", "instance_id"));
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

const bool Verify::pass(const std::string &pass) {
  return (pass.length() == PASS_LENGTH && pass == "0000" ? true : !DB_RUN(DB_SELECT_QUERY("pass = '" + pass + "'", "pass")).size());
}

const bool Verify::instance_id(const InstanceId &i) {
  auto rows = DB_RUN(DB_SELECT_QUERY("instance_id = '" + i + "'", "is_matched"));
  if (rows.size()) {
    LOGGER("Game core", "Success verify instance_id.\n  >> instance_id: " << i);
    return 1;
  } else {
    WARN_LOGGER("Game core", "Failed verify instance_id.\n  >> instance_id: " << i);
    return 0;
  }
}

const bool Verify::player_id(
  const PlayerId &p, 
  const std::shared_ptr<InstanceState> i
) {
  if ((p == i->player_ids[0] || p == i->player_ids[1])) {
    LOGGER("Game core", "Success verify player_id.\n  >> player_id: " << p);
    return 1;
  } else {
    WARN_LOGGER("Game core", "Failed verify player_id.\n  >> player_id: " << p);
    return 0;
  }
}

const PlayerId BoardOperator::judge(std::shared_ptr<InstanceState> i) {
  int r[2] = {0,0};
  for (char j = 0;j < 64;r[0] += (i->board[j++] == 1), r[0] += (i->board[j] == 2)) {}
  return i->player_ids[r[0] < r[1] + 1];
};

const int BoardOperator::update_board(
  int &p, std::shared_ptr<InstanceState> i,
  PlayerId &n
) {
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

  LOGGER("Game core", "Number of changes: " << t);

  return t ? t / c : 0;
}

const bool BoardOperator::verify_put(
  int &p,
  std::shared_ptr<InstanceState> i,
  PlayerId &n
) {
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

std::vector<int> BoardOperator::verify_put_len(int &p, int &c, int m, int *b) {
  std::vector<int> v;
  int cursor = p + m;
  while (cursor < 64 && 0 <= cursor && b[cursor] == (c - 1 ? 1 : 2) && std::abs((cursor % 8) - ((cursor - m) % 8)) == 1) {
    v.push_back(cursor);
    cursor += m;
  }
  if (!(cursor - p + m && b[cursor] == c)) v.clear();
  return std::move(v);
}

void BoardOperator::init_board(int *b) {
  for(char i=0;i<64;b[i++]=0){}
  b[27]=b[36]=1;b[28]=b[35]=2;
}

long int getTimeStamp() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
