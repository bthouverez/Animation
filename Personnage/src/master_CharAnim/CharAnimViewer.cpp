
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;


CharAnimViewer::CharAnimViewer() : Viewer(), m_frameNumber(0)
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
	m_bvh.init("data/bvh/motionGraph/courir.bvh");
    m_ske.init(m_bvh);

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;


    m_angle_a=0;
    m_angle_b=40;
    m_angle_milieu_ab = (m_angle_a+m_angle_b)/2;


    m_quat_a.setAxisAngleDegree(Vector(0,0,1),90);
    Transform R;
    m_quat_a.getMatrix44(R);
    cout<<R<<endl;

    R =RotationZ(90);
    cout<<R<<endl;
    return 0;
}



void CharAnimViewer::bvhDrawGL(const BVH& bvh, int frameNumber)
{
    Transform trans = Identity();
    bvhDrawGLRec(bvh.getRoot(), frameNumber, trans);
}


void CharAnimViewer::bvhDrawGLRec(const BVHJoint& joint, int frameNumber, Transform& T) // la fonction r�cursive sur le squelette
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
    // Dessine Articulation = sphere + ligne du n�ud courant
    // vers le p�re donc de l�origine de l2w � l�origine de f2w
    Vector PL = Vector(l2w(Point(0,0,0)));
    Vector PF = Vector(T(Point(0,0,0)));
    if(!joint.isRoot()) {
        draw_sphere(l2w);
        draw_cylinder(PL, PF, 0.5);
    }
    for(int i=0; i < joint.getNumberOfChildren(); i++)
        bvhDrawGLRec(joint.getChild(i), frameNumber, l2w);
}


void CharAnimViewer::skeletonDraw(const Skeleton& ske) {
    for(int ii = 0; ii < ske.numberOfJoint(); ii++) {
        Vector pos = ske.getJointPosition(ii);
        // std::cout << ii << " - " << pos << std::endl;
        draw_sphere(pos);
    }
}


void CharAnimViewer::skeletonUpdate(Skeleton& ske) {
    ske.setPose(m_bvh, m_frameNumber);
}

int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Viewer::manageCameraLight();

    gl.camera(m_camera);


	// Affiche une pose du bvh
	bvhDrawGL(m_bvh, m_frameNumber);
    //skeletonUpdate(m_ske);
    //skeletonDraw(m_ske);


/*
	// affiche 3 cylindres dont l'angle est interpole
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_a)*Scale(0.1,2,0.1) );
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_b)*Scale(0.1,2,0.1) );
    draw_cylinder( Translation(5,0,0)*RotationZ(m_angle_milieu_ab)*Scale(0.1,2,0.1) );

	// affiche 3 cylindres dont le quaternion est interpole
    Transform R;
    m_quat_a.getMatrix44(R);
    draw_cylinder( Translation(-5,0,0)*R*Scale(0.1,2,0.1) );
    m_quat_b.getMatrix44(R);
    draw_cylinder( Translation(-5,0,0)*R*Scale(0.1,2,0.1) );
    m_quat_milieu_ab.getMatrix44(R);
    draw_cylinder( Translation(-5,0,0)*R*Scale(0.1,2,0.1) );

*/
    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

	if (key_state('z')) { /* clear_key_state('n'); */ m_frameNumber++; m_frameNumber = m_frameNumber > 21 ? 0 : m_frameNumber; cout << m_frameNumber << endl; }
	if (key_state('b')) { /* clear_key_state('b'); */ m_frameNumber--; m_frameNumber = m_frameNumber < 0 ? 21 : m_frameNumber; cout << m_frameNumber << endl; }


    m_angle_a = int(0.1*time)%360;
    m_angle_b = int(0.1*time+40)%360;
    m_angle_milieu_ab = (m_angle_a+m_angle_b)/2;

    Vector Z(0,0,1);
    m_quat_a.setAxisAngleDegree(Z,m_angle_a); //m_quat_a.invert();
    m_quat_b.setAxisAngleDegree(Z,m_angle_b); //m_quat_b.invert();
    m_quat_milieu_ab = Quaternion::slerp( m_quat_a, m_quat_b, 0.5);

    return 0;
}

