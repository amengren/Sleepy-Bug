//
//  SNSTableViewCell.cpp
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#include "SNSTableViewCell.h"

SNSTableViewCell::SNSTableViewCell()
:m_identifier(NULL), m_indexPath(NULL)
{}

SNSTableViewCell::~SNSTableViewCell()
{
    CC_SAFE_RELEASE_NULL(m_identifier);
	setIndexPath(NULL);
}

bool SNSTableViewCell::initTableViewCell(const char* CellIdentifier)
{
    if ( !CCLayer::init() ) {
        return false;
    }
//    if ( !CCLayerColor::initWithColor(ccc4(255, 255, 255, 0)) ) {
//        return false;
//    }
    
    CC_SAFE_DELETE(m_identifier);
    m_identifier = CCString::create(CellIdentifier);
    m_identifier->retain();
    return true;
}
SNSTableViewCell* SNSTableViewCell::initWithReuseIdentifier(const char* CellIdentifier)
{
    return SNSTableViewCell::create(CellIdentifier);
}

SNSTableViewCell* SNSTableViewCell::create(const char* CellIdentifier)
{
    SNSTableViewCell* pRet = new SNSTableViewCell();
    
    if ( pRet && pRet->initTableViewCell(CellIdentifier) ) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

#pragma mark - setContentSize

void SNSTableViewCell::setContentSize(CCSize contentSize)
{
	CCLayer::setContentSize(contentSize);
	CCArray *childs = this->getChildren();
	// 单个item位置纠正
	if (childs && childs->count() == 1) {
		CCNode *child = (CCNode *)childs->objectAtIndex(0);
		if (child) {
			child->setPosition(contentSize.width * 0.5f, contentSize.height * 0.5f);
		}
	}
}
