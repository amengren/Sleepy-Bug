//
//  SNSAlertView.cpp
//  WarCraftCardGame
//
//  Created by ourpalm on 10/20/12.
//
//

#include "SNSAlertView.h"

#pragma mark - SNSAlertStack class -

static SNSAlertStack* singleton = NULL;

SNSAlertStack::SNSAlertStack():m_nowObject(NULL), m_isRuning(false), m_maskLayer(NULL)
{
	m_waitQueue = CCArray::create();
	m_waitQueue->retain();
}

SNSAlertStack::~SNSAlertStack()
{
	CC_SAFE_RELEASE_NULL(m_waitQueue);
}

bool SNSAlertStack::init()
{
	return true;
}

SNSAlertStack* SNSAlertStack::sharedStack()
{
	if (NULL == singleton) {
		singleton = new SNSAlertStack();
		if ( singleton && singleton->init() ) {
			return singleton;
		}
		CC_SAFE_DELETE(singleton);
	}
	return singleton;
}

#pragma mark - SNSAlertStack control function

void SNSAlertStack::previous(SNSAlertView *alertView)
{
	if (alertView) {
		hide(alertView);
	} else {
		hide();
	}
}

bool SNSAlertStack::next(SNSAlertView* alertView, bool inStack)
{
	CCAssert(dynamic_cast<SNSAlertView*>(alertView), "alertView should be SNSAlertView's subclass");
	if (alertView) {
		if (inStack) {
			// 判断如果当前没有正在关闭的窗口，那么开始正常弹出
			if (!m_isRuning) {
				if (NULL != m_nowObject) {
					alertView->setPrevious(m_nowObject);
					m_nowObject->setVisible(false);
				}
				m_nowObject = alertView;
				m_nowObject->retain();
				show();
				return true;
			} else {
				// 否则把对象放到待弹出队列中
				m_waitQueue->addObject(alertView);
			}
		} else {
			show(alertView);
		}
	}
	return false;
}

void SNSAlertStack::show(SNSAlertView *obj)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	
	CCAction *seq = NULL;
	SNSAlertView *temp = m_nowObject;
	SNSAlertViewShowType showType;
	if (obj) {
		temp = obj;
		showType = obj->getShowType();
	} else {
		showType = m_nowObject->getShowType();
	}
	// 添加遮罩层，如果已经贴上了，那么先移除之前的
	m_maskLayer = CCLayerColor::create(ccc4(0, 0, 0, 0), winSize.width, winSize.height);
	CCNode *parent = temp->getParent();
	if (parent && temp->getMaskEnable()) {
		// 先删掉maskLayer
		parent->removeChildByTag(9527, true);
		m_maskLayer->setOpacity(0);
		parent->addChild(m_maskLayer, 9998, 9527);
		CCFadeTo *fade = CCFadeTo::create(0.6f, 128.0f);
		m_maskLayer->runAction(fade);
	}
	// 开始处理各种显示效果
	switch (showType) {
		case SNSAlertViewShowTypeINVALID:
		{
			
		}
			break;
		case SNSAlertViewShowTypePopup:
		case SNSAlertViewShowTypeNaughty:	// 这个效果暂时没实现，也不想加太多东西，考虑以后再说吧。。。
		{
			temp->setScale(0.001);
			CCScaleTo *scale = CCScaleTo::create(0.6f, 1.0f);
			CCEaseElasticOut *out = CCEaseElasticOut::create(scale);
			seq = CCSequence::create(out, NULL);
		}
			break;
		case SNSAlertViewShowTypeDown:
		{
			float posY = temp->getPositionY();
			temp->setPositionY(winSize.height * 0.5f + temp->getContentSize().height);
			CCMoveTo *move = CCMoveTo::create(0.6f, ccp(temp->getPositionX(), posY));
			CCEaseBounceOut *out = CCEaseBounceOut::create(move);
			seq = CCSequence::create(out, NULL);
		}
			break;
		case SNSAlertViewShowTypeBrake:
		{
			// 这里由于锚点问题暂不推荐使用
			temp->setSkewX(30.0f);
			float posX = temp->getPositionX();
			m_nowObject->setPositionX(winSize.width * 0.5f + temp->getContentSize().width);
			CCMoveTo *move = CCMoveTo::create(0.3f, ccp(posX, temp->getPositionY()));
			CCEaseExponentialOut *moveOut = CCEaseExponentialOut::create(move);
			CCSkewTo * skew = CCSkewTo::create(0.6, 0.0f, 0.0f);
			CCEaseElasticOut *skewOut = CCEaseElasticOut::create(skew);
			seq = CCSequence::create(moveOut, skewOut, NULL);
		}
			break;
		default:
			break;
	}
	if (obj) {
		// 如果传进来对象，证明这个对象要不入栈
		obj->setVisible(true);
		if (seq) {
			obj->runAction(seq);
		}
	}else if (m_nowObject) {
		m_nowObject->setVisible(true);
		if (seq) {
			m_nowObject->runAction(seq);
		}
	}
}

