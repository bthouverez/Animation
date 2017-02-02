
#include "Skeleton.h"

using namespace simea;

void Skeleton::init(const BVH& bvh)
{
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
    	Transform Trans;
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
	        BVHChannel channelSrc = jointSrc.getChannel(jj);
			BVHJoint jointDst = bvhDst.getJoint(ii);
	        BVHChannel channelDst = jointDst.getChannel(jj);
	        if(channelSrc.isRotation())
	        {
	            switch(channelSrc.getAxis())
	            {
	                case AXIS_X: 
	                	Trans = RotationX( channelSrc.getData(frameNbSrc) * (1 - t) + t * channelDst.getData(frameNbDst));
	                	break;
	                case AXIS_Y: 
	                	Trans = RotationY( channelSrc.getData(frameNbSrc) * (1 - t) + t * channelDst.getData(frameNbDst));
	                	break;
	                case AXIS_Z: 
	                	Trans = RotationZ( channelSrc.getData(frameNbSrc) * (1 - t) + t * channelDst.getData(frameNbDst));
	                	break;
	                default: 
	                	break;
	            }
	        }  
	        else
	        {
	            switch(channelSrc.getAxis())
	            {
	                case AXIS_X: 
	                	Trans = Translation( Vector(channelSrc.getData(frameNbSrc), 0, 0) * t + (1-t) * Vector(channelDst.getData(frameNbDst), 0, 0));
	                	break;
	                case AXIS_Y: 
	                	Trans = Translation( Vector(0, channelSrc.getData(frameNbSrc), 0) * t + (1-t) * Vector(0, channelDst.getData(frameNbDst), 0));
	                	break;
	                case AXIS_Z: 
	                	Trans = Translation( Vector(0, 0, channelSrc.getData(frameNbSrc)) * t + (1-t) * Vector(0, 0, channelDst.getData(frameNbDst)));
	                	break;
	                default: 
	                	break;
	            }
	        }
    		l2w = l2w  * Trans;
	    }
		m_joints[ii].m_l2w = l2w;
	}    
}


void Skeleton::setPoseInterpolationQ(const simea::BVH& bvhSrc, int frameNbSrc, const simea::BVH& bvhDst, int frameNbDst, float t) {
	for(int ii = 0; ii < numberOfJoint(); ii++)	{
    	TransformQ TransSrc, TransDst;
	
        float x_s, y_s, z_s, x_d, y_d, z_d;

        BVHJoint jointSrc = bvhSrc.getJoint(ii);
        jointSrc.getOffset(x_s, y_s, z_s);

        BVHJoint jointDst = bvhDst.getJoint(ii);
        jointDst.getOffset(x_d, y_d, z_d);

        TransSrc = TransformQ(Vector(x_s, y_s, z_s));
        TransDst = TransformQ(Vector(x_d, y_d, z_d));

	    // Translation / Rotation
	    for(int jj = 0; jj < jointSrc.getNumberOfChannel(); jj++)
	    {
	        BVHChannel channelSrc = jointSrc.getChannel(jj);
	        float dataSrc = channelSrc.getData(frameNbSrc);
			BVHJoint jointDst = bvhDst.getJoint(ii);
	        BVHChannel channelDst = jointDst.getChannel(jj);
	        float dataDst = channelSrc.getData(frameNbDst);
	        if(channelSrc.isRotation())
	        {
	            switch(channelSrc.getAxis())
	            {
	                case AXIS_X: 
	                	TransSrc = TransSrc * TransformQ(Vector(1.f, 0.f, 0.f), dataSrc);
	                	TransDst = TransDst * TransformQ(Vector(1.f, 0.f, 0.f), dataDst);
	                	break;
	                case AXIS_Y: 
	                	TransSrc = TransSrc * TransformQ(Vector(0.f, 1.f, 0.f), dataSrc);
	                	TransDst = TransDst * TransformQ(Vector(0.f, 1.f, 0.f), dataDst);
	                	break;
	                case AXIS_Z: 
	                	TransSrc = TransSrc * TransformQ(Vector(0.f, 0.f, 1.f), dataSrc);
	                	TransDst = TransDst * TransformQ(Vector(0.f, 0.f, 1.f), dataDst);
	                	break;
	                default: 
	                	break;
	            }
	        }  
	        else
	        {
	            switch(channelSrc.getAxis())
	            {
	                case AXIS_X: 
	                	TransSrc = TransSrc * TransformQ(Vector(dataSrc, 0.f, 0.f));
	                	TransDst = TransDst * TransformQ(Vector(dataDst, 0.f, 0.f));
	                	break;
	                case AXIS_Y: 
	                	TransSrc = TransSrc * TransformQ(Vector(0.f, dataSrc, 0.f));
	                	TransDst = TransDst * TransformQ(Vector(0.f, dataDst, 0.f));
	                	break;
	                case AXIS_Z: 
	                	TransSrc = TransSrc * TransformQ(Vector(0.f, 0.f, dataSrc));
	                	TransDst = TransDst * TransformQ(Vector(0.f, 0.f, dataDst));
	                	break;
	                default: 
	                	break;
	            }
	        }
	    }


	    TransformQ resQ = TransformQ::slerp(TransSrc, TransDst, t);
		Transform res;
        resQ.getMatrix44<Transform>(res);

        Transform l2w = Identity();
        if(m_joints[ii].m_parentId != -1) {
            l2w = m_joints[m_joints[ii].m_parentId].m_l2w;
        }

        m_joints[ii].m_l2w = l2w * res;
	}    
}

float distance(const Skeleton& sk1, const Skeleton& sk2) {
	if(sk1.numberOfJoint() != sk2.numberOfJoint())	{
		return -1;
	}

	float res = 0.f;
	// Pour chaque joint du bvh
	for(int ii = 1; ii < sk1.numberOfJoint(); ii++) {
		// Calcule la distance entre les deux joints
		res += distance(Point(sk1.getJointPosition(ii)), Point(sk2.getJointPosition(ii)));
	}
	return res;
}