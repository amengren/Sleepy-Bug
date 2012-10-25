//
//  SNSAlertView.h
//  WarCraftCardGame
//
//  Created by ourpalm on 10/20/12.
//
//

#ifndef WarCraftCardGame_SNSAlertView_h
#define WarCraftCardGame_SNSAlertView_h

#include "SNSView.h"

class SNSAlertStack : public CCObject
{
	
};

class SNSAlertView : public CCNode
{
public:
	SNSAlertView();
	~SNSAlertView();
	virtual bool init();

public:
	CC_DEPRECATED_ATTRIBUTE static SNSAlertView* initWithNode(CCNode *node);
	static SNSAlertView* create(CCNode *node);
};

#endif
