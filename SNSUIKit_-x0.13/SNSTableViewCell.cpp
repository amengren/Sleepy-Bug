//
//  SNSTableViewCell.cpp
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#include "SNSTableViewCell.h"

SNSTableViewCell::SNSTableViewCell()
:m_identifier(NULL)
{}

SNSTableViewCell::~SNSTableViewCell()
{
    CC_SAFE_DELETE(m_identifier);
}

bool SNSTableViewCell::initTableViewCell(const char* CellIdentifier)
{
//    if ( !CCLayer::init() ) {
//        return false;
//    }
    if ( !CCLayerColor::initWithColor(ccc4f(255, 255, 255, 0)) ) {
        return false;
    }
     
    CC_SAFE_DELETE(m_identifier);
    m_identifier = new CCString(CellIdentifier);
    
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
