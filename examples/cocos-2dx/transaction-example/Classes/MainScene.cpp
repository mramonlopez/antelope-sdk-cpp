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


Scene* MainScene::createScene() {
    return MainScene::create();
}

bool MainScene::init() {
    if (!Scene::init()) {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // ************************************************************************
    auto netwotk = Label::createWithTTF("Choose network:", "fonts/Marker Felt.ttf", 14);
    netwotk->setAlignment(TextHAlignment::CENTER);
    netwotk->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height - 40.0f));
    this->addChild(netwotk);

    // ************************************************************************
    auto wax = MenuItemImage::create("wax.png", "wax-selected.png", CC_CALLBACK_1(MainScene::menuCallback, this));
    wax->setName("wax");
    wax->setPosition(Vec2(origin.x + 40.0f, origin.y + visibleSize.height - 80.0f));
    
    // ************************************************************************
    auto telos = MenuItemImage::create("telos.png", "telos-selected.png", CC_CALLBACK_1(MainScene::menuCallback, this));
    telos->setName("telos");
    telos->setPosition(Vec2(origin.x + 80.0f, origin.y + visibleSize.height - 80.0f));
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(wax, telos, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    // ************************************************************************
    auto stakeBtn = Button::create("lime-button.png", "lime-button-pressed.png", "lime-button-disabled.png", ui::Widget::TextureResType::LOCAL);
    stakeBtn->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + 80.0f));
    stakeBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                if (this->network_ != "") {
                    this->stake();
                }
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
                if (this->network_ != "") {
                    this->unstake();
                }
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

    return true;
}

void MainScene::sendAction(onikami::eosclient::Action &action) {
    try {
        // Create auth
        Authorizer auth = Authorizer(this->actor_, this->priv_key_, this->permission_);
        
        auto client = new onikami::eosclient::EOSClient(this->endpoint_, {auth});
        
        // Get action ABI from network
        client->setABIAction(&action);
        
        // Create transaction
        onikami::eosclient::Transaction transaction;
        transaction.actions.push_back(action);
        
        auto result = client->sendTransaction(transaction);
        delete client;
        
        CCLOG("Transaction result: %s", result.c_str());
    } catch (std::runtime_error e) {
        CCLOG("Error: %s", e.what());
    }
}

void MainScene::stake() {
    json delegate_data;
    
    delegate_data["from"] = this->actor_;
    delegate_data["receiver"] = this->actor_;
    delegate_data["stake_net_quantity"] = this->quantity_;
    delegate_data["stake_cpu_quantity"] = this->quantity_;
    delegate_data["transfer"] = false;
    
    onikami::eosclient::Action action = {"eosio", "delegatebw", delegate_data, {{this->actor_, this->permission_}}};
    
    sendAction(action);
}

void MainScene::unstake() {
    json delegate_data;
    
    delegate_data["from"] = this->actor_;
    delegate_data["receiver"] = this->actor_;
    delegate_data["unstake_net_quantity"] = this->quantity_;
    delegate_data["unstake_cpu_quantity"] = this->quantity_;
    delegate_data["transfer"] = false;
    
    onikami::eosclient::Action action = {"eosio", "undelegatebw", delegate_data, {{this->actor_, this->permission_}}};
    
    sendAction(action);
}

void MainScene::setNetwork(const std::string &network) {
    network_ = network;

    auto icon = (cocos2d::Sprite *) this->getChildByName("icon");
    if (icon) {
        icon->initWithFile(network_ + "-selected.png");
    }

    if (network_ == "wax") {
        this->quantity_ = "0.01000000 WAX";
        this->endpoint_ = "http://api.waxtest.alohaeos.com/v1";
        this->actor_ = "antelopecpp1";
        this->permission_ = "active";
        this->priv_key_ = "<SET ACTIVE PRIVATE KEY>";
    } else if (network_ == "telos") {
        this->quantity_ = "0.0100 TLOS";
        this->endpoint_ = "http://testnet.telos.net/v1";
        this->actor_ = "antelopecpp1";
        this->permission_ = "active";
        this->priv_key_ = "<SET ACTIVE PRIVATE KEY>";
    }
}

void MainScene::menuCallback(Ref* pSender) {
    MenuItemImage* item = static_cast<MenuItemImage*>(pSender);
    
    auto brothers = item->getParent()->getChildren();
    
    for(auto brother : brothers) {
        ((MenuItemImage*)brother)->unselected();
    }
    
    item->selected();
    this->setNetwork(item->getName());
}
