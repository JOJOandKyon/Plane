#include "AppDelegate.h"
#include "StartScene/StartScene.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AgentManager.h"
using namespace anysdk::framework;
#endif
USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    AgentManager::getInstance()->unloadAllPlugins();
#endif
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    /**
     * appKey、appSecret、privateKey需要从打包工具中游戏管理界面获取，替换
     * oauthLoginServer参数是游戏服务提供的用来做登陆验证转发的接口地址。
     */
    std::string oauthLoginServer = "OAUTH_LOGIN_SERVER";
    std::string appKey = "APP_KEY";
    std::string appSecret = "APP_SERCRET";
    std::string privateKey = "PRIVATE_KEY";
    
    AgentManager* pAgent = AgentManager::getInstance();
    pAgent->init(appKey,appSecret,privateKey,oauthLoginServer);
    
    //使用框架中代理类进行插件初始化
    pAgent->loadAllPlugins();
#endif
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::createWithRect("Plane", Rect(0, 0, 500, 640));
        director->setOpenGLView(glview);
    }

    director->getOpenGLView()->setDesignResolutionSize(480, 852, ResolutionPolicy::SHOW_ALL);

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    FileUtils::getInstance()->addSearchPath("res");

    // create a scene. it's an autorelease object
    auto scene = Start::createScene();

    // run
    director->runWithScene(scene);
	SimpleAudioEngine::getInstance() ->preloadBackgroundMusic("sound/game_music.mp3");
	//SimpleAudioEngine::getInstance()->stopBackgroundMusic("sound/game_music.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/get_bomb.wav");
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
