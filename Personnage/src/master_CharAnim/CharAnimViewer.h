
#ifndef _CHARANIMVIEWER_H
#define _CHARANIMVIEWER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
using namespace simea;		// oui oui c'est moche, à ne pas faire dans la vraie vie
#include "Skeleton.h"
#include "TransformQ.h"
#include "CharAnimController.h"
#include "MotionGraph.h"

// Comparaison d'interpolation, mis à 1, affiche 6 squelettes dans la scène, comme présenté dans le sujet
// En haut l'interpolation simple, en bas l'interpolation Quaternion
// Executer alors le programme avec le data/bvh/danse.bvh
#define COMP_INTERP 0

class CharAnimViewer : public Viewer
{
public:
    CharAnimViewer();
    CharAnimViewer(char * bvh_start, char * bvh_directory);

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
    Skeleton m_ske4;
    Skeleton m_ske5;
    Skeleton m_ske6;

    CharAnimController m_controller;

    MotionGraph m_mg;
    bool b_mg = true;

	void bvhDrawGL(const BVH&, int frameNumber);
	void bvhDrawGLRec(const BVHJoint&, int frameNumber, Transform& T); // la fonction récursive sur le squelette

    void skeletonDraw(const Skeleton& ske, Transform t); 

};

#endif
