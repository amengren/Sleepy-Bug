//
//  SNSTableView.cpp
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#include "SNSTableView.h"


SNSTableView::SNSTableView()
:m_datasource(NULL), m_tableType(TableViewTypeINVALID), m_lazyLoad(false), m_rowCount(0), 
m_itemCount(0), m_columnCount(0), m_rowMaxInstance(0), m_topPointer(0), m_bottomPointer(0),
m_minRowWidthOrHeight(0), m_padding(0), m_margin(0), m_lastDragIndex(NULL), m_orientation(ReuseCellOrientationINVALID)
{
    
}

SNSTableView::~SNSTableView()
{
    
}

bool SNSTableView::initTableViewFrame(CCRect frame, TableViewType types)
{
    if ( !SNSScrollView::initScrollViewFrame(frame) ) {
        return false;
    }
    
    m_padding = 1;
    m_margin = 1; //wgx change 1 to 0.
    
    setTableType(types);
    if (!m_tableType) {
        setTableType(TableViewTypeVertical);
    }
    
    setIsTouchEnabled(true);
    setLazyLoad(false);
    
    return true;
}

SNSTableView* SNSTableView::initWithFrame(CCRect frame, TableViewType types)
{
    return SNSTableView::create(frame, types);
}

SNSTableView* SNSTableView::create(CCRect frame, TableViewType types)
{
	SNSTableView* pRet = new SNSTableView();
    
    if ( pRet && pRet->initTableViewFrame(frame, types) ) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return pRet;
}

TableViewType SNSTableView::getTableType()
{
    return m_tableType;
}

void SNSTableView::setTableType(TableViewType var)
{
    m_tableType = var;
    switch ( m_tableType ) {
        case TableViewTypeHorizontal:
            setHorizontal(true);
            setVertical(false);
            break;
        case TableViewTypeVertical:
            setHorizontal(false);
            setVertical(true);
            break;
        default:
            break;
    }
}

void SNSTableView::onEnterTransitionDidFinish()
{
    SNSScrollView::onEnterTransitionDidFinish();
    
    if ( !m_body->getChildren() || m_body->getChildren()->count() <= 0 ) {
        loadData();
    }
}

void SNSTableView::reloadData()
{
	// 停止滚动，防止因为修改指针而崩溃
	stopScroll();
	// 清理body上的所有child
    m_body->removeAllChildrenWithCleanup(true);
	m_itemCount = 0;
	m_rowCount = 0;
	m_columnCount = 0;
    
	//清空移动指针
	m_topPointer = 0;
	m_bottomPointer = 0;
    
	//重设body的position
    setBodySize(CCSizeMake(0, 0));
    loadData();
}

void SNSTableView::loadData()
{
    float nowLength = 0;
    
	//NSLog(@"m_queue:%@", [m_cellQueue description]);
    //获取当前所有tableView的item数量
	if ( m_datasource ) {
        m_itemCount = m_datasource->tableViewAllItemsNumber(this, 1);
	} else {
		m_itemCount = 0;
	}
    
	//获取当前tableView的列数
	if ( m_datasource ) {
        m_columnCount = m_datasource->tableViewColumnsOfItemsNumber(this, 1);
	} else {
		m_columnCount = 1;
	}
    
	//然后计算当前的行数
	m_rowCount = (int)((float)m_itemCount/(float)m_columnCount + 0.9999f);
    
    //计算总高度
	setScrollWidthOrHeight();
    //设置底部指针位置
	if (m_rowCount <= m_rowMaxInstance) m_bottomPointer = m_rowCount;
    //设置顶部指针
	for (int i = 0; i < m_rowCount; i++) {
		if (i == 0) {
			m_topPointer = 0;
		}
        SNSIndexPath* indexPath = SNSIndexPath::create(i, 1);
		SNSTableViewCell *cell = getCellByIndexPath(indexPath);
        m_body->addChild(cell, -i, i); // 设置zOrder以做到上压下
        moveCellToIndexPath(cell, indexPath);
		//如果当前宽高超过屏幕高度的话,设置底部指针
		if (m_bottomPointer == 0) {
			if (m_tableType == TableViewTypeVertical) {
				if (nowLength > m_frame.size.height + getRowWidthOrHeight(indexPath)) {
					m_bottomPointer = i;
					break;
				}
			} else if (m_tableType == TableViewTypeHorizontal) {
				if (nowLength > m_frame.size.width + getRowWidthOrHeight(indexPath)) {
					m_bottomPointer = i;
					break;
				}
			}
			nowLength += getRowWidthOrHeight(indexPath);
		}
	}
    
	//最后停止自己未结束的滑动
	stopScroll();
	fixPosition();
}

