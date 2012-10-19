//
//  DD9SpriteBatchNode.cpp
//  typhoonwar
//
//  Created by ding huidong on 12-10-17.
//
//

#include "DD9SpriteBatchNode.h"

#pragma mark - DD9SpriteBatchNode

DD9SpriteBatchNode::~DD9SpriteBatchNode()
{
    CC_SAFE_RELEASE(m_pSrcSprite);
    CC_SAFE_RELEASE(m_pCapArray);
}

bool DD9SpriteBatchNode::initCreate(const char* _fileName)
{
    m_pSrcSprite = CCSprite::create(_fileName);
    m_pSrcSprite->retain();
    
    if ( !CCSpriteBatchNode::initWithTexture(m_pSrcSprite->getTexture(), 0) ) {
        return false;
    }
    
    this->setPosition(ccp(0, 0));
    this->setAnchorPoint(ccp(0, 0));
    
    m_pCapArray = CCArray::createWithCapacity(9);
    m_pCapArray->retain();
    
    m_angle.width = roundf(m_pSrcSprite->getTextureRect().size.width * 0.25);
    m_angle.height = roundf(m_pSrcSprite->getTextureRect().size.height * 0.25);
    
    //裁剪出9个精灵
    spliteToCaps();
    
    return true;
}

DD9SpriteBatchNode* DD9SpriteBatchNode::create(const char* _fileName)
{
    DD9SpriteBatchNode* pRet = new DD9SpriteBatchNode();
    
    if ( pRet && pRet->initCreate(_fileName) ) {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return pRet;
}

void DD9SpriteBatchNode::spliteToCaps()
{
    CCAssert(m_pCapArray->count() == 0, "The caps already to splited!");
    
    this->removeAllChildrenWithCleanup(true);
    
    CCRect frameRect = m_pSrcSprite->getTextureRect();
    
    // 图片坐标系为左上
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            
            CCRect rect;
            
            if (i == 1) {
                rect.size.height = frameRect.size.height - m_angle.height * 2;
                rect.origin.y = frameRect.origin.y + m_angle.height;
            }
            else {
                rect.size.height = m_angle.height;
                if(i == 0){
                    rect.origin.y = frameRect.origin.y;
                }else{
                    rect.origin.y = frameRect.origin.y + frameRect.size.height - m_angle.height;
                }
            }
            
            if (j == 1) {
                rect.size.width = frameRect.size.width - m_angle.width * 2;
                rect.origin.x = frameRect.origin.x + m_angle.width;
            }
            else {
                rect.size.width = m_angle.width;
                if(j == 0){
                    rect.origin.x = frameRect.origin.x;
                }else{
                    rect.origin.x = frameRect.origin.x + frameRect.size.width - m_angle.width;
                }
            }
            
//            CCLOG("x=%f, y=%f, w=%f, h=%f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
            CCSprite* cap = CCSprite::createWithTexture(this->getTexture(), rect);
            m_pCapArray->addObject(cap);
            this->addChild(cap);
            cap->setPosition(ccp(0, 0));
            cap->setAnchorPoint(ccp(0, 0));
            
        } // for j
    } // for i
    
    this->setContentSize(frameRect.size);
}

#define SET_POINT(_SPR_,_X_,_Y_) (_SPR_->setPosition(ccp(_X_, _Y_)))
#define SET_WIDTH(_SPR_,_W_) (_SPR_->setScaleX(_W_/_SPR_->getContentSize().width))
#define SET_HEIGHT(_SPR_,_H_) (_SPR_->setScaleY(_H_/_SPR_->getContentSize().height))

void DD9SpriteBatchNode::setContentSize(CCSize size)
{
    float mw = MAX(0, size.width - m_angle.width * 2);
    float mh = MAX(0, size.height - m_angle.height * 2);
    
    CCSpriteBatchNode::setContentSize(CCSizeMake(mw + m_angle.width * 2, mh + m_angle.height * 2));
    
    CCObject* pObject = NULL;
    CCSprite* pSprite = NULL;
    int i = 0;
    CCARRAY_FOREACH(m_pCapArray, pObject) {
        pSprite = (CCSprite*)pObject;
        if (i == 0) {
            // 上左
            SET_POINT(pSprite, 0, m_angle.height + mh);
        }else if(i == 1){
            // 上中
            SET_POINT(pSprite, m_angle.width , m_angle.height + mh);
            SET_WIDTH(pSprite, mw);
        }
        else if(i == 2){
            // 上右
            SET_POINT(pSprite, m_angle.width + mw, m_angle.height + mh);
        }else if(i == 3){
            // 中左
            SET_POINT(pSprite, 0, m_angle.height);
            SET_HEIGHT(pSprite, mh);
        }else if(i == 4){
            // 中中
            SET_POINT(pSprite, m_angle.width, m_angle.height);
            SET_WIDTH(pSprite, mw);
            SET_HEIGHT(pSprite, mh);
        }else if(i == 5){
            // 中右
            SET_POINT(pSprite, mw + m_angle.width, m_angle.height);
            SET_HEIGHT(pSprite, mh);
        }else if(i == 6){
            // 下左
        }else if(i == 7){
            // 下中
            SET_POINT(pSprite, m_angle.width, 0);
            SET_WIDTH(pSprite, mw);
        }else if(i == 8){
            // 下右
            SET_POINT(pSprite, mw + m_angle.width, 0);
        }
        i++;
    }
}

