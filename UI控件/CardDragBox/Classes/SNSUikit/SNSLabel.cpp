//
//  SNSLabel.cpp
//  WarCraftCardGame
//
//  Created by ourpalm on 11/13/12.
//
//

#include "SNSLabel.h"

SNSLabel::SNSLabel():m_label(NULL), m_shadowColor(ccc3(0, 0, 0)), m_shadowSize(1)
{
	
}

SNSLabel::~SNSLabel()
{
	CC_SAFE_RELEASE(m_label);
}

bool SNSLabel::initWithString(const char* string, const char* fontName, float fontSize, ccColor3B fontColor, ccColor3B shadowColor, float shadowSize)
{
	if (!SNSView::init()) {
		return false;
	}
	m_label = CCLabelTTF::create(string, fontName, fontSize);
	m_label->retain();
	m_label->setAnchorPoint(CCPointZero);
	m_label->setPosition(CCPointZero);
	m_label->setColor(fontColor);
	this->setContentSize(m_label->getContentSize());
	
	m_shadowSize = shadowSize;
	m_shadowColor = shadowColor;
	
	CCTexture2D *texture = createStroke(m_label, m_shadowSize, m_shadowColor);
	CCSprite *shadow = CCSprite::createWithTexture(texture);
	shadow->setPosition(CCPointZero);
	this->addChild(shadow);
	
	return true;
}

SNSLabel* SNSLabel::create(const char* string, const char* fontName, float fontSize, ccColor3B fontColor, ccColor3B shadowColor, float shadowSize)
{
	SNSLabel *instance = new SNSLabel();
	if (instance && instance->initWithString(string, fontName, fontSize, fontColor, shadowColor, shadowSize)) {
		instance->autorelease();
		return instance;
	}
	CC_SAFE_DELETE(instance);
	return NULL;
}

void SNSLabel::setString(const char* text)
{
	this->removeAllChildrenWithCleanup(true);
	m_label->setString(text);
	this->setContentSize(m_label->getContentSize());
	CCTexture2D *texture = createStroke(m_label, m_shadowSize, m_shadowColor);
	CCSprite *shadow = CCSprite::createWithTexture(texture);
	this->addChild(shadow, 0);
}

CCTexture2D* SNSLabel::createStroke(CCLabelTTF* label, float size, ccColor3B color)
{
	CCRenderTexture* rt = CCRenderTexture::create(label->getTexture()->getContentSize().width + size * 2, label->getTexture()->getContentSize().height + size * 2);
	CCPoint originalPos = label->getPosition();
	ccColor3B originalColor = label->getColor();
	bool originalVisibility = label->isVisible();
	label->setColor(color);
	label->setVisible(true);
	ccBlendFunc originalBlend = label->getBlendFunc();
	label->setBlendFunc((ccBlendFunc) { GL_SRC_ALPHA, GL_ONE });
	CCPoint bottomLeft = ccp(label->getTexture()->getContentSize().width * label->getAnchorPoint().x + size, label->getTexture()->getContentSize().height * label->getAnchorPoint().y + size);
	CCPoint positionOffset = ccp(label->getTexture()->getContentSize().width * label->getAnchorPoint().x - label->getTexture()->getContentSize().width * 0.5f, label->getTexture()->getContentSize().height * label->getAnchorPoint().y - label->getTexture()->getContentSize().height * 0.5f);
	CCPoint position = ccpSub(originalPos, positionOffset);
	
	rt->begin();
	label->setFlipY(true);
	/*
	 * 循环画出文字边框，可以根据需要适当调整
	 */
	for (int i = 0; i < 360; i += 30)
	{
		label->setPosition(ccp(bottomLeft.x + sin(CC_DEGREES_TO_RADIANS(i)) * size, bottomLeft.y + cos(CC_DEGREES_TO_RADIANS(i)) * size));
		label->visit();
	}
	/*
	 * 循环画完背景之后再画文字
	 */
	label->setColor(originalColor);
	label->setBlendFunc(originalBlend);
	label->setVisible(originalVisibility);
	label->setPosition(ccp(originalPos.x + size, originalPos.y + size));
	label->visit();
	label->setPosition(originalPos);
	label->setFlipY(false);
	rt->end();
	rt->setPosition(position);
	return rt->getSprite()->getTexture();
}
