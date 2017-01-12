
#ifndef _CHARANIMVIEWER_H
#define _CHARANIMVIEWER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
using namespace simea;		// oui oui c'est moche, à ne pas faire dans la vraie vie
#include "Skeleton.h"
#include "TransformQ.h"


class CharAnimViewer : public Viewer
{
public:
    CharAnimViewer();
    CharAnimViewer(std::string bvh_filename);

    int init();
    int render();
    int update( const float time, const float delta );


protected:
    BVH m_bvh;
    int m_frameNumber;
    std::string m_bvh_file;

    Skeleton m_ske;
    Skeleton m_ske2;
    Skeleton m_ske3;


	void bvhDrawGL(const BVH&, int frameNumber);
	void bvhDrawGLRec(const BVHJoint&, int frameNumber, Transform& T); // la fonction récursive sur le squelette

    void skeletonDraw(const Skeleton& ske, Transform t); 

	// les 6 variables qui suivent sont pour illustrer comment marche les quaternions, vous pouvez les supprimer rapdidement :
    float m_angle_a;
    float m_angle_b;
    float m_angle_milieu_ab;
    Quaternion m_quat_a;
    Quaternion m_quat_b;
    Quaternion m_quat_milieu_ab;
};



#endif
