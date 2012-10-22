//
//  SNSUIButton.h
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#ifndef DRAWFREE_SNSUIMENU_H
#define DRAWFREE_SNSUIMENU_H

#include "cocos2d.h"
USING_NS_CC;

/*
 重载CCMenu 跟SNSTableView配合使用
 */
class SNSButton : public CCMenu {
protected:
    bool m_noEnded; //移动超出多少范围后，不再响应按钮
    
public:/*重载*/
	virtual void registerWithTouchDispatcher();
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    
public:
    CC_DEPRECATED_ATTRIBUTE static SNSButton* menuWithItems(CCMenuItem *item, ...);
    CC_DEPRECATED_ATTRIBUTE static SNSButton* menuWithItem(CCMenuItem *item);
	static SNSButton* create(CCMenuItem *item, ...);
	static SNSButton* create(CCMenuItem *item);
};

#endif // DRAWFREE_SNSUIMENU_H
