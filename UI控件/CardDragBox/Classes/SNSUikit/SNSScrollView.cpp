//
//  SNSScrollView.m
//  SNSUIKit
//	version 2.0
//  Created by yang jie on 24/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//


#include "SNSScrollView.h"
//#include "SNSUtilities.h"

int sliderWidth = 0;
int sliderMargin = 0;

#define SPACE_TIME (200000)   //触发点击的最小时间(微秒，1000000为一秒)
#define INVALID_TIME (1000000)

SNSScrollView::SNSScrollView()
:m_body(NULL),m_sliderOpacity(0.0),m_startMoveInterval(0.0),m_startDragInterval(0.0),m_delegate(NULL),
m_sliderRight(NULL), m_sliderBottom(NULL),
m_isCanScroll(false),m_isDraging(false),m_bounce(false),m_horizontal(false),m_vertical(false),m_showSlider(false)
{
    m_frame = CCRectMake(0, 0, 0, 0);
    m_scrollInertia = CCPointZero;
	m_bodyRealPoint = CCPointZero;
	m_scrollStartVector = CCPointZero;
    m_scrollLastVector = CCPointZero;
	m_pos = CCPointZero;
	m_touchEffectiveArea = CCRectZero;
	m_bodyNeedMove = CCPointZero;
}

SNSScrollView::~SNSScrollView()
{
    
}

bool SNSScrollView::initScrollViewFrame(CCRect frames)
{
    if ( !SNSView::initWithColor(ccc4(0, 0, 0, 0)) ) {
        return false;
    }
	
	sliderWidth = autoLength(3);
	sliderMargin = autoLength(1);

    //setIsRelativeAnchorPoint(true);
    setAnchorPoint(ccp(0, 0));
    m_scrollInertia = ccp(0, 0);
    m_isCanScroll = false;
    m_bounce = true;
    
    drawSlider();
    setVertical(true);
    setHorizontal(true);
    m_showSlider = true;
    
    m_body = CCLayer::create();
    //m_body->setIsRelativeAnchorPoint(true);
    m_body->setAnchorPoint(ccp(0.f, 0.f));
    addChild(m_body, 0, ScrollViewChildTypeBody);
    
    m_status = ScrollViewStatusNormal;
    
    this->schedule(schedule_selector(SNSScrollView::updateBody));
    this->setIsTouchEnabled(true);
    
    setFrame(frames);
    
    return true;
}

SNSScrollView* SNSScrollView::initWithFrame(CCRect frames)
{
    return SNSScrollView::create(frames);
}