//根据当前行数算出body应该多宽或多高
void SNSTableView::setScrollWidthOrHeight()
{
	//归零最小格子高或宽
	m_minRowWidthOrHeight = 0;
	float length = 0;
	float widthOrHeight = 0;
    //获得最小格子的高度或宽度
	for (int i = 0; i < m_rowCount; i++) {
        
        SNSIndexPath* indexPath = SNSIndexPath::create(i, 1);
        widthOrHeight = getRowWidthOrHeight(indexPath);
		length += widthOrHeight;
        
		//初始化最短格子宽度
		if (i == 0) {
			m_minRowWidthOrHeight = widthOrHeight;
		}
		//纪录最短格子的宽或高
		if (widthOrHeight < m_minRowWidthOrHeight) {
			m_minRowWidthOrHeight = widthOrHeight;
		}
        
	}
    
	if (m_tableType == TableViewTypeVertical) {
		
		//重新设置body的高度，并且加上margin的高度
		setBodySize(CCSizeMake(m_bodySize.width, length + ((m_rowCount + m_padding) * m_margin)));
		//计算应该最多实例化多少个格子对象
        m_rowMaxInstance = (int)(m_frame.size.height / m_minRowWidthOrHeight + 0.9999f);
        
	} else if (m_tableType == TableViewTypeHorizontal) {
        
		//重新设置body的宽度，并且加上margin的宽度
        setBodySize(CCSizeMake(length + ((m_rowCount + m_padding) * m_margin), m_bodySize.height));
		//计算应该最多实例化多少个格子对象
		m_rowMaxInstance = (int)(m_frame.size.width / m_minRowWidthOrHeight + 0.9999f);
        
	}
}

SNSTableViewCell* SNSTableView::getCellByIndexPath(SNSIndexPath* indexPath)
{
	SNSTableViewCell *cell = NULL;
	if (NULL != m_datasource) {
		cell = m_datasource->tableViewCellForRow(this, indexPath);
	}
	if (NULL == cell) {
		cell = dequeueReusableCellWithIdentifier("SNSTableViewCell");
		if (NULL == cell) {
			cell = SNSTableViewCell::create("SNSTableViewCell");
		}
	}
	//CCLOG("cell:%s", cell->description());
	return cell;
}

SNSTableViewCellItem* SNSTableView::getItemByIndexPath(SNSIndexPath *indexPath)
{
	int row = indexPath->getRow();
	int column = indexPath->getColumn();
	SNSTableViewCellItem *item = NULL;
	SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(row);
	item = (SNSTableViewCellItem *)cell->getChildByTag(column);
	return item;
}

bool SNSTableView::indexPathInArea(SNSIndexPath *indexPath)
{
	int row = indexPath->getRow();
	if (row >= m_topPointer && row <= m_bottomPointer) {
		return true;
	}
	return false;
}

