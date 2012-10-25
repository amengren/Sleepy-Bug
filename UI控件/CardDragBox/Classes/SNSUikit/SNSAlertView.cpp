//
//  SNSAlertView.cpp
//  WarCraftCardGame
//
//  Created by ourpalm on 10/20/12.
//
//

#include "SNSAlertView.h"

SNSAlertView::SNSAlertView()
{
	
}

SNSAlertView::~SNSAlertView()
{
	
}

bool SNSAlertView::init()
{
	return true;
}

SNSAlertView* SNSAlertView::initWithNode(CCNode *node)
{
	return create(node);
}

SNSAlertView* SNSAlertView::create(CCNode *node)
{
	SNSAlertView* pRet = new SNSAlertView();
	
	if ( pRet && pRet->init() ) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}