SNSScrollView* SNSScrollView::create(CCRect frames)
{
	SNSScrollView* pRet = new SNSScrollView();
	
	if ( pRet && pRet->initScrollViewFrame(frames) ) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

#pragma mark - slider public function

void SNSScrollView::setSliderVisible(bool visible)
{
	m_showSlider = visible;
}

//设置slider的颜色以及透明度
void SNSScrollView::setSliderColor(ccColor4B color)
{
    if (!m_showSlider) return;
	m_sliderRight->setColor( ccc3(color.r, color.g, color.b) );
	m_sliderRight->setOpacity(0);
	m_sliderBottom->setColor( ccc3(color.r, color.g, color.b) );
	m_sliderBottom->setOpacity(0);
	m_sliderOpacity = color.a;
}

#pragma mark - slider private function

//绘制滑动条
void SNSScrollView::drawSlider()
{
    //if (!m_showSlider) return;
	float sliderRightHeight = 20.0f;
	float sliderBottomWidth = 20.0f;
    
	sliderRightHeight = (m_frame.size.height - sliderMargin * 2) *  m_frame.size.height / m_bodySize.height;
	sliderBottomWidth = (m_frame.size.width - sliderMargin * 2) *  m_frame.size.width / m_bodySize.width;
    
	if (!m_sliderRight) {
		m_sliderRight = CCLayerColor::create(ccc4(155, 155, 155, 0));
        m_sliderRight->setVisible(false);
		m_sliderRight->setAnchorPoint(ccp(0, 0));
        addChild(m_sliderRight, 128, ScrollViewChildTypeRightSlider);
	}
    
    m_sliderRight->setContentSize(CCSizeMake(sliderWidth, sliderRightHeight));
	m_sliderRight->setPosition(ccp(m_frame.size.width - sliderWidth - sliderMargin, m_frame.size.height - sliderRightHeight - sliderMargin));
    
	if (!m_sliderBottom) {
		m_sliderBottom = CCLayerColor::create(ccc4(155, 155, 155, 0));
		m_sliderBottom->setVisible(false);
		m_sliderBottom->setAnchorPoint(ccp(0, 0));
        addChild(m_sliderBottom, 128, ScrollViewChildTypeBottomSlider);
	}
    
    m_sliderBottom->setContentSize(CCSizeMake(sliderBottomWidth, sliderWidth));
    m_sliderBottom->setPosition(ccp(sliderMargin, sliderMargin));
	m_sliderOpacity = 155;
}

//显示slider
void SNSScrollView::showSliders()
{
    if (!m_showSlider) return;
	//CCFadeTo *fade = [CCFadeTo actionWithDuration:0.01f opacity:m_sliderOpacity);
	if (m_vertical) {
        m_sliderRight->setOpacity(m_sliderOpacity);
	}
	if (m_horizontal) {
        m_sliderBottom->setOpacity(m_sliderOpacity);
	}
}

//隐藏slider
void SNSScrollView::hideSliders()
{
    this->unschedule(schedule_selector(SNSScrollView::hideSliders));
    
    if (!m_showSlider) return;
    
	if (m_vertical) {
		m_sliderRight->runAction(CCFadeTo::create(0.3f, 0));
	}
	if (m_horizontal) {
		m_sliderBottom->runAction(CCFadeTo::create(0.3f, 0));
	}
}

//移动slider
void SNSScrollView::moveSliders()
{
    //if (!m_showSlider) return;
    
	float ratio = 0;
	CCPoint ratioPoint = ccp(0, 0);
    
	if (m_vertical) {
		ratio = m_sliderRight->getContentSize().height / m_frame.size.height;
		float yPosition = - (m_body->getPosition().y * ratio);
		float maxPosition = m_frame.size.height - m_sliderRight->getContentSize().height - sliderMargin;
		if (yPosition < sliderMargin) {
			yPosition = sliderMargin;
		}
		if (yPosition >= maxPosition) {
			yPosition = maxPosition;
		}
		m_sliderRight->setPosition( ccp(m_sliderRight->getPosition().x, yPosition) );
		ratioPoint = ccp(ratioPoint.x, 100 - (yPosition / (m_frame.size.height - m_sliderRight->getContentSize().height - sliderMargin)) * 100);
	}
    
	if (m_horizontal) {
		ratio = m_sliderBottom->getContentSize().width / m_frame.size.width;
		float xPosition = -(m_body->getPosition().x * ratio);
		float maxPosition = m_frame.size.width - (m_sliderBottom->getContentSize().width * m_sliderBottom->getScaleX());
		if (xPosition < sliderMargin) {
			/*
			 if (sliderBottom.scaleX >= 0.3f && sliderBottom.scaleX <= 1.0f) {
			 sliderBottom.scaleX -= (sliderMargin - xPosition) / m_frame.size.width;
			 }
			 */
			xPosition = sliderMargin;
		}
		if (xPosition >= maxPosition) {
			/*
			 if (sliderBottom.scaleX >= 0.3f && sliderBottom.scaleX <= 1.0f) {
			 sliderBottom.scaleX -= ((xPosition - maxPosition) / m_frame.size.width);
			 }
			 */
			xPosition = maxPosition;
		}
		m_sliderBottom->setPosition( ccp(xPosition, m_sliderBottom->getPosition().y) );
		ratioPoint = ccp((xPosition / (m_frame.size.width - (m_sliderBottom->getContentSize().width * m_sliderBottom->getScaleX()) - sliderMargin)) * 100, ratioPoint.y);
	}
    
	if ( m_delegate ) {
        m_delegate->scrollViewDidScroll(this, ratioPoint);
	}
}


#pragma mark - overwrite get & set and parent method

void SNSScrollView::setVertical(bool var)
{
    m_vertical = var;
    this->getChildByTag(ScrollViewChildTypeRightSlider)->setVisible(m_vertical);
}

bool SNSScrollView::getVertical()
{ 
    return m_vertical;
}

void SNSScrollView::setHorizontal(bool var)
{
    m_horizontal = var;
    this->getChildByTag(ScrollViewChildTypeBottomSlider)->setVisible(m_horizontal);
}

bool SNSScrollView::getHorizontal()
{
    return m_horizontal;
}

void SNSScrollView::setFrame(CCRect var)
{
    m_frame = var;
    this->setPosition(ccp(var.origin.x, var.origin.y));
    this->setContentSize(CCSizeMake(var.size.width, var.size.height));
    this->setBodySize(this->getContentSize());
	// 重置可点击区域
	m_touchEffectiveArea = CCRectZero;
}

void SNSScrollView::setBodySize(CCSize var)
{
    //如果宽高小于当前frame宽高那么他要等于当前frame宽高
	if (var.width < m_frame.size.width) {
		var.width = m_frame.size.width;
	}
	if (var.height < m_frame.size.height) {
		var.height = m_frame.size.height;
	}
    
	m_bodySize = var;
	m_body->setContentSize(CCSizeMake(0, 0));
    
	//计算当前body应该所在原点的位置
	m_bodyRealPoint = ccp(m_bodySize.width - m_frame.size.width, m_bodySize.height - m_frame.size.height);
	//先让body的向量加上他当前应该在的位置
    m_body->setPosition(ccpSub(CCPointZero, m_bodyRealPoint));
    m_body->setPosition(ccp(0, m_body->getPosition().y));
    this->drawSlider();
}

void SNSScrollView::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, false);
}

