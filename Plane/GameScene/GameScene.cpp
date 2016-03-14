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
	//������Ϸ��������
	SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/game_music.mp3",true);
	addbackground();//��ӱ���
	addHero();//������ǻ�
	UserScore();//�����û���������
	schedule(schedule_selector(GameScene::update1));//��������Update����
	schedule(schedule_selector(GameScene::update2));//�����ӵ�Update����
	schedule(schedule_selector(GameScene::update3), 0.4);//������ҷɻ�Update����
	schedule(schedule_selector(GameScene::update4), 0.4);//�����л�Update����
	auto contactListener = EventListenerPhysicsContact::create();//����������ײ�����¼�
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin,this);//��ײ�ص��¼�
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);//�����ײ����
	addScore();//��ӷ�������
	return true;
}

void GameScene::addHero()//������ǻ�
{
	auto Anime = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		auto str_name = String::createWithFormat("ui/shoot/hero%d.png",i); // ��ȡ��ҷɻ�ͼƬ��ַ
		Anime->addSpriteFrameWithFile(str_name->getCString());//����ͼƬ��ַת��Ϊ�ַ�������
	}
	Anime->setDelayPerUnit(0.1);
	Anime->setLoops(-1);

	//�����ҷɻ�����
	Hero = Sprite::create("ui/shoot/hero1.png");
	Hero->setTag(103);
	Size visibleSize = Director::sharedDirector()->getVisibleSize();
	Hero->setPosition(Point(visibleSize.width*0.5, visibleSize.height*0.3));
	Hero->runAction(Animate::create(Anime));
	addChild(Hero,10);
	auto Hero_body = PhysicsBody::createCircle(35);
	Hero_body->setGravityEnable(false);//���÷ɻ���������
	Hero_body->setDynamic(true);//����Ϊ���Ա�����������
	Hero_body->setContactTestBitmask(1);//������������Ϊ1���ܼ�⵽��ͬ��������ײ
	Hero_body->setCategoryBitmask(0x0004);
	Hero_body->setContactTestBitmask(0x006);
	Hero_body->setCollisionBitmask(0x0006);
	Hero->setPhysicsBody(Hero_body);
	listenerTouch = EventListenerTouchOneByOne::create();//��ӵ��㴥���¼�
	listenerTouch->setSwallowTouches(true);
	listenerTouch->onTouchBegan= CC_CALLBACK_2(GameScene::HeroTouchBegan, this);//��ӵ��㴥����ʼ�¼��ص�
	listenerTouch->onTouchMoved = CC_CALLBACK_2(GameScene::HeroTouchMoved, this);//��ӵ��㴥���ƶ��¼��ص�
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, Hero);
}

void GameScene::addbackground()//��ӱ�������
{
	background1 = Sprite::create("ui/shoot_background/background.png");
	background1->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	addChild(background1);
	Size mapSize = background1->getContentSize();
	background2 = Sprite::create("ui/shoot_background/background.png");
	background2->setPosition(Point(visibleSize.width / 2, mapSize.height));
	addChild(background2);
}

void GameScene::addBullet()//����ӵ�����
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

