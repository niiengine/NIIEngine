// From http://www.ogre3d.org/wiki/index.php/AnimationBlender

#include "OgreEntity.h"

using namespace Ogre;

class AnimationBlender
{
public:
	enum BlendingTransition
	{
		BlendSwitch,         // stop source and start dest
		BlendWhileAnimating,   // cross fade, blend source animation out while blending destination animation in
		BlendThenAnimate      // blend source to first frame of dest, when done, start dest anim
	};
public:
	void blend(const String & animation, BlendingTransition transition, NIIf duration, bool loop);
	void addTime( NIIf );
	NIIf getProgress() { return mTimeleft/ mDuration; }
	AnimationFusion *getSource() { return mSource; }
	AnimationFusion *getTarget() { return mTarget; }
	AnimationBlender( GeometrySpace *);
	void init( const String &animation );

	NIIf mTimeleft, mDuration;

	bool complete;
private:
	~AnimationBlender() {}

	GeometrySpace *mEntity;
	AnimationFusion *mSource;
	AnimationFusion *mTarget;

	BlendingTransition mTransition;

	bool loop;
};
