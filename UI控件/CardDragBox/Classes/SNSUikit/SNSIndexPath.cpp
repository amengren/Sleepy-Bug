//
//  SNSIndexPath.cpp
//  tableView
//	version 2.0
//  Created by Jie Yang on 12/7/11.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#include "SNSIndexPath.h"

bool SNSIndexPath::initIndexPath(unsigned int row, unsigned int column, unsigned int section)
{
    m_row = row;
    m_column = column;
    m_section = section;
    return true;
}

SNSIndexPath* SNSIndexPath::indexPathForRow(unsigned int row, unsigned int section) 
{
	return SNSIndexPath::create(row, section);
}

SNSIndexPath* SNSIndexPath::indexPathForRow(unsigned int row, unsigned int column, unsigned int section) 
{
	return SNSIndexPath::create(row, column, section);
}

SNSIndexPath* SNSIndexPath::create(unsigned int row, unsigned int section)
{
    SNSIndexPath* pRet = new SNSIndexPath();
    
    if ( pRet && pRet->initIndexPath(row, 0, section) ) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
	return NULL;
}

SNSIndexPath* SNSIndexPath::create(unsigned int row, unsigned int column, unsigned int section)
{
    SNSIndexPath* pRet = new SNSIndexPath();
    
    if ( pRet && pRet->initIndexPath(row, column, section) ) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
	return NULL;
}

const char* SNSIndexPath::description()
{
    char des[30];
    sprintf(des, "<Row = %d | Column = %d | Section = %d>", m_row, m_column, m_section);
    CCString* pRet = new CCString(des);
    pRet->autorelease();
	return (pRet->m_sString.c_str());
}