//复用cell
SNSTableViewCell* SNSTableView::dequeueReusableCellWithIdentifier(const char* cellIdentifier)
{
	SNSTableViewCell *cell = NULL;
	SNSIndexPath *indexPath = NULL;
	switch (m_orientation) {
		case ReuseCellOrientationBefore:
		{
			// 纵向向上，横向向左复用(最右移最左)
			if (m_bottomPointer < m_rowCount + 1 && m_bottomPointer > 0) {
				indexPath = SNSIndexPath::create(m_topPointer - 1, 1);
				cell = (SNSTableViewCell *)m_body->getChildByTag(m_bottomPointer - 1);
			}
		}
			break;
		case ReuseCellOrientationAfter:
		{
			// 纵向向下，横向向右复用(最左移最右)
			if (m_topPointer > 0) {
				indexPath = SNSIndexPath::create(m_bottomPointer - 1, 1);;
				cell = (SNSTableViewCell *)m_body->getChildByTag(m_topPointer - 1);
			}
		}
			break;
		default:
			break;
	}
	if (cell && indexPath) {
		int tag = indexPath->getRow();
		CCAssert(tag >= 0, "tag must be more than equal to 0");
		cell->setTag(tag);
		// 重新设置cell的zindex以做到上压下
		cell->getParent()->reorderChild(cell, -tag);
		//CCLOG("cell.tag:%d", cell->getTag);
		// 先改tag再移动，否则会出现找不到这个cell的问题
		moveCellToIndexPath(cell, indexPath);
	}
	return cell;
}

//根据头尾指针判断应该往哪个方向复用
void SNSTableView::reuseCellForOrientation(ReuseCellOrientation orientation)
{
	m_orientation = orientation;
	SNSIndexPath *indexPath = NULL;
	switch (m_orientation) {
		case ReuseCellOrientationBefore: // 纵向向上，横向向左复用(最右移最左)
		{
			if (m_topPointer > 0) {
				// 这里要传位移之后的cell的indexPath
				indexPath = SNSIndexPath::create(m_topPointer - 1, 1);
				// 调用重新设置cell的方法来更新cell
				getCellByIndexPath(indexPath);
			}
		}
			break;
		case ReuseCellOrientationAfter: // 纵向向下，横向向右复用(最左移最右)
		{
			// 这里要传位移之后的cell的indexPath
			indexPath = SNSIndexPath::create(m_bottomPointer - 1, 1);
			// 调用重新设置cell的方法来更新cell
			getCellByIndexPath(indexPath);
		}
			break;
		default:
			break;
	}
}

void SNSTableView::moveCellToIndexPath(SNSTableViewCell *cell, SNSIndexPath *indexPath)
{
	float scrollWidth = 0;
    float scrollHeight = 0;
	unsigned int i = indexPath->getRow();
	//	cell.opacity = 0.3f;
	float widthOrHeight = getRowWidthOrHeight(indexPath);
	if (m_tableType == TableViewTypeVertical) {
		scrollWidth = m_frame.size.width;
		scrollHeight = m_bodySize.height;
		cell->setContentSize(CCSizeMake(scrollWidth - (m_padding << 1), widthOrHeight));
		cell->setPosition(ccp(m_padding, scrollHeight - (i + 1) * (widthOrHeight + m_margin)));
	} else if (m_tableType == TableViewTypeHorizontal) {
		//scrollWidth = self.bodySize.width;
		scrollHeight = m_frame.size.height;
		cell->setContentSize(CCSizeMake(widthOrHeight, scrollHeight - (m_padding << 1)));
		cell->setPosition(ccp((i + 1) * (widthOrHeight + m_margin) - (widthOrHeight + m_margin), m_padding));
	}
	//	CCFadeIn *fadeTo = [CCFadeIn actionWithDuration:0.1f];
	//	[cell runAction:fadeTo];
	//CCLOG(@"move %d to:%d", cell.tag, i);
	// 如果没有实例化过tableViewCellItem那么实例化它
	SNSTableViewCellItem *item = NULL;
	SNSIndexPath *newIndexPath = NULL;
	for (int j = 0; j < m_columnCount; j++) {
		if (NULL == m_datasource) break;
		newIndexPath = SNSIndexPath::create(indexPath->getRow(), j, indexPath->getSection());
		
		item = m_datasource->tableViewItemForCell(this, newIndexPath);
		CCAssert(item, "If you overwrited delegate method:\"ItemForCellAtIndexPath\", so this function should not return a NULL value!");
		int nowCount = getItemPointerWithIndexPath(newIndexPath);
		if (NULL == cell->getChildByTag(j)) {
			if (nowCount >= m_itemCount) break;
			cell->addChild(item, 0, j);
			item->setPosition(getItemPositionWithIndexPath(newIndexPath, widthOrHeight));
		} else if (NULL != cell->getChildByTag(j) && nowCount >= m_itemCount) {
			item->removeFromParentAndCleanup(true);
		}
	}
	
	//	//如果已经过了底部指针的渲染范围，那么将他的visible设置为no
	//	if (self.lazyLoad && m_bottomPointer > 0 && i > (m_bottomPointer + 2)) {
	//		cell->setVisible(false);
	//	}
}

