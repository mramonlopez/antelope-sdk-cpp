//
//  MainScene.hpp
//  SigningRequestExample

#ifndef MainScene_hpp
#define MainScene_hpp

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <eosclient/SigningRequestCallbackDelegate.hpp>

class MainScene : public cocos2d::Scene, public onikami::eosclient::SigningRequestCallbackDelegate
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    static MainScene* createWithActorAndPermission(std::string actor, std::string permission);
    
    CC_SYNTHESIZE_READONLY(std::string, network_, Network);

    void setNetwork(const std::string &network);
    virtual void onCallback(const onikami::eosclient::SigningRequestCallback signingRequestCallback);

protected:
    std::string actor_ = "";
    std::string permission_ = "";
    std::string quantity_ = "";
    std::string chain_id_ = "";
    std::string endpoint_ = "";
    
    void stake();
    void unstake();
};

#endif /* MainScene_hpp */
