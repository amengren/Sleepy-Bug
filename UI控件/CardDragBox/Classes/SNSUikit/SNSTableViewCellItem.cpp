//
//  SNSTableViewCellItem.cpp
//  tableView
//	version 2.0
//  Created by Jie Yang on 12/7/11.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#include "SNSTableViewCellItem.h"

bool SNSTableViewCellItem::initCellItem()
{
//    if ( !CCLayerColor::initWithColor(ccc4(0, 0, 1+rand()%255, 0)) ) {
//        return false;
//    }
	if (!CCLayer::init()) {
		return false;
	}
    
    return true;
}

SNSTableViewCellItem* SNSTableViewCellItem::initItem()
{
    return SNSTableViewCellItem::create();
}

SNSTableViewCellItem* SNSTableViewCellItem::create()
{
	SNSTableViewCellItem* pRet = new SNSTableViewCellItem();
    
    if ( pRet && pRet->initCellItem() ) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
