#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace CocosDenshion;

class GameScene : public cocos2d::Layer
{
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameScene);
	void addHero();//添加主角飞机精灵
	void addbackground();//添加背景精灵
	void addBullet();//添加子弹精灵
	void addEnemy();//添加敌机精灵
	void update1(float delta); //背景移动update函数
	void update2(float dalta);//子弹和敌机移动update函数
	void update3(float dalta);//生成子弹update的函数
	void update4(float dalta);//生成敌机update的函数
	bool HeroTouchBegan(Touch* touch, Event* event);//主角飞机触摸开始回调事件
	void HeroTouchMoved(Touch* touch, Event* event);//主角飞机触摸移动回调事件
	bool onContactBegin(const PhysicsContact& contsct);//碰撞检测回调事件
	void EnemyBonb(Node* Sprite);//执行移除
	void Gameover();//游戏结束
	void addScore();//添加分数
	void UserScore();//记录用户分数
	void addMenu();//添加菜单项
	void GameRetart(Ref* pSender);//游戏重新开始菜单子项回调事件
	void GameExit(Ref* psender);//退出游戏菜单子项回调事件
	void TopScore();//显示用户历史最高分
private:
	Sprite * background1;
	Sprite * background2;
	Size visibleSize = Director::sharedDirector()->getVisibleSize();
	EventListenerTouchOneByOne* listenerTouch;
	Sprite* Hero;
	Sprite* Bullet;
	Vector<Sprite*> bulletList;
	Vector<Sprite*> EnemyList;
	float randomPointX();
	int score=0;
	int topScore=0;
	LabelTTF* Score;
	UserDefault* userDefault;
};

#endif