//复用item
SNSTableViewCellItem* SNSTableView::dequeueReusableCellItemForIndexPath(SNSIndexPath *indexPath)
{
	int row = indexPath->getRow();
	SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(row);
	SNSTableViewCellItem *item = (SNSTableViewCellItem *)cell->getChildByTag(indexPath->getColumn());
	if (item) {
		// CCLOG("item::::::::::::%d, row:%d", indexPath->getColumn(), row);
		// 判断item是否是cellItem子类
//		if (typeid(item) == typeid(SNSTableViewCellItem)) {
//			//只复用格子，内容物重新获取
//			item->removeAllChildrenWithCleanup(true);
//		}
		/* 这里不需要移除所有内容，需要移除的话去delegate函数中移除 */
		return item;
	}
	return NULL;
}

float SNSTableView::getRowWidthOrHeight(SNSIndexPath* indexPath)
{
	float rowWidthOrHeight = 64.0f;
    if ( m_delegate ) {
        rowWidthOrHeight = ((SNSTableViewDelegate *)m_delegate)->tableViewHeightForRow(this, indexPath);
    }
    return rowWidthOrHeight;
}

float SNSTableView::getCellPositionWithIndexPath(SNSIndexPath *indexPath)
{
	unsigned int i = indexPath->getRow();
	float returnValue = 0;
	for (int j = 0; j < i; j++) {
		returnValue += (getRowWidthOrHeight(SNSIndexPath::create(j, 0, 1)) + m_margin);
	}
	return returnValue;
}

CCPoint SNSTableView::getItemPositionWithIndexPath(SNSIndexPath* indexPath, float widthOrHeight)
{
	//计算每个item的size
	int xyValue = 0;
	int column = indexPath->getColumn();
    
	CCPoint point = ccp(0, 0);
	if (m_tableType == TableViewTypeVertical) {
        
		xyValue = (int)(m_bodySize.width / m_columnCount + 0.5f);
		xyValue = xyValue * column + xyValue * 0.5f;
		point = ccp(xyValue, widthOrHeight * 0.5f);
        
	} else if (m_tableType == TableViewTypeHorizontal) {
        
		xyValue = (int)(m_bodySize.height / m_columnCount + 0.5f);
		xyValue = xyValue * column + xyValue * 0.5f;
		point = ccp(widthOrHeight * 0.5f, xyValue);
        
	}
    
	return point;
}

unsigned int SNSTableView::getItemPointerWithIndexPath(SNSIndexPath* indexPath)
{
	unsigned int pointer = 0;
	if ( 0 == indexPath->getRow() ) {
		pointer = indexPath->getColumn();
	} else {
		pointer = (indexPath->getRow() * m_columnCount) + indexPath->getColumn();
	}
	return pointer;
}

void SNSTableView::moveToIndexPath(SNSIndexPath* indexPath)
{
	SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(indexPath->getRow());
	CCPoint pos;
	if (cell) {
		if (m_tableType == TableViewTypeVertical) {
            pos = ccp(m_body->getPosition().x, -cell->getPosition().y + cell->getContentSize().height * 0.5f + m_margin);
		} else if (m_tableType == TableViewTypeHorizontal) {
            pos = ccp(-cell->getPosition().x, m_body->getPosition().y);
		}
	} else {
        pos = ccp(0, -m_bodySize.height + m_frame.size.height);
	}
	moveBodyToPosition(pos);
}

#pragma mark - overwrite parent method

