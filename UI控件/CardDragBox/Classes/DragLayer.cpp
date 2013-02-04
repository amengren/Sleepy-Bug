//
//  DragLayer.cpp
//  tableViewForCocosx
//
//  Created by orix on 24/9/12.
//
//

#include "DragLayer.h"


DragLayer::DragLayer()
:m_isDraging(false), m_destinationLayer(NULL)
{
    m_data = CCArray::create();
    m_data->retain();
}

DragLayer::~DragLayer()
{
    m_data->release();
}

bool DragLayer::init()
{
    if (!CCLayerColor::init()) {
        return false;
    }
	//循环生成tableView数据
    CCString *str = NULL;
    for (int i = 0; i < 500; i++) {
        str = new CCString("Icon.png");
        m_data->addObject(str);
        str->autorelease();
    }
	//添加tableView到主界面上
	SNSTableView *tableView = SNSTableView::create(CCRectMake(0, 20, 300, 260), TableViewTypeHorizontal);
	tableView->setDelegate(this);
    tableView->setDatasource(this);
	tableView->setPageEnable(true);
	this->addChild(tableView);
    //添加碰撞块
    m_destinationLayer = CCLayerColor::create(ccc4(255, 255, 255, 255));
    m_destinationLayer->setContentSize(CCSizeMake(93, 130));
    m_destinationLayer->setPosition(ccp(360, 200));
	m_destinationLayer->setAnchorPoint(ccp(0.5f, 0.5f));
    this->addChild(m_destinationLayer);
    
    this->setTouchEnabled(true);
    return true;
}

#pragma mark - Drag function

void DragLayer::addDragElement(CCNode *child)
{
    CCAssert(NULL != child, "Element should not be NULL!");
    CCNode *element = getChildByTag(kDragElementTag);
    if (element) removeDragElement();
    addChild(child, 0, kDragElementTag);
    m_isDraging = true;
}

void DragLayer::removeDragElement()
{
    CCNode *element = getChildByTag(kDragElementTag);
    if (element) {
        //归位动画
        CCMoveTo *returnToStartPosition = CCMoveTo::create(0.3f, m_startPoint);
        CCEaseExponentialOut *exponent = CCEaseExponentialOut::create(returnToStartPosition);
        //执行函数
        CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(DragLayer::removeElement));
        
        CCAction *sequence = CCSequence::create(exponent, call, NULL);
        element->runAction(sequence);
    }
}

void DragLayer::removeElement()
{
    m_isDraging = false;
    removeChildByTag(kDragElementTag, true);
}

#pragma mark - touch method

void DragLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
}

bool DragLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCLOG("Drag start");
    CCPoint touchLocation = pTouch->getLocationInView();
    m_startPoint = CCDirector::sharedDirector()->convertToGL(touchLocation);
    return true;
}

void DragLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    //CCLOG("Drag move");
    if (m_isDraging) {
        CCNode *element = getChildByTag(kDragElementTag);
        if (element) {
            CCPoint touchLocation = pTouch->getLocationInView();
            touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
            element->setPosition(touchLocation);
            CCRect rect = CCRectMake(element->getPosition().x - element->getContentSize().width * 0.5f, element->getPosition().y - element->getContentSize().height * 0.5f, element->getContentSize().width, element->getContentSize().height);
            // 先测试一个筐的粘滞效果
            if (rect.containsPoint(m_destinationLayer->getPosition())) {
                CCLOG("开始粘滞");
                CCSprite *spr = CCSprite::create("Icon.png");
                spr->setDisplayFrame(((CCSprite *)element)->displayFrame());
                spr->setPosition(element->getPosition());
                addChild(spr);
                
                //播放粘滞动画
                CCMoveTo *returnToStartPosition = CCMoveTo::create(0.3f, m_destinationLayer->getPosition());
                CCEaseExponentialOut *exponent = CCEaseExponentialOut::create(returnToStartPosition);
                //执行函数
                //CCCallFuncN *call = CCCallFuncN::actionWithTarget(this, callfuncN_selector(DragLayer::removeElement));
                
                CCAction *sequence = CCSequence::create(exponent, NULL);
                spr->runAction(sequence);
                
                removeElement();
            }
        }
    }
}

void DragLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    removeDragElement();
    //CCLOG("Drag end");
}

void DragLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    removeDragElement();
    //CCLOG("Drag cancel");
}

#pragma mark - tableView delegate & datasource

//根据indexPath返回tableView的cell
SNSTableViewCellItem* DragLayer::tableViewItemForCell(SNSTableView* tableView, SNSIndexPath* indexPath)
{
    int num = tableView->getItemPointerWithIndexPath(indexPath);
    SNSTableViewCellItem *cellItem = tableView->dequeueReusableCellItemForIndexPath(indexPath);
	if (NULL == cellItem) {
		cellItem = SNSTableViewCellItem::create();
	}
	/*
	 复用原理: SNSTableView是以一个layer为基础、移动其向量来改变位置的控件，模拟ios的UITableView，复用这里
			同样是模拟UITableView的复用，即：实例化屏幕高度多一行的cell，然后在滑动的时候判断顶部和底部的指针
			进行复用，上下滑动时手指向上拖动，那么最上边的cell移动到最下边以复用，反之同理。
	 复用建议：希望使用者继承SNSTableViewCellItem类，然后在里边画每一个格子，同时把格子里的对象public出来
			在这个函数中判断如果cellItem不为空那么修改他（例如cclabelttf、sprite等），否则创建之后修改之；
			我上边这个方法很蛋疼，只为表现简单数据。
	 */
	if (num < m_data->count()) {
		// 如果为了防止重复，那么删除多余的数据（这里需要使用者人肉清除）
		cellItem->removeAllChildrenWithCleanup(true);
		
		CCString *bgImage = (CCString *)m_data->objectAtIndex(num);
		CCSprite *background = CCSprite::create(bgImage->m_sString.c_str());
		cellItem->addChild(background);
		
		char texts[20];
		sprintf(texts, "%i", num);
		CCLabelTTF *text = CCLabelTTF::create(texts, "Arial", 22.0f);
		text->setColor(ccc3(0, 0, 0));
		cellItem->addChild(text);
	}
    return cellItem;
}

//每个item的点击事件
void DragLayer::tableViewSelectItem(SNSTableView* tableView, SNSIndexPath* indexPath)
{
    int num = tableView->getItemPointerWithIndexPath(indexPath);
    CCLOG("点击了第%d个格子", num);
}

//返回tableView的cell数
unsigned int DragLayer::tableViewAllItemsNumber(SNSTableView* tableView, int section)
{
    return m_data->count();
}

//返回tableView每一行（cell）的列数
unsigned int DragLayer::tableViewColumnsOfItemsNumber(SNSTableView* tableView, int section)
{
    return 4;
}

//定义tableView每一行的高度
float DragLayer::tableViewHeightForRow(SNSTableView* tableView, SNSIndexPath* indexPath)
{
    return 126.0f;
}

//横向滑动的时候产生的事件
void DragLayer::tableViewDeleteActionItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position)
{
    CCLOG("indexPath:%d -- %d positon:%d -- %d", indexPath->getColumn(), indexPath->getRow(), position.x, position.y);
}

//反馈内容物（item）拖动开始事件
void DragLayer::tableViewDidDragStartItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position)
{
    CCLOG("拖动开始");
    int num = tableView->getItemPointerWithIndexPath(indexPath);
    CCString *bgImage = (CCString *)m_data->objectAtIndex(num);
    CCSprite *background = CCSprite::create(bgImage->m_sString.c_str());
    this->addDragElement(background);
}

//反馈内容物（item）拖动结束事件
void DragLayer::tableViewDidDragEndItem(SNSTableView* tableView, SNSIndexPath* indexPath, CCPoint position)
{
    CCLOG("拖动结束");
}

void DragLayer::tableViewAtPage(SNSTableView* tableView, int page, int pageCount)
{
	CCLOG("now page:%d -- pageCount:%d", page, pageCount);
}