void SNSScrollView::visit()
{
    glEnable(GL_SCISSOR_TEST); 
    CCRect frame = m_frame;
    //get world point to set the culling
    m_pos = getParent()->convertToWorldSpace(this->getPosition());
	//frame.origin = ccpAdd(m_pos, frame.origin);
	glScissor(m_pos.x * CC_CONTENT_SCALE_FACTOR(), m_pos.y * CC_CONTENT_SCALE_FACTOR(), 
              frame.size.width * CC_CONTENT_SCALE_FACTOR(), frame.size.height * CC_CONTENT_SCALE_FACTOR());
	
    SNSView::visit();
	glDisable(GL_SCISSOR_TEST);
}

/*
 - (void)draw {
 [super draw);
 glColor4ub(255,255,255,100);
 glLineWidth( 1.f );
 ccDrawLine(m_frame.origin, ccp(m_frame.origin.x + m_frame.size.width,m_frame.origin.y));
 ccDrawLine(ccp(m_frame.origin.x + m_frame.size.width, m_frame.origin.y), ccp(m_frame.origin.x + m_frame.size.width, m_frame.origin.y + m_frame.size.height));
 }
 */

#pragma mark - scrollView move function

void SNSScrollView::updateBody(float delta)
{	
    //移动惯性设置
	if (m_bounce && m_status == ScrollViewStatusNormal) {
        
		if ( m_delegate ) {
            m_delegate->scrollViewDidScroll(this);
        }
        
        scrollViewDidScroll();
        
        //CCLOG("矢量效果:%f -- %f", m_scrollInertia.x, m_scrollInertia.y);
        //根据方向判断如果当body位置小于某些值的时候停止自主移动
        if (m_body->getPosition().x != 0 && (m_body->getPosition().x < -m_bodyRealPoint.x || m_body->getPosition().x > 0)) {
              m_scrollInertia = ccp(0, m_scrollInertia.y);
        }
              
        if (m_body->getPosition().y != 0 && (m_body->getPosition().y < -m_bodyRealPoint.y || m_body->getPosition().y > 0)) {
              m_scrollInertia = ccp(m_scrollInertia.x, 0);
        }
              
        //根据获得的矢量缓速移动
        m_body->setPosition(ccpAdd(m_body->getPosition(), m_scrollInertia));
        //衰减移动矢量
        m_scrollInertia = ccpMult(m_scrollInertia, 0.96f);
              
              
        //保护系统，防止计算出来的数值无限小
        if (m_scrollInertia.x < 0.1f && m_scrollInertia.x > -0.1f) {
              m_scrollInertia.x = 0;
        }
              
        if (m_scrollInertia.y < 0.1f && m_scrollInertia.y > -0.1f) {
            m_scrollInertia.y = 0;
        }
              
        //如果自主移动已经结束，那么判断是否执行归位的action
        if (m_scrollInertia.x == 0 || m_scrollInertia.y == 0) {
            stopActions();
            
            CCPoint pos = m_body->getPosition();
               
            if (pos.y < -m_bodyRealPoint.y) {
                pos.y = -m_bodyRealPoint.y;
            }
               
            if (pos.x < -m_bodyRealPoint.x) {
                pos.x = -m_bodyRealPoint.x;
            }
            
            if (pos.y > 0) {
                pos.y = 0;
            }
               
            if (pos.x > 0) {
                pos.x = 0;
            }
               
            //m_scrollInertia = ccpAdd(m_scrollInertia, ccpSub(pos, m_body.position)); //这样做会变成简协振动
            //NSLog(@"pos desc:%f -- %f", pos.x, pos.y);
            CCMoveTo *move = CCMoveTo::create(0.8f, pos);
            CCEaseExponentialOut *ease = CCEaseExponentialOut::create(move);
            m_body->runAction(ease);
            /*
			// 这里准备做slider的弹性移动
            if (m_showSlider) {
                
                 CCScaleTo *transform = [CCScaleTo actionWithDuration:0.8f scale:1.0f);
                 CCEaseExponentialOut *transformEase = [CCEaseExponentialOut actionWithAction:transform);
                 
                 if (m_vertical) {
                 CCLayerColor *sliderRight = (CCLayerColor *)getChildByTag:ScrollViewChildTypeRightSlider);
                 [sliderRight runAction:transformEase);
                 
                 //				float ratio = sliderRight.contentSize.height / m_frame.size.height;
                 //				CCMoveTo *sliderMove = [CCMoveTo actionWithDuration:0.8f position:ccp(sliderRight.position.x, -(m_body.position.y * ratio)));
                 //				CCEaseExponentialOut *sliderMoveEase = [CCEaseExponentialOut actionWithAction:sliderMove);
                 //				[sliderRight runAction:sliderMoveEase);
                 }
                 if (m_horizontal) {
                 CCLayerColor *sliderBottom = (CCLayerColor *)getChildByTag:ScrollViewChildTypeBottomSlider);
                 [sliderBottom runAction:transformEase);
                 
                 float ratio = sliderBottom.contentSize.width / m_frame.size.width;
                 float xPosition = -(m_body.position.x * ratio);
                 float maxPosition = m_frame.size.width - (sliderBottom.contentSize.width * sliderBottom.scaleX);
                 if (xPosition > maxPosition) {
                 CCMoveTo *sliderMove = [CCMoveTo actionWithDuration:0.8f position:ccp(((m_bodySize.width - m_frame.size.width) * ratio), sliderBottom.position.y));
                 CCEaseExponentialOut *sliderMoveEase = [CCEaseExponentialOut actionWithAction:sliderMove);
                 [sliderBottom runAction:sliderMoveEase);
                 }
                 }
                 
            }
			*/
        }
        
        //当移动矢量为0时停止移动
        if (m_scrollInertia.x == 0 && m_scrollInertia.y == 0) {
            if (m_showSlider)
                this->schedule(schedule_selector(SNSScrollView::hideSliders), 0.3f);
            this->unschedule(schedule_selector(SNSScrollView::updateBody));
        }
    }
    
}