void SNSTableView::scrollViewDidScroll() {
    
    SNSScrollView::scrollViewDidScroll();
    
    //如果有数据且底部指针不为0
    if (m_rowCount > 0 && m_bottomPointer > 0) {
        
        //更新位置开始
		SNSTableViewCell *topCell = (SNSTableViewCell*)m_body->getChildByTag(m_topPointer);
		float scrollPos = 0;
		float topCellPos = 0;
		float topCellWidthOrHeight = 0;
        
		if (m_tableType == TableViewTypeVertical) {
			//如果是纵向滚动的话
			scrollPos = m_body->getPosition().y + m_bodySize.height - m_frame.size.height;
			topCellPos = m_bodySize.height - topCell->getPosition().y;
			topCellWidthOrHeight = topCell->getContentSize().height;
		} else if (m_tableType == TableViewTypeHorizontal) {
			//如果是横向滚动的话
			scrollPos = -m_body->getPosition().x;
			topCellPos = topCell->getPosition().x;
			topCellWidthOrHeight = topCell->getContentSize().width;
		}
        
		// 如果是body向上或向左滑动的话
		if (scrollPos > topCellPos) {
			if (m_bottomPointer < m_rowCount) {
				++m_bottomPointer;
//				if (m_lazyLoad) {
//					((SNSTableViewCell *)m_body)->getChildByTag(m_topPointer - 1)->setVisible(false);
//					((SNSTableViewCell *)m_body)->getChildByTag(m_bottomPointer + 1)->setVisible(true);
//				}
				reuseCellForOrientation(ReuseCellOrientationAfter);
				++m_topPointer;
				//CCLOG(@"top:%d bottom:%d isAfter", m_topPointer, m_bottomPointer);
			}
		}
		// 如果是body向下或向右滑动的话
		if (scrollPos < (topCellPos - topCellWidthOrHeight)) {
			if (m_topPointer > 0) {
				--m_topPointer;
//				if (m_lazyLoad) {
//					((SNSTableViewCell *)m_body)->getChildByTag(m_topPointer - 1)->setVisible(true);
//					((SNSTableViewCell *)m_body)->getChildByTag(m_bottomPointer + 1)->setVisible(false);
//				}
				reuseCellForOrientation(ReuseCellOrientationBefore);
				--m_bottomPointer;
				//CCLOG(@"top:%d bottom:%d isBefore", m_topPointer, m_bottomPointer);
			}
		}
    }
}

