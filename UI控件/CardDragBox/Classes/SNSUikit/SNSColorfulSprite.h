//
//  SNSColorSprite.h
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#ifndef __MM_GRAY_SCALE_SPRITE_H__
#define __MM_GRAY_SCALE_SPRITE_H__

#include "sprite_nodes/CCSprite.h"
#include "textures/CCTexture2D.h"
#include "cocoa/CCGeometry.h"
#include "cocos2d.h"

USING_NS_CC;

class SNSColorfulSprite : public CCSprite
{
public:
	SNSColorfulSprite();
	virtual ~SNSColorfulSprite();
	static SNSColorfulSprite* create(const char* pszFileName);
	static SNSColorfulSprite* createWithSpriteFrameName(const char* pszFileName);
	bool initWithTexture(CCTexture2D* pTexture, const CCRect& tRect);
	virtual void draw();

};

#endif // __MM_GRAY_SCALE_SPRITE_H__