//强制停止scrollView的滚动
void SNSScrollView::stopScroll()
{
    m_scrollInertia = ccp(0, 0);
    
    this->unschedule(schedule_selector(SNSScrollView::updateBody));
    stopActions();
    
	CCPoint pos = m_body->getPosition();
	if (pos.y < -m_bodyRealPoint.y) {
		pos.y = -m_bodyRealPoint.y;
	}
	if (pos.x < -m_bodyRealPoint.x) {
		pos.x = -m_bodyRealPoint.x;
	}
	if(pos.y > 0) {
		pos.y = 0;
	}
	if (pos.x > 0) {
		pos.x = 0;
	}
    
	m_body->setPosition(pos);
}

//停止自己的所有action
void SNSScrollView::stopActions()
{
    m_body->stopAllActions();
    
	if (m_vertical) {
        m_sliderRight->stopAllActions();
	}
	if (m_horizontal) {
        m_sliderBottom->stopAllActions();
	}
}

bool SNSScrollView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    moveMax_x = 0;
    moveMax_y = 0;
    moveMaxDistance = 0;
	//NSLog(@"touch start");
    if ( m_status != ScrollViewStatusCanNotTouch ) {
        CCPoint touchLocation = pTouch->getLocationInView();
        m_scrollStartVector = m_scrollLastVector = touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
        this->stopActions();
        m_isCanScroll = false;
        
		//对点击区域进行偏转
		if (m_touchEffectiveArea.size.width == 0 && m_touchEffectiveArea.size.height == 0) {
			// 如果可点击区域的宽和高都为0，那么给初始默认值
			m_touchEffectiveArea = CCRectMake(m_pos.x, m_pos.y, m_frame.size.width, m_frame.size.height);
		}
		if(m_touchEffectiveArea.containsPoint(touchLocation)) {
            this->unscheduleAllSelectors();
            showSliders();
            
            //NSLog(@"点击到view区域");
            m_startDragInterval = m_startMoveInterval = getLongTimes();
            m_isCanScroll = true;
            return true;
        }
    }
	return false;
}

