//
//  SNSColorSprite.h
//  tableView
//	version 2.0
//  Created by yang jie on 28/11/2011.
//  Copyright 2011 ourpalm.com. All rights reserved.
//

#include "SNSColorfulSprite.h"

SNSColorfulSprite::SNSColorfulSprite()
{
	
}


SNSColorfulSprite::~SNSColorfulSprite()
{

}

SNSColorfulSprite* SNSColorfulSprite::create( const char* pszFileName )
{
	SNSColorfulSprite* pSprGrayScale = new SNSColorfulSprite;
	if (pSprGrayScale && pSprGrayScale->initWithFile(pszFileName)) {
		pSprGrayScale->autorelease();
		return pSprGrayScale;
	}
	CC_SAFE_DELETE(pSprGrayScale);
	return NULL;
}

SNSColorfulSprite* SNSColorfulSprite::createWithSpriteFrameName(const char* pszFileName)
{
	SNSColorfulSprite* pSprGrayScale = new SNSColorfulSprite;
	if (pSprGrayScale && pSprGrayScale->initWithSpriteFrameName(pszFileName)) {
		pSprGrayScale->autorelease();
		return pSprGrayScale;
	}
	CC_SAFE_DELETE(pSprGrayScale);
	return NULL;
}

bool SNSColorfulSprite::initWithTexture( cocos2d::CCTexture2D* pTexture, const cocos2d::CCRect& tRect )
{
	do {
		CC_BREAK_IF(!CCSprite::initWithTexture(pTexture, tRect));

		GLchar * pszFragSource = 
			(GLchar *)"#ifdef GL_ES \n \
			precision mediump float; \n \
			#endif \n \
			uniform sampler2D u_texture; \n \
			varying vec2 v_texCoord; \n \
			varying vec4 v_fragmentColor; \n \
			void main(void) \n \
			{ \n \
				// Convert to greyscale using NTSC weightings \n \
				float alpha = texture2D(u_texture, v_texCoord).a; \n \
				float grey = dot(texture2D(u_texture, v_texCoord).rgb, vec3(0.299, 0.587, 0.114)); \n \
				gl_FragColor = vec4(grey, grey, grey, alpha); \n \
			}";
		CCGLProgram* pProgram = new CCGLProgram();
		pProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, pszFragSource);
		this->setShaderProgram(pProgram);
		pProgram->release();
		CHECK_GL_ERROR_DEBUG();

		this->getShaderProgram()->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		this->getShaderProgram()->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		this->getShaderProgram()->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		CHECK_GL_ERROR_DEBUG();

		this->getShaderProgram()->link();
		CHECK_GL_ERROR_DEBUG();

		this->getShaderProgram()->updateUniforms();
		CHECK_GL_ERROR_DEBUG();

		return true;
	} while (0);
	return false;
}

void SNSColorfulSprite::draw()
{
	ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex );
	ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );

	this->getShaderProgram()->use();
	this->getShaderProgram()->setUniformForModelViewProjectionMatrix();

	ccGLBindTexture2D(  this->getTexture()->getName() );

	//
	// Attributes
	//
#define kQuadSize sizeof(m_sQuad.bl)
	long offset = (long)&m_sQuad;

	// vertex
	int diff = offsetof( ccV3F_C4B_T2F, vertices);
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

	// texCoods
	diff = offsetof( ccV3F_C4B_T2F, texCoords);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// color
	diff = offsetof( ccV3F_C4B_T2F, colors);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	CC_INCREMENT_GL_DRAWS(1);
}