void SNSTableView::scrollViewDidClick(CCPoint position)
{
	//NSLog(@"点击：%f ** %f", position.x, position.y);
	CCRect frame;
    
	//如果delegate实现了点击函数那么进行处理，否则什么都不干！
	if (m_rowCount > 0 && m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewSelectRow ) {
        
		//遍历指针之间的区域，获得点击位置
		for (int i = m_topPointer; i < m_bottomPointer; i++) {
            
			SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(i);
            
			frame = CCRectMake(cell->getPosition().x, cell->getPosition().y, cell->getContentSize().width, cell->getContentSize().height);
			//NSLog(@"frame:%f ++ %f ++ %f ++ %f", frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
            
			if( /*frame.containsPoint(position)*/CCRect::CCRectContainsPoint(frame, position)) {
                
				//NSLog(@"点击了第%d个cell", i);
				SNSIndexPath *indexPath = SNSIndexPath::create(i, 0);
				
				CCRect itemFrame = CCRectMake(0, 0, 0, 0);
				float itemWidthOrHeight = 0;
				CCSize itemSize = CCSizeMake(0, 0);
                
				for (int j = 0; j < m_columnCount; j++) {
                    
					if (m_tableType == TableViewTypeVertical) {
						itemWidthOrHeight = (int)(m_frame.size.width / m_columnCount + 0.5f);
						itemFrame = CCRectMake(cell->getPosition().x + (itemWidthOrHeight * j), cell->getPosition().y, itemWidthOrHeight, cell->getContentSize().height);
						itemSize = CCSizeMake(0, cell->getContentSize().height);
					} else if (m_tableType == TableViewTypeHorizontal) {
						itemWidthOrHeight = (int)(m_frame.size.height / m_columnCount + 0.5f);
						itemFrame = CCRectMake(cell->getPosition().x, cell->getPosition().y + (itemWidthOrHeight * j), cell->getContentSize().width, itemWidthOrHeight);
						itemSize = CCSizeMake(cell->getContentSize().width, 0);
					}
                    
					if ( /*itemFrame.containsPoint(position) */ CCRect::CCRectContainsPoint(itemFrame, position)) {
                        
						SNSIndexPath *itemIndexPath = SNSIndexPath::create(i, j, 0);
                        
						int now = getItemPointerWithIndexPath(itemIndexPath);
						if (now < m_itemCount) {
							//CCLog("点击了第%d个item", now);
							if ( m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewSelectItem ) {
                                ((SNSTableViewDelegate *)m_delegate)->tableViewSelectItem(this, itemIndexPath);
							}
                            
							if ( m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewSelectItemPoint ) {
								SNSTableViewCellItem *item = (SNSTableViewCellItem*)cell->getChildByTag(j);
								//cell
								CCPoint point = ccpSub(position, ccp(itemFrame.origin.x, itemFrame.origin.y));
								//item
								point = ccpSub(point, ccp(item->getPosition().x - (itemSize.width * j), item->getPosition().y - (itemSize.height * j)));
                                
                                ((SNSTableViewDelegate *)m_delegate)->tableViewSelectItem(this, itemIndexPath, point);
							}
						}
						break;
					}
				}
                
				if ( m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewSelectRow ) {
                    ((SNSTableViewDelegate *)m_delegate)->tableViewSelectRow(this, indexPath);
					break;
				}
			}
		}
	}
}

void SNSTableView::scrollViewCellDeleteAction(SNSScrollView* scrollView, CCPoint position)
{
    CCRect frame;
    
	//如果delegate实现了点击函数那么进行处理，否则什么都不干！
	if (m_rowCount > 0 && m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewDeleteActionItem ) {
        
		//遍历指针之间的区域，获得点击位置
		for (int i = m_topPointer; i < m_bottomPointer; i++) {
            
			SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(i);
            
			frame = CCRectMake(cell->getPosition().x, cell->getPosition().y, cell->getContentSize().width, cell->getContentSize().height);
            
			if( /*frame.containsPoint(position)*/CCRect::CCRectContainsPoint(frame, position)) {
                
				//NSLog(@"点击了第%d个cell", i);
				SNSIndexPath *indexPath = SNSIndexPath::create(i, 0);
				
				CCRect itemFrame = CCRectMake(0, 0, 0, 0);
				float itemWidthOrHeight = 0;
				CCSize itemSize = CCSizeMake(0, 0);
                
				for (int j = 0; j < m_columnCount; j++) {
                    
					if (m_tableType == TableViewTypeVertical) {
						itemWidthOrHeight = (int)(m_frame.size.width / m_columnCount + 0.5f);
						itemFrame = CCRectMake(cell->getPosition().x + (itemWidthOrHeight * j), cell->getPosition().y, itemWidthOrHeight, cell->getContentSize().height);
						itemSize = CCSizeMake(0, cell->getContentSize().height);
					} else if (m_tableType == TableViewTypeHorizontal) {
						itemWidthOrHeight = (int)(m_frame.size.height / m_columnCount + 0.5f);
						itemFrame = CCRectMake(cell->getPosition().x, cell->getPosition().y + (itemWidthOrHeight * j), cell->getContentSize().width, itemWidthOrHeight);
						itemSize = CCSizeMake(cell->getContentSize().width, 0);
					}
                    
					if ( /*itemFrame.containsPoint(position)*/CCRect::CCRectContainsPoint(itemFrame, position)) {
                        
						SNSIndexPath *itemIndexPath = SNSIndexPath::create(i, j, 0);
                        
						int now = getItemPointerWithIndexPath(itemIndexPath);
						if (now < m_itemCount) {
							//CCLOG("横扫了第%d个item", now);
                            if ( m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewDeleteActionItem ) {
                                SNSTableViewCellItem *item = (SNSTableViewCellItem*)cell->getChildByTag(j);
								//cell
								CCPoint point = ccpSub(position, ccp(itemFrame.origin.x, itemFrame.origin.y));
								//item
								point = ccpSub(point, ccp(item->getPosition().x - (itemSize.width * j), item->getPosition().y - (itemSize.height * j)));
                                
                                ((SNSTableViewDelegate *)m_delegate)->tableViewDeleteActionItem(this, indexPath, point);
                            }
						}
						break;
					}
				}
			}
		}
	}
}

