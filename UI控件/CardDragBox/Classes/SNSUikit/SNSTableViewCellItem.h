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

class SNSTableViewCellItem : public CCLayerColor //public CCNode 
{
protected:
    SNSTableViewCellItem():m_indexPath(NULL){}
    virtual ~SNSTableViewCellItem(){}
    virtual bool initCellItem();
	
public:
    CC_DEPRECATED_ATTRIBUTE static SNSTableViewCellItem* initItem();
	static SNSTableViewCellItem* create();
	
	CC_SYNTHESIZE_RETAIN(SNSIndexPath *, m_indexPath, IndexPath);
    
};

#endif // _SNSUIKIT_SNSTABLEVIEWCELLITEM_H_