void GameScene::addEnemy()//��ӵл�����
{
	//���2�ֵл�
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
	EnemyList.pushBack(enemy);//����Vector
	//������Ӧ����
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

void GameScene::update1(float delta)//�����ƶ�update����
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

void GameScene::update2(float delta)//�ӵ��͵л��ƶ�update����
{
	for (int i = 0; i < bulletList.size(); i++)
	{	
		auto Bullet = bulletList.at(i);
		if (Bullet->getPositionY() > visibleSize.height)
		{
			Bullet->removeFromParent(); //�Ӳ����Ƴ�
			bulletList.eraseObject(Bullet);//�Ӽ�¼�����ӵ���vector���Ƴ�
			i--;
		}
		else
		{
			auto bullet_speed = 10;
			Bullet->setPositionY(Bullet->getPositionY()+bullet_speed);
		}
	}
	for (int i = 0; i < EnemyList.size(); i++)//����Vector�ĳ�Ա
	{
		auto enemy = EnemyList.at(i);
		enemy->setPositionY(enemy->getPositionY() - 5);
		if (enemy->getPositionY() < 0)//������Ļ��Χ���Ƴ��л�����
		{
			enemy->removeFromParent();
			EnemyList.eraseObject(enemy);
			i--;
		}
	}
}

void GameScene::update3(float dalta)//�����ӵ�update�ĺ���
{
	addBullet();
}
void GameScene::update4(float delta)//���ɵл�update�ĺ���
{
	GameScene::addEnemy();
}
bool GameScene::HeroTouchBegan(Touch* touch, Event* event)//���Ǿ��鵥�㴥�ؿ�ʼ�ص��¼�
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

void GameScene::HeroTouchMoved(Touch* touch, Event* event)//���Ǿ��鵥�㴥���ƶ��ص��¼�
{
	auto targt = static_cast<Sprite*>(event->getCurrentTarget());
	targt->setPosition(touch->getLocation());
}



float GameScene::randomPointX()//����л���������λ��X�᷽��
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

bool GameScene::onContactBegin(const PhysicsContact& contact)//������ײ�ص��¼�
{
	auto SpriteA = (Sprite*)contact.getShapeA()->getBody()->getNode();//��ȡ��ײ����
	auto SpriteB = (Sprite*)contact.getShapeB()->getBody()->getNode();//��ȡ��ײ����
	auto TagA = SpriteA->getTag();//��ȡ��ײ����Tag
	auto TagB = SpriteB->getTag();//��ȡ��ײ����Tag
	if (TagA == 106&& TagB == 104)//�ж���ײ����BΪ�л�����
	{
		//�޸ķ���
		score += 500;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//��ȡ��ը����
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy1_down%d.png",i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteA->removeFromParent();
		//�ȱ�ը�������������ִ��remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create(SpriteB, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteB->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteB->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteB);
	}
	else if (TagA == 104 && TagB == 106)//�ж���ײ����AΪ�л�����
	{
		//�޸ķ���
		score += 500;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//��ȡ��ը����
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy1_down%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteB->removeFromParent();
		//�ȱ�ը�������������ִ��remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create(SpriteA, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteA->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteA->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteA);
	}
	else if (TagA == 106 && TagB == 105)//�ж���ײ����BΪ�л�����
	{
		//�޸ķ���
		score += 1000;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//��ȡ��ը����
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy2_down%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteA->removeFromParent();
		//�ȱ�ը�������������ִ��remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create(SpriteB, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteB->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteB->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteB);
	}
	else if (TagA == 105 && TagB == 106)//�ж���ײ����AΪ�л�����
	{
		//�޸ķ���
		score += 1000;
		Score->setString(String::createWithFormat("Score:%d", score)->_string);
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//��ȡ��ը����
		{
			auto str_name = String::createWithFormat("ui/shoot/enemy2_down%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.1);
		Anime->setLoops(1);
		SpriteB->removeFromParent();
		//�ȱ�ը�������������ִ��remove
		Action* act = Sequence::create(Animate::create(Anime),CCCallFuncN::create(SpriteA, callfuncN_selector(GameScene::EnemyBonb)), NULL);
		SpriteA->runAction(act);
		auto particleSystem = ParticleSystemQuad::create("particle_texture.plist");
		particleSystem->setPosition(SpriteA->getPosition());
		addChild(particleSystem);
		EnemyList.eraseObject(SpriteA);
	}
	else if (TagA == 103|| TagB == 103)//�ж���ײ�����Ƿ�Ϊ���Ǿ���
	{
		auto Anime = Animation::create();
		for (int i = 1; i < 5; i++)//��ȡ���Ǿ��鱬ը����
		{
			auto str_name = String::createWithFormat("ui/shoot/hero_blowup_n%d.png", i);
			Anime->addSpriteFrameWithFile(str_name->getCString());
		}
		Anime->setDelayPerUnit(0.15);
		Anime->setLoops(1);
		//�ȱ�ը�������������ִ��remove
		Action* act = Sequence::create(Animate::create(Anime), CCCallFuncN::create((Sprite*)this->getChildByTag(103), callfuncN_selector(GameScene::EnemyBonb)), NULL);
		auto hero = (Sprite*)this->getChildByTag(103);
		Gameover();//ִ����Ϸ��������
		hero->runAction(act);
	}
	return true;
}

void GameScene::EnemyBonb(Node* Sprite)//ִ�ел�����remove
{
	if (Sprite->getTag() == 104||Sprite->getTag()==105||Sprite->getTag()==103)
	{
		Sprite->removeFromParent();
	}
}

void GameScene::Gameover()//��Ϸ��������
{
	//��Ϸ��������update��������ֹͣ
	this->unschedule(schedule_selector(GameScene::update1));
	this->unschedule(schedule_selector(GameScene::update2));
	this->unschedule(schedule_selector(GameScene::update3));
	this->unschedule(schedule_selector(GameScene::update4));
	//Button* GameRetart = Button::create("Exit", "Airal", 35);//�������¿�ʼ��ť
	//GameRetart->setPosition(Point(visibleSize.width/2,visibleSize.height/2));
	//addChild(GameRetart,11);
	//��Ӱ�ť�����¼�
	//GameRetart->addTouchEventListener(CC_CALLBACK_2(GameScene::RetartGameonTouch,this));
	auto exit = LabelTTF::create("Exit","Arial",25);
	exit->setColor(Color3B(45,45,45));
	exit->setPosition(Point(visibleSize.width/2-5,visibleSize.height/2-30));
	addChild(exit,11);
	TopScore();
	addMenu();
	listenerTouch->setEnabled(false);//ֹͣ�ɻ�����
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("sound/game_music.mp3");
}


void GameScene::addScore()//��ӷ���
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

void GameScene::GameRetart(Ref* pSender)//��Ϸ��������
{
	auto gameReagain = GameScene::createScene();//��������
	Director::getInstance()->replaceScene(gameReagain);//���ó���
}

void GameScene::GameExit(Ref* psender)//��Ϸ�����˳�ѡ��ص�����
{
	Director::getInstance()->end();
}

void GameScene::UserScore()
{
	userDefault = UserDefault::getInstance();//�����û�����
	int score = userDefault->getIntegerForKey("TopScore");//��ȡ��������
	userDefault->setIntegerForKey("TopScore", score);//������������
}

void GameScene::TopScore()
{
	int TopScore = userDefault->getIntegerForKey("TopScore");//��ȡkeyΪTopScore������
	if (TopScore < score)
	{
		userDefault->setIntegerForKey("TopScore", score);//��¼��߷�
		userDefault->flush();//д������
	}
	else
	{
		userDefault->setIntegerForKey("TopScore", TopScore);//��¼��߷�
		userDefault->flush();//д������
	}
	int Topscore = userDefault->getIntegerForKey("TopScore");//��ȡ��߷�����
	auto TopscoreLabel = LabelTTF::create("","Arial",35);//������ʾ��߷�Label
	TopscoreLabel->setString(String::createWithFormat("TopScene:%d",Topscore)->_string);
	TopscoreLabel->setPosition(Point(visibleSize.width/2,visibleSize.height/2+80));
	addChild(TopscoreLabel,25);
}