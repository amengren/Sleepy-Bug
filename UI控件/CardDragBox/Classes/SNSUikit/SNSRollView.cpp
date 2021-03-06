//
//  SNSRollView.cpp
//  CardDragBox
//	version 1.0
//  Created by orix on 28/9/12.
//
//

#include "SNSRollView.h"

SNSRollView::SNSRollView()
:m_data(NULL), m_stopIndex(-1), m_rollPointer(0), m_isAdd(false),
m_delegate(NULL), m_backImage(NULL), m_isSlowDown(false), m_nowPointer(0)
{
	m_frame = CCRectMake(0, 0, 0, 0);
	m_pos = ccp(0, 0);
}

SNSRollView::~SNSRollView()
{
	if (NULL != m_data) {
		CC_SAFE_DELETE(m_data);
	}
	if (NULL != m_backImage) {
		CC_SAFE_DELETE(m_backImage);
	}
}

bool SNSRollView::initRollViewFrame(CCRect frame)
{
	if (!SNSView::initViewFrame(frame)) {
		return false;
	}
	//根据frame设置自己的position
	m_frame = frame;
	m_halfFrameHeight = m_frame.size.height * 0.5f;
	this->setPosition(ccp(frame.origin.x, frame.origin.y));
	//设置初始滚动的速度
	m_speed = 0.3f;
	
	return true;
}

SNSRollView* SNSRollView::initWithFrame(CCRect frame)
{
    return SNSRollView::create(frame);
}

SNSRollView* SNSRollView::create(CCRect frame)
{
    SNSRollView* pRet = new SNSRollView();
    
    if ( pRet && pRet->initRollViewFrame(frame) ) {
        pRet->autorelease();
        return pRet;
    }
	CC_SAFE_DELETE(pRet);
    return NULL;
}

#pragma mark - set data and start animation

void SNSRollView::setDelegate(SNSRollViewDelegate *delegate)
{
	m_delegate = delegate;
}

void SNSRollView::setData(CCArray *data, CCSprite *backImage)
{
	m_data = data;
	m_data->retain();
	CCAssert(NULL != backImage, "Back image should not be NULL!");
	m_backImage = backImage;
	m_backImage->retain();  // retain一下这两个元素，防止提前释放
	if (NULL == m_data) return;
	//创建一个独立的sprite，保证数据中的sprite不被修改
	CCSprite *floatSprite = CCSprite::create();
	floatSprite->setDisplayFrame(backImage->displayFrame());	
	floatSprite->setPosition(ccp(m_frame.size.width * 0.5f, m_halfFrameHeight));
	addChild(floatSprite, 0, kSpriteZOrder);
}

void SNSRollView::resetValue()
{
	// 重新设置基础数值
	m_stopIndex = -1;
	m_speed = 0.3f;
	m_rollPointer = 0;
	m_isAdd = false;
	m_isSlowDown = false;
}

void SNSRollView::startAnimation()
{
	if (NULL == m_data) return;
	this->resetValue(); // 重新设置旋转时需要的数值
	schedule(schedule_selector(SNSRollView::update));
}

void SNSRollView::update(float dt)
{
	unsigned int count = m_data->count();
	if (count < 1) return; //低于1个元素就不用滚动了，滚动也没意义
	
	//先移动元素，翻牌完毕再修改指针
	CCSprite *spr = (CCSprite *)this->getChildByTag(kSpriteZOrder);
	if (spr->getScaleX() >= 1) m_isAdd = false;
	if (spr->getScaleX() <= 0) {
		m_isAdd = true;
		//把指针往下移动
		++m_rollPointer;
		CCSprite *newSprite = NULL;
		if (m_speed <= 0) {
			//运动停止时结束循环
			unschedule(schedule_selector(SNSRollView::update));
			//获取应该移动到的图，再播放一次动画
			if (m_stopIndex == -1) {
				newSprite = m_backImage;
			} else {
				newSprite = (CCSprite *)m_data->objectAtIndex(m_stopIndex);
			}
			spr->setDisplayFrame(newSprite->displayFrame());
			// 如果是网络超时状态，那么加速结束
			int duration = (m_stopIndex == -1)?1.2f:1.2f;
			CCScaleTo *scaleChange = CCScaleTo::create(duration, 1.0f);
			CCDelayTime *delay = CCDelayTime::create(0.6f);
			CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(SNSRollView::animationDidStop));
			CCAction *seq = CCSequence::create(scaleChange, delay, call, NULL);
			spr->runAction(seq);
			return;
		}
		//CCLOG("pointer:%d mod:%d", m_rollPointer, m_rollPointer % 2);
		if (m_rollPointer % 2) {
			//转到正面
			int index = (m_nowPointer >= count)?m_nowPointer % count:m_nowPointer;
			//CCLOG("count:%d index:%d", count, index);
			newSprite = (CCSprite *)m_data->objectAtIndex(index);
			++m_nowPointer;
		} else {
			//转到背面
			newSprite = m_backImage;
		}
		spr->setDisplayFrame(newSprite->displayFrame());
		if (m_isSlowDown) m_speed -= (m_stopIndex == -1)?0.15f:0.06f; // 这里如果网络超时，那么直接转到背面，否则转到前面
	}
	// 这里放一个加速以控制翻牌的速度
	float realSpeed = m_speed + 0.06f;
	spr->setScaleX(spr->getScaleX() + (m_isAdd?realSpeed:-realSpeed));
}

void SNSRollView::stop()
{
	m_isSlowDown = true;
}

void SNSRollView::stopAtIndex(int index)
{
	m_stopIndex = index;
	this->stop();
}

void SNSRollView::animationDidStop()
{
	//CCLOG("动画播放结束\0");
	if (NULL != m_delegate) {
		m_delegate->anmitionDidStop();
	}
}
