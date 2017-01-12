
#include "Skeleton.h"

using namespace simea;

void Skeleton::init(const BVH& bvh)
{
	std::cout << "Init " << Identity() << std::endl;
	for (int ii = 0; ii < bvh.getNumberOfJoint(); ii++) {
		BVHJoint joint = bvh.getJoint(ii);
		SkeletonJoint skej;
		skej.m_parentId = joint.getParentId();
		skej.m_l2w = Identity();
		m_joints.push_back(skej);
	}
	setPose(bvh, 0);
}

void Skeleton::init(const BVH& bvh, Transform tr)
{
	std::cout << "Init " << tr << std::endl;
	for (int ii = 0; ii < bvh.getNumberOfJoint(); ii++) {
		BVHJoint joint = bvh.getJoint(ii);
		SkeletonJoint skej;
		skej.m_parentId = joint.getParentId();
		skej.m_l2w = tr;
		m_joints.push_back(skej);
	}
	setPose(bvh, 0);
}


Vector Skeleton::getJointPosition(int i) const
{
	return Vector(m_joints[i].m_l2w(Point(0, 0, 0)));
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
		if(!current.isRoot()) {
			f2w = m_joints.at(s_joint.m_parentId).m_l2w;
		}
		// Child transform 
		Transform l2w = f2w;
		// Offset
	    float x,y,z;
	    current.getOffset(x,y,z);
	    l2w = l2w * Translation(Vector(x,y,z));
	    // Translation / Rotation
	    for(int jj = 0; jj < current.getNumberOfChannel(); jj++)
	    {
	        BVHChannel ch = current.getChannel(jj);
	        if(ch.isRotation())
	        {
	            switch(ch.getAxis())
	            {
	                case 0: 
	                	l2w = l2w * RotationX(ch.getData(frameNumber)); 
	                	break;
	                case 1: 
	                	l2w = l2w * RotationY(ch.getData(frameNumber));
	                	break;
	                case 2: 
	                	l2w = l2w * RotationZ(ch.getData(frameNumber));
	                	break;
	                default: 
	                	break;
	            }
	        }  
	        else
	        {
	            switch(ch.getAxis())
	            {
	                case 0: 
	                	l2w = l2w * Translation(Vector(ch.getData(frameNumber),0,0));
	                	break;
	                case 1: 
	                	l2w = l2w * Translation(Vector(0,ch.getData(frameNumber),0));
	                	break;
	                case 2: 
	                	l2w = l2w * Translation(Vector(0,0,ch.getData(frameNumber)));
	                	break;
	                default: 
	                	break;
	            }
	        }
	    }
	    // Child to world
	    m_joints[ii].m_l2w = l2w;
	}
}

void Skeleton::setPoseInterpolation(const BVH& bvhSrc, int frameNbSrc, const BVH& bvhDst, int frameNbDst, float t) {	
	for(int ii = 0; ii < numberOfJoint(); ii++)	{
		// Recuperation des joints
		SkeletonJoint s_joint = m_joints[ii];
		BVHJoint jointSrc = bvhSrc.getJoint(ii);
		// Father transform
		Transform f2w = Identity();
		if(!jointSrc.isRoot()) {
			f2w = m_joints.at(s_joint.m_parentId).m_l2w;
		}
		// Child transform 
		Transform l2w = f2w;
		// Offset
	    float x,y,z;
	    jointSrc.getOffset(x,y,z);
	    l2w = l2w * Translation(Vector(x,y,z));
	    // Translation / Rotation
	    for(int jj = 0; jj < jointSrc.getNumberOfChannel(); jj++)
	    {
	    	Transform xRotSrc, xRotDst, xInterpRot, yRotSrc, yRotDst, yInterpRot, zRotSrc, zRotDst, zInterpRot;
	        BVHChannel channelSrc = jointSrc.getChannel(jj);
			BVHJoint jointDst = bvhDst.getJoint(ii);
	        BVHChannel channelDst = jointDst.getChannel(jj);
	        if(channelSrc.isRotation())
	        {
	            switch(channelSrc.getAxis())
	            {
	                case 0: 
	                	xRotSrc = RotationX(channelSrc.getData(frameNbSrc)) * (1 - t);
	                	xRotDst = RotationX(channelDst.getData(frameNbDst)) * t;
	                	xInterpRot = xRotSrc * xRotDst;
	                	l2w = l2w * xInterpRot;
	                	break;
	                case 1: 
	                	yRotSrc = RotationY(channelSrc.getData(frameNbSrc)) * (1 - t);
	                	yRotDst = RotationY(channelDst.getData(frameNbDst)) * t;
	                	yInterpRot = yRotSrc * yRotDst;
	                	l2w = l2w * yInterpRot;
	                	break;
	                case 2: 
	                	zRotSrc = RotationZ(channelSrc.getData(frameNbSrc)) * (1 - t);
	                	zRotDst = RotationZ(channelDst.getData(frameNbDst)) * t;
	                	zInterpRot = zRotSrc * zRotDst;
	                	l2w = l2w * zInterpRot;
	                	break;
	                default: 
	                	break;
	            }
	        }  
	        else
	        {
	            switch(channelSrc.getAxis())
	            {
	                case 0: 
	                	l2w = l2w * Translation(Vector(channelSrc.getData(frameNbSrc),0,0));
	                	break;
	                case 1: 
	                	l2w = l2w * Translation(Vector(0,channelSrc.getData(frameNbSrc),0));
	                	break;
	                case 2: 
	                	l2w = l2w * Translation(Vector(0,0,channelSrc.getData(frameNbSrc)));
	                	break;
	                default: 
	                	break;
	            }
	        }
	    }
	    // Child to world
	    m_joints[ii].m_l2w = l2w;
	}    
}
