//
//  DD9SpriteBatchNode.h
//  typhoonwar
//
//  Created by ding huidong on 12-10-17.
//
//

#ifndef typhoonwar_DD9SpriteBatchNode_h
#define typhoonwar_DD9SpriteBatchNode_h

#include "cocos2d.h"
USING_NS_CC;

/*
 九宫格精灵节点类
 */
class DD9SpriteBatchNode : public CCSpriteBatchNode {
protected:
    bool initCreate(const char* _fileName);
public:
    static DD9SpriteBatchNode* create(const char* _fileName);
    virtual ~DD9SpriteBatchNode();
    
protected:
    // 原始帧
    CCSprite* m_pSrcSprite;
    // 切分
    CCArray* m_pCapArray;
    // 角地宽高
    CCSize m_angle;
    
protected:
    // 分割9个角
    void spliteToCaps();
    
    void setPoint(CCSprite* _pSprite, float _x, float _y);
    void setWidth(CCSprite* _pSprite, float _w);
    void setHeight(CCSprite* _pSprite, float _h);
    
public:
    //设置身体矩形
    void setContentSize(CCSize size);
    
};


#endif
