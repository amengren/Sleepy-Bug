//
//  SNSTableView.h
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#ifndef _SNSUIKIT_SNSTABLEVIEW_H_
#define _SNSUIKIT_SNSTABLEVIEW_H_

#include "cocos2d.h"

USING_NS_CC;

#include "SNSScrollView.h"
#include "SNSTableViewCell.h"
#include "SNSTableViewCellItem.h"
#include "SNSIndexPath.h"

class SNSTableView;

typedef enum {
    TableViewTypeINVALID = 0,
    TableViewTypeHorizontal,		// TableView滚动方向：横向
    TableViewTypeVertical,			// TableView滚动方向：纵向
} TableViewType;

typedef enum {
	ReuseCellOrientationINVALID = 0,
	ReuseCellOrientationBefore,		// 复用方式：向前
	ReuseCellOrientationAfter,		// 复用方式：向后
} ReuseCellOrientation;

typedef enum {
	TableViewSelectTypeINVALID = 0,
	TableViewSelectTypeSingle,		// TableView选中item的类型：单选
	TableViewSelectTypeMulti,		// TableView选中item的类型：多选
} TableViewSelectType;

class SNSTableViewDelegate : public SNSScrollViewDelegate
{
public:
    SNSTableViewDelegate(){
        /*
         如果实现特定函数，请置为true;
         */
//        isAchieve_tableViewSelectRow = false;
        isAchieve_tableViewSelectRow = true;
        //isAchieve_tableViewDeselectRow = false;
        isAchieve_tableViewSelectItem = true;
        isAchieve_tableViewSelectItemPoint = true;
        isAchieve_tableViewDeselectItem = true;
        isAchieve_tableViewDeleteActionItem = true;
		isAchieve_tableViewDragStartItem = false;
		isAchieve_tableViewDragEndItem = false;
    }
    
public:
    //定义tableView每一行或列的高度
    virtual float tableViewHeightForRow(SNSTableView* tableView, SNSIndexPath* indexPath) = 0;
    
    //每一行cell的点击事件
    bool isAchieve_tableViewSelectRow;
    virtual void tableViewSelectRow(SNSTableView* tableView, SNSIndexPath* indexPath){};
    //每一行cell的取消点击事件
    //bool isAchieve_tableViewDeselectRow;
    //virtual void tableViewDeselectRow(SNSTableView* tableView, SNSIndexPath* indexPath){};

    //每一个item的点击事件
    bool isAchieve_tableViewSelectItem;
    virtual void tableViewSelectItem(SNSTableView* tableView, SNSIndexPath* indexPath){};
    //当点击item的时候发送相对于item的点击位置
    bool isAchieve_tableViewSelectItemPoint;
    virtual void tableViewSelectItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position){};
    //每一个item的取消点击事件
    bool isAchieve_tableViewDeselectItem;
    virtual void tableViewDeselectItem(SNSTableView* tableView, SNSIndexPath* indexPath){};
    
    //反馈当前点中的删除对象
    bool isAchieve_tableViewDeleteActionItem;
    virtual void tableViewDeleteActionItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position){};
    
    //反馈内容物（item）拖动开始事件
	bool isAchieve_tableViewDragStartItem;
    virtual void tableViewDidDragStartItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position){};
    
    //反馈内容物（item）拖动结束事件
	bool isAchieve_tableViewDragEndItem;
    virtual void tableViewDidDragEndItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position){};
	
	//当启用page功能的时候，返回当前是第几页
	virtual void tableViewAtPage(SNSTableView* tableView, int page, int pageCount){};
};

//DataSource Delegate
class SNSTableViewDataSource
{
public:
    //返回tableView总的数据数量
    virtual unsigned int tableViewAllItemsNumber(SNSTableView* tableView, int section) = 0;
    //根据indexPath返回tableView的cellItem
    virtual SNSTableViewCellItem* tableViewItemForCell(SNSTableView* tableView, SNSIndexPath* indexPath) = 0;
    
    //根据indexPath返回tableView的cell
    virtual SNSTableViewCell* tableViewCellForRow(SNSTableView* tableView, SNSIndexPath* indexPath) { return NULL; }
    //返回tableView的列数
    virtual unsigned int tableViewColumnsOfItemsNumber(SNSTableView* tableView, int section) { return 1; }
};