void SNSScrollView::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) 
{
	//NSLog(@"Move body");
    CCPoint touchLocation = pTouch->getLocationInView();
    CCPoint prevLocation = pTouch->getPreviousLocationInView();
    
    touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
    prevLocation = CCDirector::sharedDirector()->convertToGL(prevLocation);
    CCPoint diff = ccpSub(touchLocation, prevLocation);
    float distance = ccpDistance(touchLocation, prevLocation);
    
	if(m_isCanScroll){
       
		//CCLOG("移动的距离：%f", distance);
        
        //NSLog(@"diff desc:%f -- %f", diff.x, diff.y);
		//判断时间大于阈值且移动速度大于阈值那么重新设置上一次的移动矢量且更新开始移动的时间
        if ( getLongTimes() - m_startMoveInterval > SPACE_TIME && distance > 30 ) {
            m_startMoveInterval = getLongTimes();
            m_scrollLastVector = touchLocation;
        }
        
        if (!m_horizontal) {
            diff.x = 0;
        }
        
        if (!m_vertical) {
            diff.y = 0;
        }
        
        //如果超过边界区域，移动减半
        if( m_body->getPosition().y < -m_bodyRealPoint.y || m_body->getPosition().x < -m_bodyRealPoint.x || m_body->getPosition().y > 0 || m_body->getPosition().x > 0 ){
            diff = ccpMult(diff, 0.5f);
        }
        
        //设置body当前位置
        CCPoint pos = ccpAdd(m_body->getPosition(), diff);
        
        //如果不可以bounce的话到边界停止弹动
        if (!m_bounce) {
            if(pos.y < -m_bodyRealPoint.y) {
                pos.y = -m_bodyRealPoint.y;
            }
            if (pos.x < -m_bodyRealPoint.x) {
                pos.x = -m_bodyRealPoint.x;
            }
            if(pos.y > 0) {
                pos.y = 0;
            }
            if (pos.x > 0) {
                pos.x = 0;
            }
        }
        
        if (m_status == ScrollViewStatusNormal) 
            m_body->setPosition(pos);
        
        if ( m_delegate ) {
            m_delegate->scrollViewDidScroll(this);
        }
        scrollViewDidScroll();
        
        int maxX = abs(int(m_scrollStartVector.x - touchLocation.x));
        int maxY = abs(int(m_scrollStartVector.y - touchLocation.y));
        float k = maxX*maxX + maxY*maxY;
        if(k < 0){
            k *= -1; 
        }
        float maxDis = sqrt(k);
        if(moveMax_x < maxX){
            moveMax_x = maxX; 
        }
        if(moveMax_y < maxY){
            moveMax_y = maxY; 
        }
        
        if(moveMaxDistance < maxDis){
            moveMaxDistance = maxDis;
        }
        //斜率
        lineK = (touchLocation.y - m_scrollStartVector.y) / (touchLocation.x - m_scrollStartVector.x);
	}
    //判断时间阈值在指定范围内且移动速度小于阈值那么触发拖动开始事件
    if (!m_isDraging && getLongTimes() - m_startDragInterval > SPACE_TIME && getLongTimes() - m_startDragInterval < INVALID_TIME && distance < 10) {
        scrollViewDidStartDrag(ccpSub(ccpSub(touchLocation, m_pos), m_body->getPosition()));
        m_startDragInterval -= 1000000;
        m_isDraging = true;
    }
}

