//
//  SNSIndexPath.h
//  tableView
//	version 2.0
//  Created by Jie Yang on 12/7/11.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#ifndef _SNSUIKIT_SNSINDEXPATH_H_
#define _SNSUIKIT_SNSINDEXPATH_H_

#include "cocos2d.h"

USING_NS_CC;

/*
 */
class SNSIndexPath : public CCObject 
{
public:
	CC_SYNTHESIZE(unsigned int,	m_row, Row);
	CC_SYNTHESIZE(unsigned int,	m_column, Column);
	CC_SYNTHESIZE(unsigned int,	m_section, Section);
    
protected:
    SNSIndexPath():m_row(0),m_column(0),m_section(0){};
    virtual ~SNSIndexPath(){};
    bool initIndexPath(unsigned int row, unsigned int column, unsigned int section);
    
public:
    CC_DEPRECATED_ATTRIBUTE static SNSIndexPath* indexPathForRow(unsigned int row, unsigned int section);
    CC_DEPRECATED_ATTRIBUTE static SNSIndexPath* indexPathForRow(unsigned int row, unsigned int column, unsigned int section);
	static SNSIndexPath* create(unsigned int row, unsigned int section);
    static SNSIndexPath* create(unsigned int row, unsigned int column, unsigned int section);
    //打印
    const char* description();
};

#endif // _SNSUIKIT_SNSINDEXPATH_H_
