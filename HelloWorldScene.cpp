#include "HelloWorldScene.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

	/*
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    */
	m_isHeroWalking = false;

	m_map = CCTMXTiledMap::create("0.tmx");
	addChild(m_map);

	CreateHeroAnimations();

	CCArray* frames = m_walkAnimations[kDown]->getFrames();
	CCAnimationFrame* frame = (CCAnimationFrame*)frames->objectAtIndex(0);
	m_heroSprite = CCSprite::createWithSpriteFrame(frame->getSpriteFrame());

	m_heroSprite->setAnchorPoint(CCPointZero);
	m_heroSprite->setPosition(positionForTileCoord(ccp(1, 11)));
	addChild(m_heroSprite);

	CCMenuItem* down = CCMenuItemFont::create("down", this, menu_selector(HelloWorld::menuCallBackMove));
	CCMenuItem* left = CCMenuItemFont::create("left", this, menu_selector(HelloWorld::menuCallBackMove));
	CCMenuItem* right = CCMenuItemFont::create("right", this, menu_selector(HelloWorld::menuCallBackMove));
	CCMenuItem* up = CCMenuItemFont::create("up", this, menu_selector(HelloWorld::menuCallBackMove));

	CCMenu* menu = CCMenu::create(down, left, right, up, NULL);
	down->setTag(kDown);
	left->setTag(kLeft);
	right->setTag(kRight);
	up->setTag(kUp);

	menu->alignItemsHorizontallyWithPadding(50);
	addChild(menu);

	schedule(schedule_selector(HelloWorld::update));

    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::CreateHeroAnimations()
{
	CCTexture2D* heroTexture = CCTextureCache::sharedTextureCache()->addImage("hero.png");
	m_walkAnimations = new CCAnimation*[4];
	
	for (int i = 0; i < 4; i++)
	{
		CCArray* arrFrames = new CCArray(4);
		for (int j = 0; j < 4; j++)
		{
			arrFrames->addObject(CCSpriteFrame::createWithTexture(heroTexture, cocos2d::CCRect(32*j, 32*i, 32, 32)));
		}
		CCAnimation* animation = new CCAnimation();
		animation->initWithSpriteFrames(arrFrames, 0.1f);
		arrFrames->release();
		m_walkAnimations[i] = animation;
	}
	
}

void HelloWorld::menuCallBackMove( CCObject* pSender )
{
	if (m_isHeroWalking)
	{
		return;
	}
	m_isHeroWalking = true;
	CCNode* node = (CCNode*)pSender;
	int targetDirection = node->getTag();

	CCPoint moveByPosition;
	switch (targetDirection)
	{
	case kDown:
		moveByPosition = ccp(0, -32);
		break;
	case kLeft:
		moveByPosition = ccp(-32, 0);
		break;
	case kRight:
		moveByPosition = ccp(32, 0);
		break;
	case kUp:
		moveByPosition = ccp(0, 32);
		break;
	}
	

	CCAction* action = CCSequence::create(CCSpawn::create(CCAnimate::create(m_walkAnimations[targetDirection]), 
		CCMoveBy::create(0.40f, moveByPosition), NULL),
		CCCallFuncND::create(this, callfuncND_selector(HelloWorld::onWalkDone), (void*)targetDirection), NULL);
	m_heroSprite->runAction(action);
}

void HelloWorld::onWalkDone( CCNode* pTarget, void* data )
{
	m_isHeroWalking = false;
	int dirction = (int)data;
	setFaceDirection((HeroDirection)dirction);
}

void HelloWorld::setFaceDirection( HeroDirection direction )
{
	m_heroSprite->setTextureRect(CCRectMake(0, 32*direction, 32, 32));
}

HelloWorld::~HelloWorld()
{
	for (int i = 0; i < 4; i++)
	{
		CC_SAFE_RELEASE(m_walkAnimations[i]);
	}
	CC_SAFE_DELETE_ARRAY(m_walkAnimations);	
	this->unscheduleAllSelectors();
}

cocos2d::CCPoint HelloWorld::positionForTileCoord( cocos2d::CCPoint tileCoord )
{
	CCPoint pos = ccp((tileCoord.x * m_map->getTileSize().width), 
		((m_map->getMapSize().height - tileCoord.y - 1) * m_map->getTileSize().height));
	return pos;
}

void HelloWorld::setSceneScrollPosition( cocos2d::CCPoint position )
{
	// 获取屏幕尺寸
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	// 计算Tilemap的宽高
	CCSize mapSizeInPixel = CCSizeMake(m_map->getMapSize().width * m_map->getTileSize().width, 
		m_map->getMapSize().height * m_map->getTileSize().height);
	// 取勇士当前x坐标和屏幕中点x的最大值， 如果勇士的x值比较大哦，则会滚动
	float x = MAX(position.x, screenSize.width / 2.0f);
	float y = MAX(position.y, screenSize.height / 2.0f);
	// 地图总宽度大于屏幕宽度的时候才有可能滚动
	if (mapSizeInPixel.width > screenSize.width)
	{
		x = MIN(x, mapSizeInPixel.width - screenSize.width / 2.0f);
	}
	if (mapSizeInPixel.height > screenSize.height)
	{
		y = MIN(y, mapSizeInPixel.height - screenSize.height / 2.0f);
	}
	
	// 勇士的实际位置
	CCPoint heroPosition = ccp(x, y);
	// 屏幕中点位置
	CCPoint screenCenter = ccp(screenSize.width / 2.0f, screenSize.height / 2.0f);
	// 计算勇士实际位置和中点位置的距离
	CCPoint scrollPosition = ccpSub(screenCenter, heroPosition);
	// 将场景移动到响应位置
	this->setPosition(scrollPosition);
}

void HelloWorld::update( float t )
{
	if (m_isHeroWalking)
	{
		setSceneScrollPosition(m_heroSprite->getPosition());
	}
	
}