void SNSAlertStack::hide(SNSAlertView *alertView)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCAction *seq = NULL;
	CCCallFuncN *call = NULL;
	SNSAlertViewShowType showType = SNSAlertViewShowTypeINVALID;
	SNSAlertView *temp = m_nowObject;
	if (alertView) {
		temp = alertView;
		// 这个变量只在隐藏的时候用到，很蛋疼，因为如果在其他地方用的话会导致弹出框相互影响
		m_singleObject = alertView;
		showType = alertView->getShowType();
		call = CCCallFuncN::create(this, callfuncN_selector(SNSAlertStack::singleHide));
	} else {
		if (m_nowObject) {
			showType = m_nowObject->getShowType();
			call = CCCallFuncN::create(this, callfuncN_selector(SNSAlertStack::releaseHide));
		}
	}
	
	switch (showType) {
		case SNSAlertViewShowTypeINVALID:
		{
			if (alertView) {
				singleHide();
			} else {
				releaseHide();
			}
		}
			break;
		case SNSAlertViewShowTypePopup:
		case SNSAlertViewShowTypeNaughty:
		{
			CCScaleTo *scale = CCScaleTo::create(0.3f, 0.01);
			CCEaseExponentialIn *in = CCEaseExponentialIn::create(scale);
			seq = CCSequence::create(in, call, NULL);
		}
			break;
		case SNSAlertViewShowTypeDown:
		{
			CCMoveTo *move = CCMoveTo::create(0.3f, ccp(temp->getPositionX(), winSize.height * 0.5f + temp->getContentSize().height));
			CCEaseExponentialIn *in = CCEaseExponentialIn::create(move);
			seq = CCSequence::create(in, call, NULL);
		}
			break;
		case SNSAlertViewShowTypeBrake:
		{
			CCMoveTo *move = CCMoveTo::create(0.3f, ccp(-winSize.width * 0.5f - temp->getContentSize().width, temp->getPositionY()));
			CCEaseExponentialIn *in = CCEaseExponentialIn::create(move);
			seq = CCSequence::create(in, call, NULL);
		}
			break;
		default:
			break;
	}
	if (alertView) {
		if (seq) {
			alertView->runAction(seq);
		}
	} else if (m_nowObject) {
		if (seq) {
			m_isRuning = true;
			m_nowObject->runAction(seq);
		}
	}
}

void SNSAlertStack::singleHide()
{
	if (m_singleObject) {
		m_singleObject->removeFromParentAndCleanup(true);
		m_singleObject = NULL;
		m_isRuning = false;
		// 移除遮罩层
		if (m_maskLayer) {
			m_maskLayer->removeFromParentAndCleanup(true);
			m_maskLayer = NULL;
		}
	}
}

void SNSAlertStack::releaseHide()
{
	if (m_nowObject) {
		// 先获得上一个节点，再删除当前节点，最后再设置上一个节点为当前节点，不知道我说明白没？
		SNSAlertView *previous = m_nowObject->getPrevious();
		m_nowObject->removeFromParentAndCleanup(true);
		CC_SAFE_RELEASE_NULL(m_nowObject);// 一定要置NULL，否则野指针
		// 移除遮罩层
		if (m_maskLayer) {
			m_maskLayer->removeFromParentAndCleanup(true);
			m_maskLayer = NULL;
		}
		if (NULL != previous) {
			m_nowObject = previous;
			show();
		}
		m_isRuning = false;
		// 判断是否还有待弹出窗口，有则全部弹出他们
		if (m_waitQueue && m_waitQueue->count() > 0) {
			CCObject *obj = NULL;
			SNSAlertView *alert = NULL;
			CCARRAY_FOREACH(m_waitQueue, obj) {
				alert = (SNSAlertView *)obj;
				if (alert) {
					next(alert);
					m_waitQueue->removeObject(obj);
				}
			}
		}
	}
}

#pragma mark - SNSAlertView class -

SNSAlertView::SNSAlertView():m_delegate(NULL), m_previous(NULL), 
m_showType(SNSAlertViewShowTypeINVALID), m_alertViewInStack(true),
m_haveMask(true)
{
	
}

SNSAlertView::~SNSAlertView()
{
	
}

bool SNSAlertView::initWithNode(CCNode *parent)
{
	if (parent && CCLayer::init()) {
		parent->addChild(this, 9999);
		this->setVisible(false);
		this->setTouchEnabled(true);
		this->setAnchorPoint(ccp(0, 0));
		this->ignoreAnchorPointForPosition(false);
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		this->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));
		return true;
	}
	return false;
}

SNSAlertView* SNSAlertView::alertWithNode(CCNode *parent)
{
	return create(parent);
}

SNSAlertView* SNSAlertView::create(CCNode *parent)
{
	SNSAlertView* pRet = new SNSAlertView();
	if ( pRet && pRet->initWithNode(parent) ) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

#pragma makr - SNSAlertView set&get method

void SNSAlertView::setDelegate(SNSAlertViewDelegate *delegate)
{
	m_delegate = delegate;
}

void SNSAlertView::setPrevious(SNSAlertView *previous)
{
	m_previous = previous;
}

SNSAlertView* SNSAlertView::getPrevious()
{
	return m_previous;
}

#pragma mark - SNSAlertView control function

void SNSAlertView::show(bool isInStrack)
{
	m_alertViewInStack = isInStrack;
	SNSAlertStack::sharedStack()->next(this, isInStrack);
}

void SNSAlertView::close()
{
	if (m_alertViewInStack) {
		SNSAlertStack::sharedStack()->previous();
	} else {
		SNSAlertStack::sharedStack()->previous(this);
	}
	if (m_delegate) {
		m_delegate->alertViewDidClose();
	}
}

#pragma mark - Touch Dispatcher function

void SNSAlertView::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
}

bool SNSAlertView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	// 吸收所有的点击事件
	return true;
}

void SNSAlertView::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	
}

void SNSAlertView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	
}

void SNSAlertView::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	
}
