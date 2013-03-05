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
m_minRowWidthOrHeight(0), m_padding(0), m_margin(0), m_lastDragIndex(NULL), m_orientation(ReuseCellOrientationINVALID),
m_selectType(TableViewSelectTypeINVALID), m_selectedItem(NULL), m_itemSize(CCSizeZero)
//dhd
,m_isAllMaxInstance(false),m_moveAnimationDuration(1.0f)
{
	
}

SNSTableView::~SNSTableView()
{
	if (NULL != m_selectedItem) {
		CC_SAFE_DELETE(m_selectedItem);
	}
}

bool SNSTableView::initTableViewFrame(CCRect frame, TableViewType types)
{
    if ( !SNSScrollView::initScrollViewFrame(frame) ) {
        return false;
    }
    
    m_padding = 1;
    m_margin = 0; //wgx change 1 to 0.
    
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
	SNSTableView* instance = new SNSTableView();
    
    if (instance && instance->initTableViewFrame(frame, types) ) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return instance;
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

TableViewSelectType SNSTableView::getSelectType()
{
	return m_selectType;
}

void SNSTableView::setSelectType(TableViewSelectType var)
{
	m_selectType = var;
	// 根据cell的数量初始化选中数组
	if (NULL != m_selectedItem) {
		CC_SAFE_DELETE(m_selectedItem);
	}
	m_selectedItem = new bool[m_itemCount];
	// 默认全部置false
	for (int i = 0; i < m_itemCount; i++) {
		m_selectedItem[i] = false;
	}
}

void SNSTableView::onEnterTransitionDidFinish()
{
    SNSScrollView::onEnterTransitionDidFinish();
    
    if ( !m_body->getChildren() || m_body->getChildren()->count() <= 0 ) {
        loadData();
    }
}

void SNSTableView::reloadData(bool fullReload)
{
	// 停止滚动，防止因为修改指针而崩溃
	stopScroll();
	if (fullReload) {
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
	} else {
		int lastRowCount = m_rowCount;
		// 重算item数
		if ( m_datasource ) {
			m_itemCount = m_datasource->tableViewAllItemsNumber(this, 1);
		} else {
			m_itemCount = 0;
		}
		// 根据cell的数量初始化选中数组
		if (NULL != m_selectedItem) {
			CC_SAFE_DELETE(m_selectedItem);
		}
		m_selectedItem = new bool[m_itemCount];
		// 默认全部置false
		for (int i = 0; i < m_itemCount; i++) {
			m_selectedItem[i] = false;
		}
		
		//获取当前tableView的列数
		if ( m_datasource ) {
			m_columnCount = m_datasource->tableViewColumnsOfItemsNumber(this, 1);
		} else {
			m_columnCount = 1;
		}
		
		//然后计算当前的行数
		m_rowCount = (int)((float)m_itemCount / (float)m_columnCount + 0.9999f);
		
		// 如果两次的行数不一样，证明数据量发生了变化，那么重新刷新数据
		if (lastRowCount != m_rowCount) {
			return reloadData();
		}
		
		//遍历指针之间的区域，刷新item
		for (int i = m_topPointer; i < m_bottomPointer; i++) {
			SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(i);
			if (cell) {
				// 如果cell存在的话reset cell的item数据
				moveCellToIndexPath(cell, cell->getIndexPath());
			}
		}
		//最后停止自己未结束的滑动
		stopScroll();
		fixPosition();
	}
}

void SNSTableView::loadData()
{
    float nowLength = 0;
    //获取当前所有tableView的item数量
	if ( m_datasource ) {
        m_itemCount = m_datasource->tableViewAllItemsNumber(this, 1);
	} else {
		m_itemCount = 0;
	}
	
	// 根据cell的数量初始化选中数组
	if (NULL != m_selectedItem) {
		CC_SAFE_DELETE(m_selectedItem);
	}
	m_selectedItem = new bool[m_itemCount];
	// 默认全部置false
	for (int i = 0; i < m_itemCount; i++) {
		m_selectedItem[i] = false;
	}
    
	//获取当前tableView的列数
	if ( m_datasource ) {
        m_columnCount = m_datasource->tableViewColumnsOfItemsNumber(this, 1);
	} else {
		m_columnCount = 1;
	}
    
	//然后计算当前的行数
	m_rowCount = (int)((float)m_itemCount / (float)m_columnCount + 0.9999f);
    
    //计算总高度
	setScrollWidthOrHeight();
    //设置底部指针位置
	if (m_rowCount == m_rowMaxInstance + 1 || m_rowCount <= m_rowMaxInstance) m_bottomPointer = m_rowCount;
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
    
    //dhd 如果显示所有实例的话
    if ( m_isAllMaxInstance ) {
        m_rowMaxInstance = m_rowCount;
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

bool SNSTableView::selectItemByIndexPath(SNSIndexPath *indexPath)
{
	if (indexPathInArea(indexPath)) {
		SNSTableViewCellItem *item = getItemByIndexPath(indexPath);
		if (item) {
			switch (m_selectType) {
				case TableViewSelectTypeSingle:
				{
					// 默认取消选中body中的所有子item
					CCObject *obj = NULL;
					CCARRAY_FOREACH(m_body->getChildren(), obj) {
						SNSTableViewCell *cell = (SNSTableViewCell *)obj;
						if (cell) {
							for (int j = 0; j < m_columnCount; j++) {
								SNSTableViewCellItem *item = (SNSTableViewCellItem*)cell->getChildByTag(j);
								if (item) {
									item->setSelected(false);
								}
							}
						}
					}
					//单选直接选中
					item->setSelected(true);
				}
					break;
				case TableViewSelectTypeMulti:
				{
					// 多选的时候才可以取消
					item->setSelected(!item->getSelected());
				}
					break;
				default:
					break;
			}
			
			m_selectedItem[getItemPointerWithIndexPath(indexPath)] = item->getSelected();
			return true;
		}
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
		// 重设cell的indexPath
		cell->setIndexPath(indexPath);
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
	//CCLOG("move %d to:%d", cell->getTag(), i);
	// 如果没有实例化过tableViewCellItem那么实例化它(复用情况下只能在这里实例化item)
	SNSTableViewCellItem *item = NULL;
	SNSIndexPath *newIndexPath = NULL;
	// 根据横纵设置item的contentSize
	if (m_tableType == TableViewTypeVertical) {
		m_itemSize = CCSizeMake(cell->getContentSize().width / m_columnCount, widthOrHeight);
	} else if (m_tableType == TableViewTypeHorizontal) {
		m_itemSize = CCSizeMake(widthOrHeight, cell->getContentSize().height / m_columnCount);
	}
	// 没辙，必须在这里设置cell的Item
	int nowCount;
	for (int j = 0; j < m_columnCount; j++) {
		if (NULL == m_datasource) break;
		newIndexPath = SNSIndexPath::create(indexPath->getRow(), j, indexPath->getSection());
		nowCount = getItemPointerWithIndexPath(newIndexPath);
		// 无论如何都要去执行一下取item的方法，否则初始化会有问题
		item = m_datasource->tableViewItemForCell(this, newIndexPath);
		CCAssert(item, "If you overwrited delegate method:\"ItemForCellAtIndexPath\", so this function should not return a NULL value!");
		item->setContentSize(m_itemSize);
		item->setIndexPath(newIndexPath);
		
		if (nowCount >= m_itemCount) {
			// 如果当前读取的数据已经大于总数据量了，那么隐藏当前item
			item->setVisible(false);
		} else {
			// 否则显示他，并且做重置操作
			item->setVisible(true);
		}
		if (NULL == cell->getChildByTag(j)) {
			cell->addChild(item, 0, j);
			item->setPosition(getItemPositionWithIndexPath(newIndexPath, widthOrHeight));
			//CCLOG("item.width:%f, item.height:%f", item->getContentSize().width, item->getContentSize().height);
		}
//		else {
//			// 否则移除这个item
//			item->removeFromParentAndCleanup(true);
//		}
	}
	// 为了保证indexPath一直正确，所以最后再设置cell的indexPath（因为有可能这个indexPath是来自cell的indexPath，具体看代码）
	cell->setIndexPath(indexPath);
}

//复用item
SNSTableViewCellItem* SNSTableView::dequeueReusableCellItemForIndexPath(SNSIndexPath *indexPath)
{
	int row = indexPath->getRow();
	SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(row);
	SNSTableViewCellItem *item = (SNSTableViewCellItem *)cell->getChildByTag(indexPath->getColumn());
	if (item) {
//		CCLOG("item::::::::::::%d, row:%d", indexPath->getColumn(), row);
		// 判断item是否是cellItem子类
		if (typeid(*item) == typeid(SNSTableViewCellItem)) {
			//只复用格子，内容物重新获取
			item->removeAllChildrenWithCleanup(true);
		}
		//根据记录判断是否应该让他选中
		item->setSelected(m_selectedItem[getItemPointerWithIndexPath(indexPath)]);
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
	unsigned int row = indexPath->getRow();
	float returnValue = 0;
	for (int j = 0; j < row; j++) {
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
		// 为了让横屏的时候计算从上向下走
        column = m_columnCount - 1 - column;
		xyValue = (int)(m_bodySize.height / m_columnCount + 0.5f);
		xyValue = xyValue * column + xyValue * 0.5f;
		point = ccp(widthOrHeight * 0.5f, xyValue);
        
	}
    
	return point;
}

unsigned int SNSTableView::getItemPointerWithIndexPath(SNSIndexPath* indexPath)
{
	int pointer = -1;
	if (indexPath) {
		if ( 0 == indexPath->getRow() ) {
			pointer = indexPath->getColumn();
		} else {
			pointer = (indexPath->getRow() * m_columnCount) + indexPath->getColumn();
		}
	}
	return pointer;
}

CCSize SNSTableView::getItemSize()
{
	return m_itemSize;
}

void SNSTableView::moveToIndexPath(SNSIndexPath* indexPath, bool _animation)
{
	SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(indexPath->getRow());
	CCPoint pos = ccp(0, -m_bodySize.height + m_frame.size.height);
	if (cell) {
		if (m_tableType == TableViewTypeVertical) {
            pos = ccp(m_body->getPosition().x, -cell->getPosition().y + cell->getContentSize().height * 0.5f + m_margin);
			if (pos.y > 0) {
				pos = CCPointZero;
			}
		} else if (m_tableType == TableViewTypeHorizontal) {
            pos = ccp(-cell->getPosition().x, m_body->getPosition().y);
			if (pos.x > 0) {
				pos = CCPointZero;
			}
		}
	}
	
	moveBodyToPosition(pos);
    
    //dhd
    if ( _animation ) {
        m_body->runAction(CCMoveTo::create(m_moveAnimationDuration, pos));
    } else {
        moveBodyToPosition(pos);
    }
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
				reuseCellForOrientation(ReuseCellOrientationAfter);
				++m_topPointer;
				//CCLOG("top:%d bottom:%d isAfter", m_topPointer, m_bottomPointer);
			}
		}
		// 如果是body向下或向右滑动的话
		if (scrollPos < (topCellPos - topCellWidthOrHeight)) {
			if (m_topPointer > 0) {
				--m_topPointer;
				reuseCellForOrientation(ReuseCellOrientationBefore);
				--m_bottomPointer;
				//CCLOG(@"top:%d bottom:%d isBefore", m_topPointer, m_bottomPointer);
			}
		}
    }
}

void SNSTableView::returnNowPage(int nowPage, int pageCount)
{
	if (m_delegate) {
		((SNSTableViewDelegate *)m_delegate)->tableViewAtPage(this, nowPage, pageCount);
	}
}

void SNSTableView::scrollViewDidClick(CCPoint position)
{
	//NSLog("点击：%f ** %f", position.x, position.y);
	CCRect frame;
    
	//如果delegate实现了点击函数那么进行处理，否则什么都不干！
	if (m_rowCount > 0 && m_delegate && ((SNSTableViewDelegate *)m_delegate)->isAchieve_tableViewSelectRow ) {
        
		//遍历指针之间的区域，获得点击位置
		for (int i = m_topPointer; i < m_bottomPointer; i++) {
            
			SNSTableViewCell *cell = (SNSTableViewCell *)m_body->getChildByTag(i);
            
			frame = CCRectMake(cell->getPosition().x, cell->getPosition().y, cell->getContentSize().width, cell->getContentSize().height);
			//NSLog("frame:%f ++ %f ++ %f ++ %f", frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
            
			if( frame.containsPoint(position) ) {
                
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
                    
					if ( itemFrame.containsPoint(position) ) {
                        
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
            
			if( frame.containsPoint(position) ) {
                
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
                    
					if ( itemFrame.containsPoint(position) ) {
                        
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
            
			if( frame.containsPoint(position) ) {
                
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
                    
					if ( itemFrame.containsPoint(position) ) {
                        
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
