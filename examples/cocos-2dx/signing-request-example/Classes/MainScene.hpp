//
//  MainScene.hpp
//  MyCppGame

#ifndef MainScene_hpp
#define MainScene_hpp

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    static MainScene* createWithActorAndPermission(std::string actor, std::string permission);
    
    CC_SYNTHESIZE_READONLY(std::string, network_, Network);

    void setNetwork(const std::string &network);
    
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
