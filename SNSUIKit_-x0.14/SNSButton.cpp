//
//  SNSUIButton.cpp
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#include "SNSButton.h"

#define MOVE_PIXEL (10) //移动超过这个像素的点 不触发按钮事件

SNSButton* SNSButton::menuWithItems(CCMenuItem* item, ...)
{
	va_list args;
    va_start(args,item);
    SNSButton *pRet = new SNSButton();
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

SNSButton* SNSButton::menuWithItem(CCMenuItem* item)
{
    return create(item, NULL);
}

SNSButton* SNSButton::create(CCMenuItem *item, ...)
{
	va_list args;
    va_start(args,item);
    SNSButton *pRet = new SNSButton();
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

SNSButton* SNSButton::create(CCMenuItem *item)
{
	return create(item, NULL);
}

void SNSButton::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, m_handlerPriority + 1, true);
}

bool SNSButton::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    m_noEnded = false;
	
	return CCMenu::ccTouchBegan(touch, event);
}

void SNSButton::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    if ( !m_noEnded ) {
        CCMenu::ccTouchEnded(touch, event);
    } else {
        CCMenu::ccTouchCancelled(touch, event);
    }
}

void SNSButton::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{
    m_noEnded = false;
    CCMenu::ccTouchCancelled(touch, event);
}

void SNSButton::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if ( !m_noEnded && ccpDistance(touch->getLocation(), touch->getPreviousLocation()) > MOVE_PIXEL ) {
        m_noEnded = true;
    }
    
    CCMenu::ccTouchMoved(touch, event);
}
