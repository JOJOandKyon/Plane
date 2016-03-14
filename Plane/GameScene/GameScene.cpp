#include "GameScene.h"
#include "cocostudio/CocoStudio.h"
#include "StartScene\StartScene.h"
//#include "Enemy\Enemy.h"

USING_NS_CC;
using namespace cocostudio::timeline;

Scene* GameScene::createScene()
{
	//Scene* Scene = Scene::create();
	Scene* Scene = Scene::createWithPhysics();
	//Scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	Scene->getPhysicsWorld()->setGravity(Vec2(0,-900));
	Layer* layer = GameScene::create();
	Scene->addChild(layer);
	return Scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto rootNode = CSLoader::createNode("MainScene.csb");
	addChild(rootNode);
	//播放游戏背景音乐
	SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/game_music.mp3",true);
	addbackground();//添加背景
	addHero();//添加主角机
	UserScore();//创建用户分数数据
	schedule(schedule_selector(GameScene::update1));//开启背景Update函数
	schedule(schedule_selector(GameScene::update2));//开启子弹Update函数
	schedule(schedule_selector(GameScene::update3), 0.4);//开启玩家飞机Update函数
	schedule(schedule_selector(GameScene::update4), 0.4);//开启敌机Update函数
	auto contactListener = EventListenerPhysicsContact::create();//创建刚体碰撞监听事件
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin,this);//碰撞回调事件
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);//添加碰撞监听
	addScore();//添加分数计数
	return true;
}

void GameScene::addHero()//添加主角机
{
	auto Anime = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		auto str_name = String::createWithFormat("ui/shoot/hero%d.png",i); // 提取玩家飞机图片地址
		Anime->addSpriteFrameWithFile(str_name->getCString());//将其图片地址转化为字符串类型
	}
	Anime->setDelayPerUnit(0.1);
	Anime->setLoops(-1);

	//添加玩家飞机精灵
	Hero = Sprite::create("ui/shoot/hero1.png");
	Hero->setTag(103);
	Size visibleSize = Director::sharedDirector()->getVisibleSize();
	Hero->setPosition(Point(visibleSize.width*0.5, visibleSize.height*0.3));
	Hero->runAction(Animate::create(Anime));
	addChild(Hero,10);
	auto Hero_body = PhysicsBody::createCircle(35);
	Hero_body->setGravityEnable(false);//设置飞机不受重力
	Hero_body->setDynamic(true);//设置为可以被物理场所作用
	Hero_body->setContactTestBitmask(1);//必须设置这项为1才能检测到不同的物体碰撞
	Hero_body->setCategoryBitmask(0x0004);
	Hero_body->setContactTestBitmask(0x006);
	Hero_body->setCollisionBitmask(0x0006);
	Hero->setPhysicsBody(Hero_body);
	listenerTouch = EventListenerTouchOneByOne::create();//添加单点触摸事件
	listenerTouch->setSwallowTouches(true);
	listenerTouch->onTouchBegan= CC_CALLBACK_2(GameScene::HeroTouchBegan, this);//添加单点触摸开始事件回调
	listenerTouch->onTouchMoved = CC_CALLBACK_2(GameScene::HeroTouchMoved, this);//添加单点触摸移动事件回调
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, Hero);
}

void GameScene::addbackground()//添加背景精灵
{
	background1 = Sprite::create("ui/shoot_background/background.png");
	background1->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	addChild(background1);
	Size mapSize = background1->getContentSize();
	background2 = Sprite::create("ui/shoot_background/background.png");
	background2->setPosition(Point(visibleSize.width / 2, mapSize.height));
	addChild(background2);
}

void GameScene::addBullet()//添加子弹精灵
{
	Bullet = Sprite::create("ui/shoot/bullet2.png");
	auto point = Hero->getPosition();
	Bullet->setPosition(Point(point));
	Bullet->setTag(106);
	addChild(Bullet);
	bulletList.pushBack(Bullet);
	auto bullet_body = PhysicsBody::createBox(Bullet->getContentSize());
	bullet_body->setDynamic(true);
	bullet_body->setContactTestBitmask(1);
	bullet_body->setGravityEnable(false);
	bullet_body->setCategoryBitmask(0x0001);
	bullet_body->setContactTestBitmask(0x0003);
	bullet_body->setCollisionBitmask(0x0003);
	Bullet->setPhysicsBody(bullet_body);
}

