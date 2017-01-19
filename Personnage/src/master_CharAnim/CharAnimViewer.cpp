
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;


CharAnimViewer::CharAnimViewer() : Viewer(), m_frameNumber(0), m_bvh_file("data/bvh/motionGraph/courir.bvh")
{
}

CharAnimViewer::CharAnimViewer(std::string file): Viewer(), m_frameNumber(0), m_bvh_file(file)
{
}


int CharAnimViewer::init()
{
    Viewer::init();
    cout<<"==>master_CharAnim/CharAnimViewer"<<endl;
    m_camera.lookat( Point(0,0,0), 250 );

    init_cylinder();
    init_sphere();


    //m_bvh.init("data/bvh/Robot.bvh" );
	m_bvh.init(m_bvh_file);

    m_ske.init(m_bvh);
    // m_ske2.init(m_bvh);
    // m_ske3.init(m_bvh);

    // m_ske4.init(m_bvh);
    // m_ske5.init(m_bvh);
    // m_ske6.init(m_bvh);

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

    return 0;
}



void CharAnimViewer::bvhDrawGL(const BVH& bvh, int frameNumber)
{
    Transform trans = Identity();
    bvhDrawGLRec(bvh.getRoot(), frameNumber, trans);
}


void CharAnimViewer::bvhDrawGLRec(const BVHJoint& joint, int frameNumber, Transform& T) // la fonction récursive sur le squelette
{
    float x, y, z;
    joint.getOffset(x, y, z);
    Vector v(x, y, z);
    Transform l2f = Translation(v);
    for(int ii = 0; ii < joint.getNumberOfChannel(); ii++) {
        auto channel = joint.getChannel(ii);
        if(channel.isRotation())  {
            switch(channel.getAxis()) {
                case 0:
                    l2f = l2f * RotationX(channel.getData(frameNumber));
                    break;
                case 1:
                    l2f = l2f * RotationY(channel.getData(frameNumber));
                    break;
                case 2:
                    l2f = l2f * RotationZ(channel.getData(frameNumber));
                    break;
                default:
                    break;
            }
        } else if(channel.isTranslation()) {
            switch(channel.getAxis()) {
                case 0:
                    l2f = l2f * Translation(channel.getData(frameNumber), 0, 0);
                    break;
                case 1:
                    l2f = l2f * Translation(0, channel.getData(frameNumber), 0);
                    break;
                case 2:
                    l2f = l2f * Translation(0, 0, channel.getData(frameNumber));
                    break;
                default:
                    break;
            }
        }
    }
    Transform l2w = T * l2f;
    // Dessine Articulation = sphere + ligne du nœud courant
    // vers le père donc de l’origine de l2w à l’origine de f2w
    Vector PL = Vector(l2w(Point(0,0,0)));
    Vector PF = Vector(T(Point(0,0,0)));
    if(!joint.isRoot()) {
        draw_sphere(l2w);
        draw_cylinder(PL, PF, 0.5);
    }
    for(int i=0; i < joint.getNumberOfChildren(); i++)
        bvhDrawGLRec(joint.getChild(i), frameNumber, l2w);
}


void CharAnimViewer::skeletonDraw(const Skeleton& ske, Transform t) {
    for(int ii = 0; ii < ske.numberOfJoint(); ii++) {

        Vector pos = ske.getJointPosition(ii);
        draw_sphere(Vector(t(Point(pos))), 0.5);

        Vector pos_father(0,0,0);
        if(ii > 0) {
            pos_father = ske.getJointPosition(ske.getParentId(ii));
            draw_cylinder(Vector(t(Point(pos))), Vector(t(Point(pos_father))), 0.2);
        } 
    }
}


int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Viewer::manageCameraLight();

    gl.camera(m_camera);


	// Affiche une pose du bvh
	// bvhDrawGL(m_bvh, m_frameNumber);
    skeletonDraw(m_ske, Identity());
    // skeletonDraw(m_ske2, Translation(-75.0, 0.0, -15.0));
    // skeletonDraw(m_ske3, Translation(75.0, 0.0, -15.0));

    // skeletonDraw(m_ske4, Translation(0.0, 50.0, 0.0));
    // skeletonDraw(m_ske5, Translation(75.0, 50.0, -15.0));
    // skeletonDraw(m_ske6, Translation(-75.0, 50.0, -15.0));

    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

    if (key_state('z')) { 
        //clear_key_state('z');  
        m_frameNumber++; 
        m_frameNumber = m_frameNumber % m_bvh.getNumberOfFrame(); 
        cout << m_frameNumber << endl; 

        m_ske.setPose(m_bvh, m_frameNumber);

        // m_ske.setPoseInterpolation(m_bvh, m_frameNumber, m_bvh, (m_frameNumber+40) % m_bvh.getNumberOfFrame(), 0.5);
        // m_ske2.setPose(m_bvh, m_frameNumber);
        // m_ske3.setPose(m_bvh, (m_frameNumber+40) % m_bvh.getNumberOfFrame());
        // m_ske6.setPose(m_bvh, m_frameNumber);
        // m_ske5.setPose(m_bvh, (m_frameNumber+40) % m_bvh.getNumberOfFrame());
        // m_ske4.setPoseInterpolationQ(m_bvh, m_frameNumber,m_bvh, (m_frameNumber+40) % m_bvh.getNumberOfFrame(), 0.5);
    }   
    // m_ske.setPose(m_bvh, int(time / 75.0) % m_bvh.getNumberOfFrame());
    std::cout << "time " << time << std::endl;
    std::cout << "delta " << delta << std::endl;

    return 0;
}