void SNSScrollView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    //NSLog(@"移动结束");
    CCPoint touchLocation = pTouch->getLocationInView();
    touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
    CCPoint moveLength = ccpSub(touchLocation, m_scrollLastVector);
    CCPoint returnPosition = ccpSub(ccpSub(touchLocation, m_pos), m_body->getPosition());
    if(m_isCanScroll) {
        //如果最后一次手指移开与手指落下时间间隔大于一定时间，那么判断这次操作无效
        long long timeDistance = (long long)(getLongTimes() - m_startMoveInterval);
		//CCLOG("now time!:%lld", getLongTimes());
		//CCLOG("start time!:%lld", m_startMoveInterval);
		//CCLOG("my time!:%lld", timeDistance);
        //获得公共传递用的position
        
        if (timeDistance < SPACE_TIME) {
            
            //设置移动矢量，并且乘以一个小于1的系数，让他移动这不那么快
            m_scrollInertia = ccpAdd(m_scrollInertia, ccpMult(moveLength, 0.25f));
            
            if (!m_horizontal) {
                m_scrollInertia.x = 0;
            }
            if (!m_vertical) {
                m_scrollInertia.y = 0;
            }
            
            //总移动距离小于10的时候清空移动矢量并触发点击事件
            if (ccpDistance(touchLocation, m_scrollStartVector) < 10) {
                //这里判断矢量于0之间的距离如果小于上边10点的位移再乘以衰减系数(用来判断是否是在滑动过程中的点击)
                if (ccpDistance(m_scrollInertia, CCPointZero) <= 10.0f * 0.4f) {
					//先清空，否则在移动到指定位置的时候会有问题
					m_scrollInertia = ccp(0, 0);
                    //如果移动矢量小于一定数值发送点击事件
                    if ( m_delegate ) {
                        m_delegate->scrollViewDidClickBody(this, returnPosition);
                    }
                    scrollViewDidClick(returnPosition);
                    return;
                 }
                m_scrollInertia = ccp(0, 0);
            }
        } else {
            //否则如果没有设置分页的情况下清空移动矢量
            if (moveLength.x > 0 && m_horizontal) {
                m_scrollInertia.x = 0.11;
            } else if (moveLength.y > 0 && m_vertical) {
                m_scrollInertia.y = 0.11;
            } else if (moveLength.x < 0 && m_horizontal) {
                m_scrollInertia.x = -0.11;
            } else if (moveLength.y < 0 && m_vertical) {
                m_scrollInertia.y = -0.11;
            }
        }
        
        //斜率.
        if (lineK < 0.25 && lineK > -0.25 && moveMaxDistance > 20) {
            if (m_delegate) {
                m_delegate->scrollViewCellDeleteAction(this, m_body->getPosition());
            }
            scrollViewCellDeleteAction(this, returnPosition);
        }
    }
    // 结束拖动
    if (m_isDraging) {
        scrollViewDidEndDrag(returnPosition);
        m_isDraging = false;
    }
        
    //NSLog(@"距离：%f", ccpDistance(touchLocation, m_scrollLastVector));
	this->schedule(schedule_selector(SNSScrollView::updateBody));
}

void SNSScrollView::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
     stopScroll();
}

void SNSScrollView::moveBody(CCPoint ratio)
{    
    //水平移动
    if ( m_horizontal ) {
        ratio.x = MAX(ratio.x, 0);
        ratio.x = MIN(ratio.x, 100);
        
        m_body->setPosition(ccp((m_frame.size.width-m_bodySize.width) * 0.01 * ratio.x, m_body->getPosition().y));
    }
    
    //垂直移动
    if ( m_vertical ) {
        ratio.y = MAX(ratio.y, 0);
        ratio.y = MIN(ratio.y, 100);
        
        m_body->setPosition(ccp(m_body->getPosition().x, (m_frame.size.height-m_bodySize.height) * 0.01 * ratio.y));
    }
    moveSliders();
}

void SNSScrollView::setMoveEnable(bool isEnable)
{
	if (isEnable) {
		if (m_status != ScrollViewStatusCanNotTouch) {
			m_status = ScrollViewStatusNormal;
		}
	} else {
		m_status = ScrollViewStatusCanNotMove;
	}
}

