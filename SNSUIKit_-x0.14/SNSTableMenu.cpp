//
//  SNSMenu.cpp
//  drawFree
//
//  Created by dhd on 12-4-1.
//  Copyright (c) 2012年 topgame. All rights reserved.
//

#include "SNSTableMenu.h"

#define MOVE_PIXEL (10) //移动超过这个像素的点 不触发按钮事件

SNSTableMenu::SNSTableMenu()
{
    m_priority = kCCMenuHandlerPriority+1;
}

bool SNSTableMenu::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    m_noEnded = false;
    return CCMenu::ccTouchBegan(touch, event);
}

void SNSTableMenu::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    if ( !m_noEnded ) {
        CCMenu::ccTouchEnded(touch, event);
    } else {
        CCMenu::ccTouchCancelled(touch, event);
    }
}

void SNSTableMenu::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{
    m_noEnded = false;
    CCMenu::ccTouchCancelled(touch, event);
}

void SNSTableMenu::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if ( !m_noEnded && ccpDistance(touch->getLocation(), touch->getPreviousLocation()) > MOVE_PIXEL ) {
        m_noEnded = true;
    }
    
    CCMenu::ccTouchMoved(touch, event);
}

SNSTableMenu* SNSTableMenu::create(CCMenuItem* item, ...)
{
    va_list args;
    va_start(args,item);
    SNSTableMenu *pRet = new SNSTableMenu();
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

SNSTableMenu* SNSTableMenu::createWithItem(CCMenuItem* item)
{
    return create(item, NULL);
}

void SNSTableMenu::registerWithTouchDispatcher(void)
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, m_priority, true);
}

void SNSTableMenu::setMenuHandlerPriority(int _priority)
{
    m_priority = _priority;
}

