//
//  SNSMenu.h
//  drawFree
//
//  Created by dhd on 12-4-1.
//  Copyright (c) 2012年 topgame. All rights reserved.
//

#ifndef DRAWFREE_SNSUIMENU_H
#define DRAWFREE_SNSUIMENU_H

#include "cocos2d.h"
USING_NS_CC;

/*
 重载CCMenu 跟SNSTableView配合使用
 */
class SNSUIMenu : public CCMenu {
protected:
    bool m_noEnded; //移动超出多少范围后，不再响应按钮
    
public:/*重载*/
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    
    int priorty;
    virtual void onEnter();
public:
    static SNSUIMenu* menuWithItems(CCMenuItem* item, ...);
    static SNSUIMenu* menuWithItem(CCMenuItem* item);
};

#endif // DRAWFREE_SNSUIMENU_H
