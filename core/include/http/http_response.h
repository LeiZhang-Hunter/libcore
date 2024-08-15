#pragma once

#include <string>
#include "non_copyable.h"
#include "event/event_smart_ptr.h"
namespace Core {
namespace Http {

class HttpResponse :public Core::Noncopyable {

public:
//    HttpResponse(struct evhttp_request *request_, const char* uri)
    HttpResponse(struct evhttp_request *request_)
    :
//    httpUri(uri),
    request(request_),
    httpBuffer(evbuffer_new()),
    output_headers(evhttp_request_get_output_headers(request)) {};

    bool response(short code, const std::string& response);

    bool header(const std::string &key, const std::string &value);

    /**
     * 获取响应体积
     * @return
     */
    size_t getResponseSize() {
        return responseSize;
    }

    ~HttpResponse() {
    };

private:
    /**
     * @brief 请求句柄
     * 
     */
    struct evhttp_request *request;

    /**
     * @brief buffer
     * 
     */
    Event::EventBufferPtr httpBuffer;

    size_t responseSize = 0;

    //http响应头
    struct evkeyvalq *output_headers;
};
}
}
