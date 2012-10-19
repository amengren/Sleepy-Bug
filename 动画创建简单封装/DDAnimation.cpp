//
//  DDAnimation.cpp
//
//  Created by ding huidong on 12-7-18.
//  Copyright (c) 2012年 dhd. All rights reserved.
//

#include "DDAnimation.h"

#define ACTION_PLAY_ANIMATE_TAG (10) //播放动画动作

DDAnimation::~DDAnimation()
{
    CC_SAFE_RELEASE(mAnimation);
}

DDAnimation* DDAnimation::create(const char* item, ...)
{
    va_list args;
    va_start(args,item);
    
    DDAnimation* pRet = new DDAnimation();
    
    if ( pRet && pRet->init(item, args) ) {
        pRet->autorelease();
        va_end(args);
        return pRet;
    }
    
    va_end(args);
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool DDAnimation::init(const char* item, va_list args)
{
    if (item)
    {
        CCArray *pFrames = CCArray::createWithCapacity(6);
        
        //第一个值
        CCSprite* pSpr = CCSprite::create(item);
        pFrames->addObject(pSpr->displayFrame());
        
        //循环读取下一个值
        const char *i = va_arg(args, const char*);
        while (i)
        {
            if ( (pSpr = CCSprite::create(i)) ) {
                pFrames->addObject(pSpr->displayFrame());
            }
            i = va_arg(args, const char*);
        }
        
        //创建动画
        mAnimation = CCAnimation::createWithSpriteFrames(pFrames);
        mAnimation->retain();
        
        //初始化第一帧
        if ( !CCSprite::initWithSpriteFrame((CCSpriteFrame*)pFrames->objectAtIndex(0)) ) {
            return false;
        }
        
        return true;
    }
        
    return false;
}

DDAnimation* DDAnimation::create(const char* item, int min, int max)
{
    DDAnimation* pRet = new DDAnimation();
    
    if ( pRet && pRet->init(item, min, max) ) {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool DDAnimation::init(const char* item, int min, int max)
{
    if (item)
    {
        CCArray *pFrames = CCArray::createWithCapacity(max-min+2);
        
        char tmpChar[32] = {0};
        CCSprite* pSpr = NULL;
        for (int i=min; i<=max; i++) {
            sprintf(tmpChar, "%s%d.png", item, i);
            if ( (pSpr = CCSprite::create(tmpChar)) ) {
                pFrames->addObject(pSpr->displayFrame());
            }
        }
        
        //创建动画
        mAnimation = CCAnimation::createWithSpriteFrames(pFrames);
        mAnimation->retain();
        
        //初始化第一帧
        if ( !CCSprite::initWithSpriteFrame((CCSpriteFrame*)pFrames->objectAtIndex(0)) ) {
            return false;
        }
        
        return true;
    }
    
    return false;
}

void DDAnimation::setNewAnimation(const char* item, ...)
{
    this->stop();
    CC_SAFE_RELEASE(mAnimation);
    
    CCArray *pFrames = CCArray::createWithCapacity(6);
    
    va_list args;
    va_start(args, item);
    
    //第一个值
    CCSprite* pSpr = CCSprite::create(item);
    pFrames->addObject(pSpr->displayFrame());
    
    //循环读取下一个值
    const char *i = va_arg(args, const char*);
    while (i)
    {
        if ( (pSpr = CCSprite::create(i)) ) {
            pFrames->addObject(pSpr->displayFrame());
        }
        i = va_arg(args, const char*);
    }
    
    //创建动画
    mAnimation = CCAnimation::createWithSpriteFrames(pFrames);
    mAnimation->retain();
    
    va_end(args);
}

void DDAnimation::play(float duration)
{
    stop();
    CCAnimate* pAnimate = CCAnimate::create(mAnimation);
    pAnimate->setDuration(duration);
    CCRepeatForever *pRepeat = CCRepeatForever::create(pAnimate);
    pRepeat->setTag(ACTION_PLAY_ANIMATE_TAG);
    this->runAction(pRepeat);
}

void DDAnimation::playCount(float duration, int _count, CCCallFuncND* _func)
{
    stop();
    CCAnimate* pAnimate = CCAnimate::create(mAnimation);
    pAnimate->setDuration(duration);
    CCRepeat *pRepeat = CCRepeat::create(pAnimate, _count);
    pRepeat->setTag(ACTION_PLAY_ANIMATE_TAG);
    this->runAction(CCSequence::createWithTwoActions(pRepeat, _func));
}

void DDAnimation::playCount(float duration, int _count)
{
    playCount(duration, _count, NULL);
}

void DDAnimation::stop()
{
    this->stopActionByTag(ACTION_PLAY_ANIMATE_TAG);
}
