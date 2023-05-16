//
//  MainScene.hpp
//  SigningRequestExample

#ifndef MainScene_hpp
#define MainScene_hpp

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <eosclient/SigningRequestCallbackDelegate.hpp>
#include <eosclient/Action.hpp>

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    
    CREATE_FUNC(MainScene);
    
    void menuCallback(Ref* pSender);
    void setNetwork(const std::string &network);
    
protected:
    std::string actor_ = "";
    std::string permission_ = "";
    std::string priv_key_ = "";
    
    std::string quantity_ = "";
    std::string endpoint_ = "";
    std::string network_ = "";
    
    void sendAction(onikami::eosclient::Action &action);
    
    void stake();
    void unstake();
};

#endif /* MainScene_hpp */
