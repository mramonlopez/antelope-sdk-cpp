//
//  MainScene.cpp
//  SigningRequestExample

#include "MainScene.hpp"
#include <nlohmann/json.hpp>
#include <eosclient/Action.hpp>
#include <eosclient/EosioSigningRequest.hpp>
#include <eosclient/SigningRequestCallbackManager.hpp>
#include <eosclient/Transaction.hpp>
#include <eosclient/eosclient.hpp>
#include <eosclient/PermissionLevel.hpp>

USING_NS_CC;
using namespace cocos2d::ui;
using namespace nlohmann;
using namespace onikami::eosclient;

MainScene* MainScene::createWithActorAndPermission(std::string actor, std::string permission) {
    auto scene = new MainScene();
    
    if (scene) {
        scene->actor_ = actor;
        scene->permission_ = permission;
        scene->network_ = "wax";
        
        if (scene->init()) {
            scene->autorelease();
            return scene;
        }

        CC_SAFE_DELETE(scene);
        scene = nullptr;
    }
    
    return nullptr;
}

bool MainScene::init() {
    if (!Scene::init()) {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // ************************************************************************
    auto icon = Sprite::create(this->network_ + ".png");
    icon->setPosition(Vec2(origin.x + 20.0f, origin.y + visibleSize.height - 40.0f));
    icon->setName("icon");
    this->addChild(icon);

    auto label = Label::createWithTTF("Logged as " + this->actor_ + "@" + this->permission_, "fonts/Marker Felt.ttf", 8);
    label->setPosition(Vec2(origin.x + visibleSize.width/2 + 20.0f,
                            origin.y + visibleSize.height - 40.0f));
    this->addChild(label, 1);
    
    // ************************************************************************
    auto stakeBtn = Button::create("lime-button.png", "lime-button-pressed.png", "lime-button-disabled.png", ui::Widget::TextureResType::LOCAL);
    stakeBtn->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + 80.0f));
    stakeBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                this->stake();
                break;
            default:
                break;
        }
    });
    
    auto size = stakeBtn->getContentSize();
    
    auto stakeLbl = Label::createWithTTF("Stake", "fonts/Marker Felt.ttf", 12);
    stakeLbl->setTextColor(Color4B::WHITE);
    stakeLbl->setAlignment(TextHAlignment::CENTER);
    stakeLbl->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));
    stakeBtn->addChild(stakeLbl);
    
    this->addChild(stakeBtn);
    
    // ************************************************************************
    auto unstakeBtn = Button::create("lime-button.png", "lime-button-pressed.png", "lime-button-disabled.png", ui::Widget::TextureResType::LOCAL);
    unstakeBtn->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + 40.0f));
    unstakeBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                this->unstake();
                break;
            default:
                break;
        }
    });
    
    size = unstakeBtn->getContentSize();
    
    auto unstakeLbl = Label::createWithTTF("Untake", "fonts/Marker Felt.ttf", 12);
    unstakeLbl->setTextColor(Color4B::WHITE);
    unstakeLbl->setAlignment(TextHAlignment::CENTER);
    unstakeLbl->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));
    unstakeBtn->addChild(unstakeLbl);
    
    this->addChild(unstakeBtn);

    SigningRequestCallbackManager::getInstance()->setDelegate(this);
    
    return true;
}

void MainScene::stake() {
    json delegate_data;
    
    delegate_data["from"] = this->actor_;
    delegate_data["receiver"] = this->actor_;
    delegate_data["stake_net_quantity"] = this->quantity_;
    delegate_data["stake_cpu_quantity"] = this->quantity_;
    delegate_data["transfer"] = false;
    
    onikami::eosclient::Action action = {"eosio", "delegatebw", delegate_data, {{this->actor_, this->permission_}}};
    
    try {
        // Get action ABI from network
        auto client = new onikami::eosclient::EOSClient(this->endpoint_, {});
        client->setABIAction(&action);
        delete client;
        
        // Create transaction
        onikami::eosclient::Transaction transaction;
        transaction.actions.push_back(action);
        
        onikami::eosclient::RequestDataV2 data;
        
        // Create request
        data.chain_id = this->chain_id_;
        data.req = transaction;
        data.flags = onikami::eosclient::RequestDataV2::BROADCAST;
        data.callback = "esrtestapp://result?tx={{tx}}";
        
        auto request = new onikami::eosclient::EosioSigningRequest(data);
        auto url = request->encode();
        
        Application::getInstance()->openURL(url);
        
        delete request;
    } catch (std::runtime_error e) {
        CCLOG("Error: %s", e.what());
    }
}

void MainScene::unstake() {
    json delegate_data;
    
    delegate_data["from"] = this->actor_;
    delegate_data["receiver"] = this->actor_;
    delegate_data["unstake_net_quantity"] = this->quantity_;
    delegate_data["unstake_cpu_quantity"] = this->quantity_;
    delegate_data["transfer"] = false;
    
    onikami::eosclient::Action action = {"eosio", "undelegatebw", delegate_data, {{this->actor_, this->permission_}}};
    
    try {
        // Get action ABI from network
        auto client = new onikami::eosclient::EOSClient(this->endpoint_, {});
        client->setABIAction(&action);
        delete client;

        // Create transaction
        onikami::eosclient::Transaction transaction;
        transaction.actions.push_back(action);
        
        onikami::eosclient::RequestDataV2 data;

        // Create request
        data.chain_id = this->chain_id_;
        data.req = transaction;
        data.flags = onikami::eosclient::RequestDataV2::BROADCAST;
        data.callback = "esrtestapp://result?tx={{tx}}";
        
        auto request = new onikami::eosclient::EosioSigningRequest(data);
        auto url = request->encode();
        
        Application::getInstance()->openURL(url);
        
        delete request;
    } catch (std::runtime_error e) {
        CCLOG("Error: %s", e.what());
    }
}

void MainScene::setNetwork(const std::string &network) {
    network_ = network;

    auto icon = (cocos2d::Sprite *) this->getChildByName("icon");
    if (icon) {
        icon->initWithFile(network_ + "-selected.png");
    }

    if (network_ == "wax") {
        this->quantity_ = "0.01000000 WAX";
        this->chain_id_ = "1064487b3cd1a897ce03ae5b6a865651747e2e152090f99c1d19d44e01aea5a4";
        this->endpoint_ = "http://api.waxsweden.org/v1";
    } else if (network_ == "telos") {
        this->quantity_ = "0.0100 TLOS";
        this->chain_id_ = "4667b205c6838ef70ff7988f6e8257e8be0e1284a2f59699054a018f743b1d11";
        this->endpoint_ = "http://mainnet.telos.net/v1";
    }
}

void MainScene::onCallback(const SigningRequestCallback signingRequestCallback) {
    CCLOG("Transaction received: %s\n", signingRequestCallback.tx.c_str());
}
