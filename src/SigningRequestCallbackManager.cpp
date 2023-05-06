#include <eosclient/SigningRequestCallbackManager.hpp>
#include <eosclient/EdUrlParser.h>

using namespace onikami::eosclient;

SigningRequestCallbackManager* SigningRequestCallbackManager::getInstance() {
    static SigningRequestCallbackManager* instance = nullptr;

    if (instance == nullptr) {
        instance = new SigningRequestCallbackManager();
    }

    return instance;
}

void SigningRequestCallbackManager::setDelegate(SigningRequestCallbackDelegate* delegate) {
    delegate_ = delegate;
}

void SigningRequestCallbackManager::onCallback(const std::string url_string) {
    if (delegate_ == nullptr) {
        return;
    }

    auto url = EdUrlParser::parseUrl(url_string);

    SigningRequestCallback signingRequestCallback;

    signingRequestCallback.host = url->hostname;
    signingRequestCallback.path = url->path;
    
    unordered_map<string, string> map;

	int c = EdUrlParser::parseKeyValueMap(&map, url->query);

    if (c > 0) {
        if (map.count("sig") > 0) {
            signingRequestCallback.sigs.push_back(map["sig"]);
        }

        if (map.count("tx") > 0) {
            signingRequestCallback.tx = map["tx"];
        }
        
        if (map.count("bn") > 0) {
            signingRequestCallback.bn = map["bn"];
        }

        if (map.count("sa") > 0) {
            signingRequestCallback.sa = map["sa"];
        }

        if (map.count("sp") > 0) {
            signingRequestCallback.sp = map["sp"];
        }

        if (map.count("rbn") > 0) {
            signingRequestCallback.rbn = map["rbn"];
        }

        if (map.count("rid") > 0) {
            signingRequestCallback.rid = map["rid"];
        }

        if (map.count("req") > 0) {
            signingRequestCallback.req = map["req"];
        }

        if (map.count("ex") > 0) {
            signingRequestCallback.ex = map["ex"];
        }

        if (map.count("cid") > 0) {
            signingRequestCallback.cid = map["cid"];
        }

        // Additional signatures.
        signingRequestCallback.sigs = vector<string>();
        for (auto const& x : map) {
            if (x.first.substr(0, 3) == "sig") {
                signingRequestCallback.sigs.push_back(x.second);
            }
        }
    }

    delegate_->onCallback(signingRequestCallback);

    delete url;
}