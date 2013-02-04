//
//  SNSUIView.h
//  SNSUIKit
//	version 2.0
//  Created by Jie Yang on 1/13/12.
//  Copyright 2012 ourpalm.com. All rights reserved.
//

#ifndef _SNSUIKIT_SNSUIVIEW_H_
#define _SNSUIKIT_SNSUIVIEW_H_

#include "cocos2d.h"

USING_NS_CC;

class SNSView : public CCLayer {
public:
	CC_PROPERTY(CCRect, m_frame, Frame);
	CC_PROPERTY(CCSize,	m_bodySize, BodySize);
    
protected:
    bool initViewFrame(CCRect frame);
	
public:
	CC_DEPRECATED_ATTRIBUTE static SNSView* initWithFrame(CCRect frame);
	static SNSView* create(CCRect frame);
    
};

#endif // _SNSUIKIT_SNSUIVIEW_H_
