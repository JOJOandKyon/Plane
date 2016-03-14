#include "StartScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GameScene/GameScene.h"
#include "GameScene\GameScene.h"
USING_NS_CC;

using namespace cocostudio::timeline;
using namespace cocos2d::ui;

Scene* Start::createScene()
{
	auto scene = Scene::create();
	auto layer = Start::create();
	scene->addChild(layer);
	return scene;
}

bool Start::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto rootNode = CSLoader::createNode("MainScene.csb");
	addChild(rootNode);
	Size visibleSize = Director::sharedDirector()->getVisibleSize();
	addbackground();
	schedule(schedule_selector(Start::update1));//开启调用Update函数功能
	addHero();
	addButton();
	//UserScore();
	return true;
}

void Start::addbackground()
{
	background1 = Sprite::create("ui/shoot_background/background.png");
	background1->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	addChild(background1);
	Size mapSize = background1->getContentSize();
	background2 = Sprite::create("ui/shoot_background/background.png");
	background2->setPosition(Point(visibleSize.width / 2, mapSize.height));
	addChild(background2);
}

void Start::update1(float delta)
{
	Size mapSize = background1->getContentSize();
	int X1 = background1->getPositionY() - 7;
	background1->setPositionY(X1);
	background2->setPositionY(background1->getPositionY() + mapSize.height);
	if (background1->getPositionY() < - mapSize.height/2 )
	{
		background1->setPositionY(visibleSize.height/2);
	}
}

void Start::addHero()
{	//添加帧动画
	auto Anime = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		auto str_name = String::createWithFormat("ui/shoot/hero%d.png", i);//提取玩家飞机图片地址
		Anime->addSpriteFrameWithFile(str_name->getCString());//将其图片地址转化为字符串类型
	}
	Anime->setDelayPerUnit(0.1);
	Anime->setLoops(-1);

	//添加玩家飞机精灵
	Hero = Sprite::create("ui/shoot/hero1.png");
	Size visibleSize = Director::sharedDirector()->getVisibleSize();
	Hero->setPosition(Point(visibleSize.width*0.5, visibleSize.height*0.3));
	Hero->runAction(Animate::create(Anime));
	addChild(Hero);
}

void Start::addButton()
{
	
	Button* StartButton = Button::create("game_start.png");
	StartButton->setPosition(Point(visibleSize.width/2,visibleSize.height*0.55));
	StartButton->addTouchEventListener(CC_CALLBACK_2(Start::GameStartonTouch, this));
	addChild(StartButton);
}

void Start::GameStartonTouch(Ref* pSender, Widget::TouchEventType type)
{
	auto gameScene = GameScene::createScene();
	Director::getInstance()->setDepthTest(true);
	//TransitionScene* ccts = TransitionScene::create(2,gameScene);
	CCTransitionScene* reScene = CCTransitionMoveInR::create(0.5f,gameScene);
	Director::getInstance()->replaceScene(reScene);
}

//void Start::UserScore()
//{
//	userDefault = UserDefault::getInstance();
//	userDefault->setIntegerForKey("TopScore", topScore);
//	userDefault->flush();
//}