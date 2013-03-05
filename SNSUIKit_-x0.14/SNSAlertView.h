//
//  SNSAlertView.h
//  WarCraftCardGame
//
//  Created by ourpalm on 10/20/12.
//
//

#ifndef WarCraftCardGame_SNSAlertView_h
#define WarCraftCardGame_SNSAlertView_h

#include "SNSView.h"

enum SNSAlertViewShowType {
	SNSAlertViewShowTypeINVALID = 0,
	SNSAlertViewShowTypePopup,				// 气泡弹出样式
	SNSAlertViewShowTypeNaughty,			// 顽皮气泡弹出样式
	SNSAlertViewShowTypeDown,				// 掉落效果弹出样式
	SNSAlertViewShowTypeBrake,				// 急刹车效果弹出样式
};

class SNSAlertView;
class SNSAlertStack : public CCObject
{
public:
	SNSAlertStack();
	~SNSAlertStack();
	virtual bool init();
	
	// delegate方法
	void previous(SNSAlertView *alertView = NULL);
	bool next(SNSAlertView* alertView, bool inStack = true);
	
	static SNSAlertStack* sharedStack();
private:
	void show(SNSAlertView *obj = NULL);
	void hide(SNSAlertView *obj = NULL);
	
	void singleHide();
	
	void releaseHide();
private:
	SNSAlertView *				m_nowObject;				// 当前对象
	SNSAlertView *				m_singleObject;				// 关闭时需要传递的指针
	
	CCLayerColor *				m_maskLayer;				// 背景遮罩层（透明黑背景）
	
	CCArray *					m_waitQueue;				// 待弹出队列
	bool						m_isRuning;					// 记录是否在播放动画，如果在播放动画，添加和移除均延后
};

class SNSAlertViewDelegate
{
public:
	virtual void alertViewDidClose(){};
};

class SNSAlertView : public CCLayer
{
public:
	SNSAlertView();
	~SNSAlertView();
	virtual bool initWithNode(CCNode *parent);
	
	// 覆盖touch方法
	void registerWithTouchDispatcher();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	
	// 显示alertView
	virtual void show(bool isInStrack = true);
	// close函数
	virtual void close();

public:
	CC_DEPRECATED_ATTRIBUTE static SNSAlertView* alertWithNode(CCNode *parent);
	static SNSAlertView* create(CCNode *parent);
	
	// 设置alertView delegate
	void setDelegate(SNSAlertViewDelegate *delegate);
	// 设置上一个节点指针
	void setPrevious(SNSAlertView *previous);
	// 获取上一个节点的指针
	SNSAlertView* getPrevious();
	
private:
	SNSAlertViewDelegate*		m_delegate;					// 设置alertViewDelegate
	SNSAlertView*				m_previous;					// 上一节点
	bool						m_alertViewInStack;			// 记录是否在堆栈中（因为有些窗口需要不在堆栈中）
	
	/*
	 * 设置是否需要遮罩层
	 */
	CC_SYNTHESIZE(bool, m_haveMask, MaskEnable);
	
	/*
	 * 显示类型的get & set方法
	 */
	CC_SYNTHESIZE(SNSAlertViewShowType, m_showType, ShowType);
};

#endif
