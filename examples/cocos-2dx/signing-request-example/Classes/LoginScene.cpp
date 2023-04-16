/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "LoginScene.h"
#include "ui/CocosGUI.h"
#include <eosclient/EosioSigningRequest.hpp>

USING_NS_CC;
using namespace cocos2d::ui;

Scene* LoginScene::createScene()
{
    return LoginScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool LoginScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
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
    auto wax = MenuItemImage::create("wax.png", "wax-selected.png", CC_CALLBACK_1(LoginScene::menuCallback, this));
    wax->setName("wax");
    wax->setPosition(Vec2(origin.x + 40.0f, origin.y + visibleSize.height - 80.0f));
    
    // ************************************************************************
    auto telos = MenuItemImage::create("telos.png", "telos-selected.png", CC_CALLBACK_1(LoginScene::menuCallback, this));
    wax->setName("telos");
    telos->setPosition(Vec2(origin.x + 80.0f, origin.y + visibleSize.height - 80.0f));
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(wax, telos, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    // ************************************************************************
    auto label = Label::createWithTTF("Account:", "fonts/Marker Felt.ttf", 14);
    label->setAlignment(TextHAlignment::CENTER);
    label->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height * 0.5f + 20.0f));
    this->addChild(label);
    
    // ************************************************************************
    auto account = TextField::create("(enter account)", "fonts/som-border.fnt", 14);
    account->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height * 0.5f));
    account->setCursorEnabled(true);
    account->setMaxLength(12);
    account->setMaxLengthEnabled(true);
    account->setTextColor(Color4B::WHITE);
    account->addEventListener([this](Ref* caller, TextField::EventType event) -> void {
//        if (event == TextField::EventType::ATTACH_WITH_IME || event == TextField::EventType::DETACH_WITH_IME) {
//            this->updatePopupPosition(event == TextField::EventType::ATTACH_WITH_IME);
//        }
    });
    
    // ************************************************************************
    auto info = Label::createWithTTF("(12 characters, alphanumeric a-z, 1-5)", "fonts/Marker Felt.ttf", 10);
    info->setTextColor(Color4B::GRAY);
    info->setAlignment(TextHAlignment::CENTER);
    info->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height * 0.5f - 20.0f));
    this->addChild(info);
    
    this->addChild(account);
    
    // ************************************************************************
    auto loginButton = Button::create("lime-button.png", "lime-button-pressed.png", "lime-button-dissabled.png", ui::Widget::TextureResType::LOCAL);
    loginButton->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + 40.0f));
    loginButton->addTouchEventListener([=](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                // loginButton
                break;
            default:
                break;
        }
    });
    
    auto size = loginButton->getContentSize();
    
    auto login = Label::createWithTTF("Login", "fonts/Marker Felt.ttf", 12);
    login->setTextColor(Color4B::WHITE);
    login->setAlignment(TextHAlignment::CENTER);
    login->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));
    loginButton->addChild(login);
    
    this->addChild(loginButton);

    return true;
}


void LoginScene::menuCallback(Ref* pSender) {
    MenuItemImage* item = static_cast<MenuItemImage*>(pSender);
    
    auto brothers = item->getParent()->getChildren();
    
    for(auto brother : brothers) {
        ((MenuItemImage*)brother)->unselected();
    }
    
    item->selected();
    this->network_ = item->getName();
}


void LoginScene::login(std::string account) {
    onikami::eosclient::RequestDataV2 data;

    data.chain_id = "4667b205c6838ef70ff7988f6e8257e8be0e1284a2f59699054a018f743b1d11"; // TELOS

    onikami::eosclient::IdentityV2 identity;

    identity.permission.actor = "onikamigames";
    identity.permission.permission = "active";
    data.req = identity;
    data.callback = "waxtestapp://login/?tx={{tx}}";


    auto request = new onikami::eosclient::EosioSigningRequest(data);
    auto url = request->encode();
    
    CCLOG("URL: %s", url.c_str());
    
    Application::getInstance()->openURL(url);
    
    delete request;
}
