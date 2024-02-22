#include <drogon/drogon.h>

/* ----- use modules include -----*/
#include "logger_module.h"

Json::Value* json_parse(Json::Value &root, const std::string &message) {
  try {
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    auto reader = builder.newCharReader();
    if (!reader->parse(message.c_str(), message.cend().base(), &root, &errs)) {
      ERR_LOGGER("JSON module", "err " << errs);
      return nullptr;
    }
    return &root;
  } catch (...) {
    return nullptr;
  }
}

std::string json_stringify(Json::Value &root) {
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "";
  return Json::writeString(builder, root);
};