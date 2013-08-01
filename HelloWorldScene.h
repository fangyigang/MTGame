#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::CCLayer
{
public:
	~HelloWorld();
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	
	typedef enum{
		kDown,
		kLeft,
		kRight,
		kUp,
	}HeroDirection;

private:
	cocos2d::CCTMXTiledMap* m_map;
	cocos2d::CCSprite* m_heroSprite;
	cocos2d::CCAnimation** m_walkAnimations;
	bool m_isHeroWalking;

	void CreateHeroAnimations();
	void setFaceDirection(HeroDirection direction);

	void menuCallBackMove(CCObject* pSender);
	void onWalkDone(CCNode* pTarget, void* data);

	// 坐标系转换
	cocos2d::CCPoint positionForTileCoord(cocos2d::CCPoint tileCoord);
	// 场景移动
	void setSceneScrollPosition(cocos2d::CCPoint position);

	void update(float t);
};

#endif // __HELLOWORLD_SCENE_H__