void GameScene::addEnemy()//添加敌机精灵
{
	//随机2种敌机
	int i;
	do
	{
		i = rand() % 2 + 1;
		if (i < 3 && i > 0)
		{
			break;
		}
	} while (1);
	auto str_name = String::createWithFormat("ui/shoot/enemy%d.png", i);
	auto enemy = Sprite::create(str_name->getCString());
	enemy->setTag(103 + i);
	enemy->setPosition(randomPointX(), visibleSize.height + 120 + rand() % 1500);
	enemy->setAnchorPoint(ccp(0.5, 1));
	addChild(enemy,10);
	EnemyList.pushBack(enemy);//加入Vector
	//创建对应刚体
	auto enemy_body = PhysicsBody::create();
	if (i == 1)
	{
		enemy_body = PhysicsBody::createCircle(20);
	}
	else
	{
		enemy_body = PhysicsBody::createCircle(40);
	}
	enemy_body->setDynamic(true);
	enemy_body->setGravityEnable(false);
	enemy_body->setContactTestBitmask(1);
	enemy_body->setCategoryBitmask(0x002);
	enemy_body->setContactTestBitmask(0x0005);
	enemy_body->setCollisionBitmask(0x0005);
	enemy->setPhysicsBody(enemy_body);
}

void GameScene::update1(float delta)//背景移动update函数
{
	Size mapSize = background1->getContentSize();
	int Y1 = background1->getPositionY() - 7;
	background1->setPositionY(Y1);
	background2->setPositionY(background1->getPositionY() + mapSize.height);
	if (background1->getPositionY() < -mapSize.height / 2)
	{
		background1->setPositionY(visibleSize.height / 2);
	}
}

void GameScene::update2(float delta)//子弹和敌机移动update函数
{
	for (int i = 0; i < bulletList.size(); i++)
	{	
		auto Bullet = bulletList.at(i);
		if (Bullet->getPositionY() > visibleSize.height)
		{
			Bullet->removeFromParent(); //从层中移除
			bulletList.eraseObject(Bullet);//从记录所有子弹的vector中移除
			i--;
		}
		else
		{
			auto bullet_speed = 10;
			Bullet->setPositionY(Bullet->getPositionY()+bullet_speed);
		}
	}
	for (int i = 0; i < EnemyList.size(); i++)//调出Vector的成员
	{
		auto enemy = EnemyList.at(i);
		enemy->setPositionY(enemy->getPositionY() - 5);
		if (enemy->getPositionY() < 0)//超出屏幕范围，移除敌机精灵
		{
			enemy->removeFromParent();
			EnemyList.eraseObject(enemy);
			i--;
		}
	}
}

