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
	void addHero();//������Ƿɻ�����
	void addbackground();//��ӱ�������
	void addBullet();//����ӵ�����
	void addEnemy();//��ӵл�����
	void update1(float delta); //�����ƶ�update����
	void update2(float dalta);//�ӵ��͵л��ƶ�update����
	void update3(float dalta);//�����ӵ�update�ĺ���
	void update4(float dalta);//���ɵл�update�ĺ���
	bool HeroTouchBegan(Touch* touch, Event* event);//���Ƿɻ�������ʼ�ص��¼�
	void HeroTouchMoved(Touch* touch, Event* event);//���Ƿɻ������ƶ��ص��¼�
	bool onContactBegin(const PhysicsContact& contsct);//��ײ���ص��¼�
	void EnemyBonb(Node* Sprite);//ִ���Ƴ�
	void Gameover();//��Ϸ����
	void addScore();//��ӷ���
	void UserScore();//��¼�û�����
	void addMenu();//��Ӳ˵���
	void GameRetart(Ref* pSender);//��Ϸ���¿�ʼ�˵�����ص��¼�
	void GameExit(Ref* psender);//�˳���Ϸ�˵�����ص��¼�
	void TopScore();//��ʾ�û���ʷ��߷�
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