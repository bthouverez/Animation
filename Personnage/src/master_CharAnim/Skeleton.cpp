
#include "Skeleton.h"

using namespace simea;

void Skeleton::init(const BVH& bvh)
{
	for (int ii = 0; ii < bvh.getNumberOfJoint(); ++ii) {
		BVHJoint joint = bvh.getJoint(ii);
		SkeletonJoint skej;
		skej.m_parentId = joint.getParentId();
		skej.m_l2w = Identity();
		m_joints.push_back(skej);
	}
	setPose(bvh, 0);
	std::cout << "SKEINIT" << std::endl;
}


Vector Skeleton::getJointPosition(int i) const
{
	return Vector(m_joints[i].m_l2w(Vector(0, 0, 0)));
}


int Skeleton::getParentId(const int i) const
{
	return m_joints[i].m_parentId;
}


void Skeleton::setPose(const BVH& bvh, int frameNumber)
{	
	for(int ii = 0; ii < numberOfJoint(); ii++)	{
		// Recuperation des joints
		SkeletonJoint s_joint = m_joints[ii];
		BVHJoint current = bvh.getJoint(ii);
		// Father transform
		Transform f2w = Identity();
		if(!current.isRoot())
		{
			f2w = m_joints.at(s_joint.m_parentId).m_l2w;
		}
		// Child transform 
		Transform l2w = f2w;
		// Offset
	    float x,y,z;
	    current.getOffset(x,y,z);
	    l2w = l2w * Translation(Vector(x,y,z));
	    // Translation / Rotation
	    for(int j = 0; j < current.getNumberOfChannel(); j++)
	    {
	        BVHChannel ch = current.getChannel(j);
	        if(ch.isRotation())
	        {
	            switch(ch.getAxis())
	            {
	                case 0: l2w = l2w * RotationX(ch.getData(frameNumber)); break;
	                case 1: l2w = l2w * RotationY(ch.getData(frameNumber)); break;
	                case 2: l2w = l2w * RotationZ(ch.getData(frameNumber)); break;
	                default: break;
	            }
	        }  
	        else
	        {
	            switch(ch.getAxis())
	            {
	                case 0: l2w = l2w * Translation(Vector(ch.getData(frameNumber),0,0)); break;
	                case 1: l2w = l2w * Translation(Vector(0,ch.getData(frameNumber),0)); break;
	                case 2: l2w = l2w * Translation(Vector(0,0,ch.getData(frameNumber))); break;
	                default: break;
	            }
	        }
	    }
	    // Child to world
	    m_joints[ii].m_l2w = l2w;
	}
}
