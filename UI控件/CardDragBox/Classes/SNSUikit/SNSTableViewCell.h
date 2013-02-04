//
//  SNSTableViewCell.h
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#ifndef _SNSUIKIT_SNSTABLEVIEWCELL_H_
#define _SNSUIKIT_SNSTABLEVIEWCELL_H_

#include "cocos2d.h"
#include "SNSIndexPath.h"

USING_NS_CC;


class SNSTableViewCell : public CCLayer //public CCLayer
{
public:
	CCString*       m_identifier;
    
protected:
    SNSTableViewCell();
    virtual ~SNSTableViewCell();
    bool initTableViewCell(const char* CellIdentifier);
    
public:
	CC_DEPRECATED_ATTRIBUTE static SNSTableViewCell* initWithReuseIdentifier(const char* CellIdentifier);
	static SNSTableViewCell* create(const char* CellIdentifier);
	
public:
	CC_SYNTHESIZE_RETAIN(SNSIndexPath*, m_indexPath, IndexPath);
	
	// 设置变形
	virtual void setContentSize(CCSize contentSize);
};

#endif // _SNSUIKIT_SNSTABLEVIEWCELL_H_
