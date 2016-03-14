#ifndef _STARAT_SCENE_H_
#define _STARAT_SCENE_H_

#include "cocos2d.h"
#include "ui\CocosGUI.h"
using namespace cocos2d;
using namespace cocos2d::ui;

class Start : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(Start);
	void addbackground();
	void update1(float delta);
	void addHero();
	void addButton();
	void GameStartonTouch(Ref* pSender, Widget::TouchEventType type);
	//void UserScore();
public:
	//static UserDefault* userDefault;
private:
	Sprite * background1;
	Sprite * background2;
	Sprite* Hero;
	Size visibleSize = Director::sharedDirector()->getVisibleSize();
	//int topScore=0;
};

#endif