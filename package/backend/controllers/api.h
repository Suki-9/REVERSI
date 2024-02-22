#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class api : public drogon::HttpController<api>
{
  public:
    void post_create(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr &)> &&callback);
    void post_find(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr &)> &&callback);

    METHOD_LIST_BEGIN
    METHOD_ADD(api::post_create, "/create", Post, Options);
    METHOD_ADD(api::post_find, "/find", Post, Options);
    METHOD_LIST_END
};