void SNSScrollView::setIsTouchEnabled(bool enabled)
{
    SNSView::setTouchEnabled(enabled);
    
	if (enabled) {
		if (m_status != ScrollViewStatusCanNotMove) {
			m_status = ScrollViewStatusNormal;
		}
	} else {
		m_status = ScrollViewStatusCanNotTouch;
	}
}

#pragma mark - self function

//修正scrollView复用的位置
void SNSScrollView::fixPosition()
{
	m_scrollInertia = ccp(-0.11f, -0.11f);
	this->schedule(schedule_selector(SNSScrollView::updateBody));
}

void SNSScrollView::moveBodyToPosition(CCPoint position)
{
	m_bodyNeedMove = ccpSub(position, m_body->getPosition());
	this->schedule(schedule_selector(SNSScrollView::moveBodySchedule));
}

void SNSScrollView::moveBodySchedule(CCTimer delta)
{
	// 如果移动到边框了的话停止移动
	if (m_body->getPosition().x != 0 && (m_body->getPosition().x < -m_bodyRealPoint.x || m_body->getPosition().x > 0)) {
		m_bodyNeedMove = ccp(0, m_bodyNeedMove.y);
	}
	
	if (m_body->getPosition().y != 0 && (m_body->getPosition().y < -m_bodyRealPoint.y || m_body->getPosition().y > 0)) {
		m_bodyNeedMove = ccp(m_bodyNeedMove.x, 0);
	}
	CCPoint subValue = ccpMult(m_bodyNeedMove, 0.2);
	//设置body当前位置
	CCPoint pos = ccpAdd(m_body->getPosition(), subValue);
	
	//如果不可以bounce的话到边界停止弹动
	if (!m_bounce) {
		if(pos.y < -m_bodyRealPoint.y) {
			pos.y = -m_bodyRealPoint.y;
		}
		if (pos.x < -m_bodyRealPoint.x) {
			pos.x = -m_bodyRealPoint.x;
		}
		if(pos.y > 0) {
			pos.y = 0;
		}
		if (pos.x > 0) {
			pos.x = 0;
		}
	}
	
	if (m_status == ScrollViewStatusNormal)
		m_body->setPosition(pos);
	
	if ( m_delegate ) {
		m_delegate->scrollViewDidScroll(this);
	}
	scrollViewDidScroll();
	// 等比衰减移动
	m_bodyNeedMove = ccpSub(m_bodyNeedMove, subValue);
	//CCLOG("%f -- %f", m_bodyNeedMove.y, subValue.y);
	if (m_bodyNeedMove.equals(CCPointZero)) {
		this->unschedule(schedule_selector(SNSScrollView::moveBodySchedule));
		this->fixPosition();
	}
}

//当scrollView滚动的时候触发的事件
void SNSScrollView::scrollViewDidScroll()
{
    //可以在子类中重写这个方法
	moveSliders();
}

//当scrollView点击的时候触发的事件
void SNSScrollView::scrollViewDidClick(CCPoint position)
{
    //在子类中重写这个方法即可获得当前点击的位置
}

void SNSScrollView::scrollViewCellDeleteAction(SNSScrollView *scrollView, CCPoint position)
{
    //在子类中重写这个方法即可获得当前清扫最后的位置
}

//scrollView内容物拖动开始的事件
void SNSScrollView::scrollViewDidStartDrag(CCPoint position)
{
    //在子类中重写这个方法可以获得内容物拖动开始的事件
}

//scrollView内容物拖动结束的事件
void SNSScrollView::scrollViewDidEndDrag(CCPoint position)
{
    //在子类中重写这个方法可以获得内容物拖动结束的事件
}

#pragma mark - helper function

int SNSScrollView::autoLength(int length)
{
	CCSize mainFrame = CCDirector::sharedDirector()->getWinSize();
    if ( mainFrame.width > 480 ) {
		length <<= 1;
	}
	return length;
}

long long SNSScrollView::getLongTimes()
{
	struct timeval tvStart;
	gettimeofday(&tvStart,NULL);
	long long tStart = (long long)1000000*tvStart.tv_sec+tvStart.tv_usec;
	return tStart;
}
