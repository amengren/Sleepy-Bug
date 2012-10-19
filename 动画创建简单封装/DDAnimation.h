//
//  DDAnimation.h
//
//  Created by ding huidong on 12-7-18.
//  Copyright (c) 2012年 dhd. All rights reserved.
//

#ifndef dazaxie_DDAnimation_h
#define dazaxie_DDAnimation_h

#include "cocos2d.h"
USING_NS_CC;

class DDAnimation : public CCSprite {
protected:
    CCAnimation* mAnimation;
    
protected:
    bool init(const char* item, va_list args);
    bool init(const char* item, int min, int max);
    
public:
    virtual ~DDAnimation();
    
    //传入图片名称列表
    static DDAnimation* create(const char* item, ...);
    
    //传入名称，最小后缀-》最大后缀
    static DDAnimation* create(const char* item, int min, int max);
    
public:
    //设置新的资源替换掉现有的
    void setNewAnimation(const char* item, ...);
    
public:
    //播放动画（间隔）
    void play(float duration);
    //停止播放
    void stop();
    //播放动画（间隔，次数，播完后执行函数）
    void playCount(float duration, int _count);
    void playCount(float duration, int _count, CCCallFuncND* _func);
};

#endif
