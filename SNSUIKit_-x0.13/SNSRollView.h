//
//  SNSRollView.h
//  CardDragBox
//	version 1.0
//  Created by orix on 28/9/12.
//
//

#ifndef CardDragBox_SNSRollView_h
#define CardDragBox_SNSRollView_h

#include "SNSView.h"

#define kSpriteZOrder 3

class SNSRollViewDelegate
{
public:
	virtual void anmitionDidStop() = 0;
};

class SNSRollView : public SNSView
{
protected:
    SNSRollView();
    virtual ~SNSRollView();
    bool initRollViewFrame(CCRect frame);
	
	void update(float dt);
	
	SNSRollViewDelegate *	m_delegate;			// delegate
	CCArray *				m_data;				// 图片数据，里边对象类型是CCSprite
	CCSprite *				m_backImage;		// 卡牌背面的图片
	int						m_stopIndex;		// 停止位置在数据中的索引
	CCPoint					m_pos;				// 位置
	float					m_speed;			// 移动速度
	float					m_halfFrameHeight;	// 半个frame的高度
	int						m_rollPointer;		// 移动指针，判断当前移动到第几个元素了
	bool					m_isAdd;			// 判断是否是放大卡牌操作（模拟旋转）
	bool					m_isSlowDown;		// 是否允许减速（减速的话旋转就会结束）
	
public:
    CC_DEPRECATED_ATTRIBUTE static SNSRollView* initWithFrame(CCRect frame);//DEPRECATED弃用
	static SNSRollView* create(CCRect frames);
	
	// 设置delegate
	void setDelegate(SNSRollViewDelegate *delegate);
	// 设置数据（ccsprite数组）
	void setData(CCArray *data, CCSprite *backImage);
	// 重置基础数值
	void resetValue();
	// 开始播放动画
	void startAnimation();
	// 停止播放动画,并且停止在背面
	void stop();
	// 设置最终停止的位置并播放停止动画
	void stopAtIndex(int index);
	// 结束播放动画的回调函数
	void animationDidStop();
	
public: /* 重写义父类函数 */
	virtual void visit();
	
};

#endif
