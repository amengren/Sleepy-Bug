//
//  SNSMenu.h
//  drawFree
//
//  Created by dhd on 12-4-1.
//  Copyright (c) 2012年 topgame. All rights reserved.
//

#ifndef DRAWFREE_SNSTableMenu_H
#define DRAWFREE_SNSTableMenu_H

#include "cocos2d.h"
USING_NS_CC;

/*
 重载CCMenu 跟SNSTableView配合使用
 */
class SNSTableMenu : public CCMenu {
protected:
    SNSTableMenu();
    
protected:
    bool m_noEnded; //移动超出多少范围后，不再响应按钮
    int m_priority;
    
public:/*重载*/
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    
    virtual void registerWithTouchDispatcher(void);
    void setMenuHandlerPriority(int _priority);
    
public:
    static SNSTableMenu* create(CCMenuItem* item, ...);
    static SNSTableMenu* createWithItem(CCMenuItem* item);
};

#endif // DRAWFREE_SNSTableMenu_H