class SNSTableView : public SNSScrollView, public SNSScrollViewDelegate {
	
public:
    //CC_SYNTHESIZE(SNSTableViewDelegate*, m_delegate, Delegate);
	CC_SYNTHESIZE(SNSTableViewDataSource*, m_datasource, Datasource);
    CC_PROPERTY(TableViewType, m_tableType, TableType);
	CC_PROPERTY(TableViewSelectType, m_selectType, SelectType);
    CC_SYNTHESIZE(bool, m_lazyLoad, LazyLoad);
    
    CC_SYNTHESIZE_READONLY(unsigned int, m_rowCount, RowCount);
    
protected:
	unsigned int					m_itemCount;
	
	unsigned int					m_columnCount;
	unsigned int					m_rowMaxInstance;
	unsigned int					m_topPointer;
    unsigned int					m_bottomPointer;
	
	float							m_minRowWidthOrHeight;
	int								m_padding;
	int								m_margin;
    SNSIndexPath *                  m_lastDragIndex;
	ReuseCellOrientation			m_orientation;
	
	CCSize							m_itemSize;
	
	bool *							m_selectedItem;
    
protected:
    SNSTableView();
    virtual ~SNSTableView();
    virtual bool initTableViewFrame(CCRect frame, TableViewType types);
    
protected:
    virtual void onEnterTransitionDidFinish();
	
    virtual void scrollViewDidClick(CCPoint position);
    //轻扫手势
    virtual void scrollViewCellDeleteAction(SNSScrollView* scrollView, CCPoint ratio);
    //scrollView内容物拖动开始的事件
    virtual void scrollViewDidStartDrag(CCPoint position);
    //scrollView内容物拖动结束的事件
    virtual void scrollViewDidEndDrag(CCPoint position);
	
public:
	virtual void scrollViewDidScroll();
	virtual void returnNowPage(int nowPage, int pageCount);
    
public:
    CC_DEPRECATED_ATTRIBUTE static SNSTableView* initWithFrame(CCRect frame, TableViewType types);
	static SNSTableView* create(CCRect frame, TableViewType types);
    
public:
    //重载数据,fullReload为true的时候完全重新载入数据，否则只是为了当前修改某些cell的数据而局部刷新
    void reloadData(bool fullReload = true);
    //加载数据
    void loadData();
    
    //获得每一行的宽度或高度
    float getRowWidthOrHeight(SNSIndexPath* indexPath);
    //根据数据集设置当前body的高或者宽
    void setScrollWidthOrHeight();
	//根据indexPath获得当前cell的位置
	float getCellPositionWithIndexPath(SNSIndexPath *indexPath);
    //根据indexPath获得当前cell的item的size
    CCPoint getItemPositionWithIndexPath(SNSIndexPath* indexPath, float widthOrHeight);
    //根据indexPath获得当前的物品指针(必须写在这里，因为indexPath根本不知道也不需要知道当前有几列，等同于第几个item)
    unsigned int getItemPointerWithIndexPath(SNSIndexPath* indexPath);
	//获取每个item的大小，ccsize类型
	CCSize getItemSize();
    //根据indexPath获得cell
    SNSTableViewCell* getCellByIndexPath(SNSIndexPath* indexPath);
	//根据indexPath获得item
	SNSTableViewCellItem* getItemByIndexPath(SNSIndexPath *indexPath);
	//将cell移动到指定indexPath的位置
	void moveCellToIndexPath(SNSTableViewCell *cell, SNSIndexPath *indexPath);
	//判断传过来的indexPath的cell是否在顶部和底部指针之间
	bool indexPathInArea(SNSIndexPath *indexPath);
	//选中指定的tableView的item,然后调用cellItem中的setSelected事件
	bool selectItemByIndexPath(SNSIndexPath *indexPath);
	
	//根据当前指针来确定是往前复用cell还是往后复用cell
	void reuseCellForOrientation(ReuseCellOrientation orientation);
    //复用cell函数
    SNSTableViewCell* dequeueReusableCellWithIdentifier(const char* cellIdentifier);
	//复用item函数
	SNSTableViewCellItem* dequeueReusableCellItemForIndexPath(SNSIndexPath *indexPath);
    //根据传过来的indexPath移动cell到相应的位置
    void moveToIndexPath(SNSIndexPath* indexPath);
    
};

#endif // _SNSUIKIT_SNSTABLEVIEW_H_
