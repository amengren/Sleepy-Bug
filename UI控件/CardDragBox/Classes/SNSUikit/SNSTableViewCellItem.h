//
//  SNSTableViewCellItem.h
//  tableView
//	version 2.0
//  Created by Jie Yang on 12/7/11.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#ifndef _SNSUIKIT_SNSTABLEVIEWCELLITEM_H_
#define _SNSUIKIT_SNSTABLEVIEWCELLITEM_H_

#include "cocos2d.h"
#include "SNSIndexPath.h"

USING_NS_CC;

class SNSTableViewCellItem : public CCLayer
{
protected:
    SNSTableViewCellItem():m_indexPath(NULL), m_isSelected(false) {}
    virtual ~SNSTableViewCellItem() {
		setIndexPath(NULL);
	}
    virtual bool initCellItem();
	
public:
    CC_DEPRECATED_ATTRIBUTE static SNSTableViewCellItem* initItem();
	static SNSTableViewCellItem* create();
	
	CC_SYNTHESIZE_RETAIN(SNSIndexPath*, m_indexPath, IndexPath);

public:
	// 设置选中状态
	virtual void setSelected(bool isSelected){m_isSelected = isSelected;};
														// 设置选中状态
	virtual	bool getSelected(){return m_isSelected;};	// 获得选中状态
	bool			m_isSelected;						// 选中状态
};

#endif // _SNSUIKIT_SNSTABLEVIEWCELLITEM_H_
