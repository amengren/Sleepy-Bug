//
//  SNSUIView.cpp
//  SNSUIKit
//	version 2.0
//  Created by Jie Yang on 1/13/12.
//  Copyright 2012 ourpalm.com. All rights reserved.
//

#include "SNSView.h"

bool SNSView::initViewFrame(CCRect frame)
{
    if ( !CCLayer::init() ) {
        return false;
    }
    setFrame(frame);
    return true;
}

SNSView* SNSView::initWithFrame(CCRect frame)
{
	return SNSView::create(frame);
}

SNSView* SNSView::create(CCRect frame)
{
	SNSView* pRet = new SNSView();
    
    if ( pRet && pRet->initViewFrame(frame) ) {
        pRet->autorelease();
        return pRet;
    }
	CC_SAFE_DELETE(pRet);
    return NULL;
}

CCRect SNSView::getFrame()
{
    return m_frame;
}

void SNSView::setFrame(CCRect var)
{
    m_frame = var;
}

CCSize SNSView::getBodySize()
{
    return m_bodySize;
}

void SNSView::setBodySize(CCSize var)
{
    m_bodySize = var;
}
