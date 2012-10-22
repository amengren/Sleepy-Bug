//
//  SNSMenu.cpp
//  drawFree
//
//  Created by dhd on 12-4-1.
//  Copyright (c) 2012年 topgame. All rights reserved.
//

#include "SNSUIMenu.h"

#define MOVE_PIXEL (10) //移动超过这个像素的点 不触发按钮事件

bool SNSUIMenu::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    m_noEnded = false;
    return CCMenu::ccTouchBegan(touch, event);
}

void SNSUIMenu::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    if ( !m_noEnded ) {
        CCMenu::ccTouchEnded(touch, event);
    } else {
        CCMenu::ccTouchCancelled(touch, event);
    }
}

void SNSUIMenu::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{
    m_noEnded = false;
    CCMenu::ccTouchCancelled(touch, event);
}

void SNSUIMenu::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if ( !m_noEnded && ccpDistance(touch->locationInView(), touch->previousLocationInView()) > MOVE_PIXEL ) {
        m_noEnded = true;
    }
    
    CCMenu::ccTouchMoved(touch, event);
}



SNSUIMenu* SNSUIMenu::menuWithItems(CCMenuItem* item, ...)
{
    va_list args;
    va_start(args,item);
    SNSUIMenu *pRet = new SNSUIMenu();
    if (pRet && pRet->initWithItems(item, args))
    {
        pRet->autorelease();
        va_end(args);
        return pRet;
    }
    va_end(args);
    CC_SAFE_DELETE(pRet);
    return NULL;
}

SNSUIMenu* SNSUIMenu::menuWithItem(CCMenuItem* item)
{
    return menuWithItems(item, NULL);
}



void SNSUIMenu::onEnter()
{
    CCMenu::onEnter();
    CCTouchDispatcher::sharedDispatcher()->removeDelegate(this);
    CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, -301, true);
}
