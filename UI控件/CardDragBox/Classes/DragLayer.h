//
//  DragLayer.h
//  tableViewForCocosx
//
//  Created by orix on 24/9/12.
//
//

#ifndef tableViewForCocosx_DragLayer_h
#define tableViewForCocosx_DragLayer_h

#include "cocos2d.h"
#include "SNSTableView.h"

#define kDragElementTag 1

USING_NS_CC;

class DragLayer : public CCLayerColor, public SNSTableViewDataSource, public SNSTableViewDelegate
{
public:
    DragLayer();
    ~DragLayer();
    
    virtual bool init();
    
    CREATE_FUNC(DragLayer);
	NODE_FUNC(DragLayer);
public:
    void addDragElement(CCNode *child);
    void removeDragElement();
    void removeElement();
    void putDragElement();      //将拖动的内容物放置在指定地点
    
private:
    CCPoint						m_startPoint;           //纪录开始拖动的点，以让卡片回到初始位置
    bool						m_isDraging;            //是否允许拖动
    CCLayerColor *				m_destinationLayer;     //目标层
    CCArray *					m_data;
	
	CCSize						m_cellSize;
    
protected: /*SNSTableViewDataSource*/
    //根据indexPath返回tableView的cell
    virtual SNSTableViewCellItem* tableViewItemForCell(SNSTableView* tableView, SNSIndexPath* indexPath);
    //返回tableView的cell数
    virtual unsigned int tableViewAllItemsNumber(SNSTableView* tableView, int section);
    
protected: /*SNSTableViewDelegate*/
    //定义tableView每一行或列的高度
    virtual float tableViewHeightForRow(SNSTableView* tableView, SNSIndexPath* indexPath);
    //返回tableView的列数
    unsigned int tableViewColumnsOfItemsNumber(SNSTableView* tableView, int section);
    // 横扫时触发的事件
    virtual void tableViewDeleteActionItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position);
    // 每个item的点击事件
    virtual void tableViewSelectItem(SNSTableView* tableView, SNSIndexPath* indexPath);
    //反馈内容物（item）拖动开始事件
    virtual void tableViewDidDragStartItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position);
    //反馈内容物（item）拖动结束事件
    virtual void tableViewDidDragEndItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position);
	//返回当前页数和总页数
	virtual void tableViewAtPage(SNSTableView* tableView, int page, int pageCount);
    
public:
    virtual void registerWithTouchDispatcher();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};

#endif