//scrollView内容物拖动开始的事件
void SNSTableView::scrollViewDidStartDrag(CCPoint position)
{
    //CCLOG("拖动开始");
    CCRect frame;
    
	//如果delegate实现了点击函数那么进行处理，否则什么都不干！
	if (m_rowCount > 0 && m_delegate) {
        
		//遍历指针之间的区域，获得点击位置
		for (int i = m_topPointer; i < m_bottomPointer; i++) {
            
			SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(i);
            
			frame = CCRectMake(cell->getPosition().x, cell->getPosition().y, cell->getContentSize().width, cell->getContentSize().height);
            
			if( /*frame.containsPoint(position)*/CCRect::CCRectContainsPoint(frame, position) ) {
                
				//NSLog(@"点击了第%d个cell", i);
				SNSIndexPath *indexPath = SNSIndexPath::create(i, 0);
				
				CCRect itemFrame = CCRectMake(0, 0, 0, 0);
				float itemWidthOrHeight = 0;
				CCSize itemSize = CCSizeMake(0, 0);
                
				for (int j = 0; j < m_columnCount; j++) {
                    
					if (m_tableType == TableViewTypeVertical) {
						itemWidthOrHeight = (int)(m_frame.size.width / m_columnCount + 0.5f);
						itemFrame = CCRectMake(cell->getPosition().x + (itemWidthOrHeight * j), cell->getPosition().y, itemWidthOrHeight, cell->getContentSize().height);
						itemSize = CCSizeMake(0, cell->getContentSize().height);
					} else if (m_tableType == TableViewTypeHorizontal) {
						itemWidthOrHeight = (int)(m_frame.size.height / m_columnCount + 0.5f);
						itemFrame = CCRectMake(cell->getPosition().x, cell->getPosition().y + (itemWidthOrHeight * j), cell->getContentSize().width, itemWidthOrHeight);
						itemSize = CCSizeMake(cell->getContentSize().width, 0);
					}
                    
					if (/* itemFrame.containsPoint(position)*/CCRect::CCRectContainsPoint(itemFrame, position)) {
                        
						SNSIndexPath *itemIndexPath = SNSIndexPath::create(i, j, 0);
                        
						int now = getItemPointerWithIndexPath(itemIndexPath);
						if (now < m_itemCount) {
							//CCLOG("拖动第%d个item", now);
                            if ( m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewDragStartItem) {
								m_isCanScroll = false;
                                SNSTableViewCellItem *item = (SNSTableViewCellItem*)cell->getChildByTag(j);
								//cell
								CCPoint point = ccpSub(position, ccp(itemFrame.origin.x, itemFrame.origin.y));
								//item
								point = ccpSub(point, ccp(item->getPosition().x - (itemSize.width * j), item->getPosition().y - (itemSize.height * j)));
                                
                                ((SNSTableViewDelegate *)m_delegate)->tableViewDidDragStartItem(this, indexPath, point);
                                m_lastDragIndex = indexPath;
                            }
						}
						break;
					}
				}
			}
		}
	}
}

//scrollView内容物拖动结束的事件
void SNSTableView::scrollViewDidEndDrag(CCPoint position)
{
    //CCLOG("拖动结束");
    if ( m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewDragEndItem) {
        ((SNSTableViewDelegate *)m_delegate)->tableViewDidDragEndItem(this, m_lastDragIndex, position);
		m_isCanScroll = true;
    }
}
