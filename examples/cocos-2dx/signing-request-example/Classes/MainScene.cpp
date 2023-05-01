//
//  MainScene.cpp
//  MyCppGame

#include "MainScene.hpp"
#include <nlohmann/json.hpp>
#include <eosclient/Action.hpp>
#include <eosclient/EosioSigningRequest.hpp>
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

    auto label = Label::createWithTTF("Logged as " + this->actor_ + "@" + this->permission_, "fonts/Marker Felt.ttf", 10);
    label->setPosition(Vec2(origin.x + visibleSize.width/2 + 20.0f,
                            origin.y + visibleSize.height - 40.0f));
    this->addChild(label, 1);
    
    // ************************************************************************
    auto hiButton = Button::create("lime-button.png", "lime-button-pressed.png", "lime-button-disabled.png", ui::Widget::TextureResType::LOCAL);
    hiButton->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + 40.0f));
    hiButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                this->stake();
                break;
            default:
                break;
        }
    });
    
    auto size = hiButton->getContentSize();
    
    auto hi = Label::createWithTTF("Say hi!", "fonts/Marker Felt.ttf", 12);
    hi->setTextColor(Color4B::WHITE);
    hi->setAlignment(TextHAlignment::CENTER);
    hi->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));
    hiButton->addChild(hi);
    
    this->addChild(hiButton);
    
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
    
}

void MainScene::unstake() {
    json delegate_data;
    
    delegate_data["from"] = this->actor_;
    delegate_data["receiver"] = this->actor_;
    delegate_data["unstake_net_quantity"] = this->quantity_;
    delegate_data["unstake_cpu_quantity"] = this->quantity_;
    delegate_data["transfer"] = false;
    
    onikami::eosclient::Action action = {"eosio", "undelegatebw", delegate_data, {{this->actor_, this->permission_}}};
    
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
        this->endpoint_ = "http://api.waxsweden.org";
    } else if (network_ == "telos") {
        this->quantity_ = "0.0100 TLOS";
        this->chain_id_ = "4667b205c6838ef70ff7988f6e8257e8be0e1284a2f59699054a018f743b1d11";
        this->endpoint_ = "https://mainnet.telos.net/ ";
    }
}
