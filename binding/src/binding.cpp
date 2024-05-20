#include "napi.h"
#include <iostream>

void Method(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    std::cout << "hi!" << std::endl;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "HelloWorld"), Napi::Function::New(env, Method));
    return exports;
}

NODE_API_MODULE(ibft, Init)
