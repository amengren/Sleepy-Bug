//------------------------------------------------------------------------
//
//	SCMLAnimator : KickStarter project Spriter renderer for cocos2d-x.
//
//	Spriter website : http://www.kickstarter.com/projects/539087245/spriter
//
//	Licensed under the BSD license, see LICENSE in root for details.
// 
//	Copyright (c) 2012 James Hui (a.k.a. Dr.Watson)
// 
//	For latest updates, please visit http://jameshui.com
//
//------------------------------------------------------------------------


#ifndef _CC_SPRITER_X_H_
#define _CC_SPRITER_X_H_

#include <vector>
#include <string>

#include "cocos2d.h"

#include "tinyxml.h"


class CCSpriterX;

namespace SCMLHelper
{

	struct File
	{
		File();
		~File();

		void Init(TiXmlNode *node);

		int id;
		std::string name;
		float width;
		float height;

		cocos2d::CCSprite *sprite;

	};

	class Folder
	{
	public:
		Folder();
		~Folder();
		
		void Init(TiXmlNode *node);
		
		int GetFileCount();
		File *GetFile(int index);

	private:
		int mId;
		std::string mName;

		std::vector <File *> mFiles;

	};

	struct ObjectRef
	{
		void Init(TiXmlNode *node);

		int id;
		int timeline;
		int key;
		int z_index;
	};

	struct Object
	{
		void Init(TiXmlNode *node, CCSpriterX *animator);

		int folder;
		int file;
		float x;
		float y;
		float angle;
		float pivot_x;
		float pivot_y;
		int z_index;

		cocos2d::CCSprite *sprite;

	};

	class Key
	{
	public:
		Key();
		~Key();
		
		void Init(TiXmlNode *node, CCSpriterX *animator);
		
		int GetObjectRefCount();
		ObjectRef *GetObjectRef(int index);
		
		int GetObjectCount();
		Object *GetObject(int index);

		float GetTime();

		bool IsSpinCounterClockwise();

	private:
		int mId;
		float mTime;
		bool mSpinCounterClockwise;

		std::vector <Object *> mObjects;
		std::vector <ObjectRef *> mObjectRefs;
		// will have bones later

	};

	class Timeline
	{
	public:
		Timeline();
		~Timeline();

		void Init(TiXmlNode *node, CCSpriterX *animator);
		int GetKeyframeCount();
		Key *GetKeyframe(int index);

	private:
		int mId;
		std::vector <Key *> mKeyframes;
	};


	class Animation
	{
	public:
		Animation();
		~Animation();

		void Update(float dt);
		void Init(TiXmlNode *node, CCSpriterX *animator);

		void Render();

		bool IsDone();

		void Restart();

		void SetPosition(const cocos2d::CCPoint &pos);
        
        void SetLoop(bool isloop);
	private:
		int mId;
		std::string mName;
		float mLength;
		bool mLooping;
		bool mDone;

		Timeline *mMainline;
		int mCurrKeyframe;

		std::vector <Timeline *> mTimelines;

		float mTimer;

		cocos2d::CCPoint mPosition;

	};


	class Entity
	{
	public:
		Entity();
		~Entity();
		
		void Update(float dt);
		void Render();

		void SetId(int id);
		void SetName(const char *name);
		void AddAnimation(Animation *animation);

		void SetPosition(const cocos2d::CCPoint &pos);

		void NextAnimation();

        void SetAnimation(int aniID);
        
        void SetLoop(bool isloop);
	private:

		int mId;
		std::string mName;

		std::vector <Animation *> mAnimations;

		int mCurrAnimation;

	};

}

/*
 *  spriter精灵操作类
 */
class CCSpriterX : public cocos2d::CCNode
{
public:
	~CCSpriterX();
    //默认使用图片加载，设置为true使用plist
	static CCSpriterX* create(const char *filename, bool isPlist=false);
    
protected:
    CCSpriterX();
    bool initWithFile(const char *filename, bool isPlist);
    
public:
    //获取指定帧精灵
    cocos2d::CCSprite *getSprite(int folderId, int fileId);
    
    //设置做点点
	void setPosition(const cocos2d::CCPoint &pos);
    
    //设置播放动画ID号
    void setanimaID(int id);
    
    //切换到下一个动画
	void PlayNext();
    
    //是否循环播放
    void setisloop(bool isloop);
    
protected:
    //绘制
	virtual void draw(void);
    
    //更新
	virtual void update(float dt);

private:

	std::vector <SCMLHelper::Folder *> mFolders;
	std::vector <SCMLHelper::Entity *> mEntities;

	int mCurrEntity;
};



#endif
