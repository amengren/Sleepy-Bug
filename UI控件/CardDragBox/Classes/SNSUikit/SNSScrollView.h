//
//  SNSScrollView.h
//  SNSUIKit
//	version 2.0
//  Created by yang jie on 24/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#ifndef _SNSUIKIT_SNSSCROLLVIEW_H_
#define _SNSUIKIT_SNSSCROLLVIEW_H_

#include "cocos2d.h"

USING_NS_CC;

#include "SNSView.h"

typedef enum {
	ScrollViewChildTypeINVALID = 0,
	ScrollViewChildTypeBody,
	ScrollViewChildTypeRightSlider,
	ScrollViewChildTypeBottomSlider,
} ScrollViewChildType;

typedef enum {
	ScrollViewStatusINVALID = 0,
	ScrollViewStatusNormal,
	ScrollViewStatusCanNotMove,
	ScrollViewStatusCanNotTouch,
} ScrollViewStatus;

class SNSScrollView;
class SNSScrollViewDelegate
{
public:
    // any offset changes
    virtual void scrollViewDidScroll(SNSScrollView* scrollView){};

    // called when scrollView click event
    virtual void scrollViewDidClickBody(SNSScrollView* scrollView, CCPoint position){};
    
    //当滚动scrollView的时候发送当前滚动百分比（1～100）之间的数值，以方便自定义滚动条(ratio的x和y分别是)
    virtual void scrollViewDidScroll(SNSScrollView* scrollView, CCPoint ratio){};
    
	//scrollView轻扫的时候调用的函数
    virtual void scrollViewCellDeleteAction(SNSScrollView* scrollView, CCPoint ratio){};

};

/*
 */
class SNSScrollView : public SNSView 
{
public:
	CCLayer*					m_body;
	
	CCPoint						m_scrollInertia;
	CCPoint						m_bodyRealPoint;
	CCPoint                     m_scrollStartVector;
    CCPoint                     m_scrollLastVector;
	CCPoint						m_pos;
	CCPoint						m_bodyNeedMove;
    
    //wgx add...
    float                       lineK;
    float                       moveMaxDistance;
    float                       moveMax_x;
    float                       moveMax_y;
    //..........
	
	float						m_sliderOpacity;
	
	ScrollViewStatus			m_status;
	
	bool						m_isCanScroll;
    bool                        m_isDraging;
	bool						m_bounce;
	CC_PROPERTY(bool, m_horizontal, Horizontal);
	CC_PROPERTY(bool, m_vertical, Vertical);
	CC_SYNTHESIZE(bool, m_showSlider, ShowSlider);
	CC_SYNTHESIZE(SNSScrollViewDelegate*, m_delegate, Delegate);
	// 触碰有效区概念，也就是说在这个区域之中的touch事件我们才响应,与剪裁区域区分开来 add by ver 2.0
	CC_SYNTHESIZE(CCRect, m_touchEffectiveArea, TouchEffectiveArea);
	
	long long                   m_startMoveInterval;
    long long                   m_startDragInterval;
    
    CCLayerColor*               m_sliderRight;
    CCLayerColor*               m_sliderBottom;
    
public: /*重定义父类函数*/
    virtual void setFrame(CCRect var);
    virtual void setBodySize(CCSize var);
    virtual void registerWithTouchDispatcher();
    virtual void visit();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    virtual void setIsTouchEnabled(bool enabled);
    
protected:
    SNSScrollView();
    virtual ~SNSScrollView();
    bool initScrollViewFrame(CCRect frames);
    
public:
    CC_DEPRECATED_ATTRIBUTE static SNSScrollView* initWithFrame(CCRect frames);
	static SNSScrollView* create(CCRect frames);
    
protected:
    void updateBody(float delta);
	int autoLength(int length);
	long long getLongTimes();
    
protected:
    //绘制滑动条
    void drawSlider();
    //显示slider
    void showSliders();
    //隐藏slider
    void hideSliders();
    //移动slider
    void moveSliders();
	//移动body的位置
	void moveBodyToPosition(CCPoint position);
	//移动body位置的schdule
	void moveBodySchedule(CCTimer delta);
	//修正body的复用位置
	void fixPosition();
    
public:
    //当scrollView滚动的时候触发的事件
    virtual void scrollViewDidScroll();
    //当scrollView点击的时候触发的事件
    virtual void scrollViewDidClick(CCPoint position);
    //点scrollView横扫的时候触发的事件
    virtual void scrollViewCellDeleteAction(SNSScrollView *scrollView, CCPoint position);
    //scrollView内容物拖动开始的事件
    virtual void scrollViewDidStartDrag(CCPoint position);
    //scrollView内容物拖动结束的事件
    virtual void scrollViewDidEndDrag(CCPoint position);
	
	//设置是否需要显示滑动条
	void setSliderVisible(bool visible);
	//设置slider的颜色以及透明度
    void setSliderColor(ccColor4B color);
    //强制停止scrollView的滚动
    void stopScroll();
    //停止自己的所有action
    void stopActions();
    
public:
    //手动移动body坐标, xy坐标分别传0~100之间的值过去代表他的百分比
    void moveBody(CCPoint ratio);
    //启用/禁用拖动操作
    void setMoveEnable(bool isEnable);
    
};

#endif // _SNSUIKIT_SNSSCROLLVIEW_H_

