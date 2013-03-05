//
//  SNSLabel.h
//  WarCraftCardGame
//
//  Created by ourpalm on 11/13/12.
//
//

#ifndef __WarCraftCardGame__SNSLabel__
#define __WarCraftCardGame__SNSLabel__

#include "SNSView.h"

class SNSLabel : public SNSView
{
public:
	SNSLabel();
	~SNSLabel();
	virtual bool initWithString(const char* string, const char* fontName, float fontSize, ccColor3B fontColor, ccColor3B shadowColor, float shadowSize);
	
	static SNSLabel* create(const char* string, const char* fontName, float fontSize, ccColor3B fontColor = ccc3(255, 255, 255), ccColor3B shadowColor = ccc3(0, 0, 0), float shadowSize = 1);
	
public:
	CCTexture2D* createStroke(CCLabelTTF* label, float size, ccColor3B color);
	
	virtual void setString(const char* text);
private:
	/* 作为印章的label */
	CCLabelTTF*				m_label;
	/* 阴影的颜色 */
	ccColor3B				m_shadowColor;
	/* 阴影线条的粗细 */
	float					m_shadowSize;
};

#endif /* defined(__WarCraftCardGame__SNSLabel__) */