void GameScene::update3(float dalta)//生成子弹update的函数
{
	addBullet();
}
void GameScene::update4(float delta)//生成敌机update的函数
{
	GameScene::addEnemy();
}
bool GameScene::HeroTouchBegan(Touch* touch, Event* event)//主角精灵单点触控开始回调事件
{
	CCLOG("begin");
	auto targt = static_cast<Sprite*>(event->getCurrentTarget());
	auto location = targt->convertToNodeSpace(touch->getLocation());
	auto size = targt->getContentSize();
	auto rect = Rect(0, 0, size.width, size.height);
	if (rect.containsPoint(location))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GameScene::HeroTouchMoved(Touch* touch, Event* event)//主角精灵单点触控移动回调事件
{
	auto targt = static_cast<Sprite*>(event->getCurrentTarget());
	targt->setPosition(touch->getLocation());
}



float GameScene::randomPointX()//随机敌机精灵生成位置X轴方向
{
	do
	{
		float pointX = rand();
		if (pointX <= visibleSize.width-45 && pointX>=45)
		{
			return pointX;
		}
	} while (1);
}

bool GameScene::onContactBegin(const PhysicsContact& contact)//刚体碰撞回调事件
{
	auto SpriteA = (Sprite*)contact.getShapeA()->getBody()->getNode();//获取碰撞精灵
	auto SpriteB = (Sprite*)contact.getShapeB()->getBody()->getNode();//获取碰撞精灵
	auto TagA = SpriteA->getTag();//获取碰撞精灵Tag
	auto TagB = SpriteB->getTag();//获取碰撞精灵Tag
	if (TagA == 106&& TagB == 104)//判断碰撞精灵B为敌机精灵
	{
		//修改分数
		score += 500;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//获取爆炸动画
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy1_down%d.png",i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteA->removeFromParent();
		//等爆炸动画播放完毕再执行remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create(SpriteB, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteB->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteB->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteB);
	}
	else if (TagA == 104 && TagB == 106)//判断碰撞精灵A为敌机精灵
	{
		//修改分数
		score += 500;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//获取爆炸动画
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy1_down%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteB->removeFromParent();
		//等爆炸动画播放完毕再执行remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create(SpriteA, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteA->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteA->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteA);
	}
	else if (TagA == 106 && TagB == 105)//判断碰撞精灵B为敌机精灵
	{
		//修改分数
		score += 1000;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//获取爆炸动画
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy2_down%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteA->removeFromParent();
		//等爆炸动画播放完毕再执行remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create(SpriteB, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteB->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteB->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteB);
	}
	else if (TagA == 105 && TagB == 106)//判断碰撞精灵A为敌机精灵
	{
		//修改分数
		score += 1000;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//获取爆炸动画
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy2_down%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteB->removeFromParent();
		//等爆炸动画播放完毕再执行remove
		Action* act = Sequence::create(Animate::create(Anime),CCCallFuncN::create(SpriteA, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteA->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteA->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteA);
	}
	else if (TagA == 103|| TagB == 103)//判断碰撞精灵是否为主角精灵
	{
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//获取主角精灵爆炸动画
		{
			auto str_name = String::createWithFormat("ui/shoot/hero_blowup_n%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.15);
		Anime->setLoops(1);
		//等爆炸动画播放完毕再执行remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create((Sprite*)this->getChildByTag(103), callfuncN_selector(GameScene::EnemyBonb)), NULL);
		auto hero = (Sprite*)this->getChildByTag(103);
		Gameover();//执行游戏结束函数
		hero->runAction(act);
	}
	return true;
}

void GameScene::EnemyBonb(Node* Sprite)//执行敌机精灵remove
{
	if (Sprite->getTag() == 104||Sprite->getTag()==105||Sprite->getTag()==103)
	{
		Sprite->removeFromParent();
	}
}

void GameScene::Gameover()//游戏结束函数
{
	//游戏结束所有update函数运行停止
	this->unschedule(schedule_selector(GameScene::update1));
	this->unschedule(schedule_selector(GameScene::update2));
	this->unschedule(schedule_selector(GameScene::update3));
	this->unschedule(schedule_selector(GameScene::update4));
	//Button* GameRetart = Button::create("Exit", "Airal", 35);//创建重新开始按钮
	//GameRetart->setPosition(Point(visibleSize.width/2,visibleSize.height/2));
	//addChild(GameRetart,11);
	//添加按钮触摸事件
	//GameRetart->addTouchEventListener(CC_CALLBACK_2(GameScene::RetartGameonTouch,this));
	auto exit = LabelTTF::create("Exit","Arial",25);
	exit->setColor(Color3B(45,45,45));
	exit->setPosition(Point(visibleSize.width/2-5,visibleSize.height/2-30));
	addChild(exit,11);
	TopScore();
	addMenu();
	listenerTouch->setEnabled(false);//停止飞机触控
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("sound/game_music.mp3");
}


void GameScene::addScore()//添加分数
{
	Score = LabelTTF::create("","Arial", 40);
	Score->setString(String::createWithFormat("Score:%d",score)->_string);
	Score->setAnchorPoint(ccp(0,1));
	Score->setPosition(Point(0,visibleSize.height));
	addChild(Score,25);
}


void GameScene::addMenu()
{
	auto menu = Menu::create();
	auto start = MenuItemImage::create("game_Reagain.png", "game_Reagain.png", "game_Reagain.png", CC_CALLBACK_1(GameScene::GameRetart, this));
	auto over = MenuItemImage::create("121.png", "121.png", "121.png",CC_CALLBACK_1(GameScene::GameExit,this));
	menu->addChild(start);
	menu->addChild(over);
	menu->alignItemsVerticallyWithPadding(20);
	menu->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	addChild(menu,10);
}

void GameScene::GameRetart(Ref* pSender)//游戏重启函数
{
	auto gameReagain = GameScene::createScene();//创建场景
	Director::getInstance()->replaceScene(gameReagain);//调用场景
}

void GameScene::GameExit(Ref* psender)//游戏结束退出选项回调函数
{
	Director::getInstance()->end();
}

void GameScene::UserScore()
{
	userDefault = UserDefault::getInstance();//创建用户数据
	int score = userDefault->getIntegerForKey("TopScore");//获取分数数据
	userDefault->setIntegerForKey("TopScore", score);//创建分数数据
}

void GameScene::TopScore()
{
	int TopScore = userDefault->getIntegerForKey("TopScore");//读取key为TopScore的数据
	if (TopScore < score)
	{
		userDefault->setIntegerForKey("TopScore", score);//记录最高分
		userDefault->flush();//写入数据
	}
	else
	{
		userDefault->setIntegerForKey("TopScore", TopScore);//记录最高分
		userDefault->flush();//写入数据
	}
	int Topscore = userDefault->getIntegerForKey("TopScore");//获取最高分数据
	auto TopscoreLabel = LabelTTF::create("","Arial",35);//创建显示最高分Label
	TopscoreLabel->setString(String::createWithFormat("TopScene:%d",Topscore)->_string);
	TopscoreLabel->setPosition(Point(visibleSize.width/2,visibleSize.height/2+80));
	addChild(TopscoreLabel,25);
}