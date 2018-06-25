#ifndef _SCENE_H_
#define _SCENE_H_

#include <opencv2/core/core.hpp>

#include <math.h>
#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

typedef double Direction; 

//�����о��Ρ�Բ������ֱ�ߡ�����ֱ�ߡ������5�ֱ߽�����
#define BOUNDARY_UNKNOWN            0
#define BOUNDRY_RECT                1
#define BOUNDRY_CICLE               2
#define BOUNDRY_SINGLE_LINE         3
#define BOUNDRY_MULTIPLE_LINE       4
#define BOUNDRY_POLYGON             5

/************************************************************************/
/**�ṹ�� Boundary��
flag:
        BOUNDRY_RECT:point_boundray�����δ�ŵ��Ǿ��ε��ĸ����㡣
		BOUNDRY_CICLE��point_boundray���δ�ŵ���Բ�����꣨center.x,center.y��
		              �ͣ�center.x+R��center.y��,����RΪԲ�İ뾶
		BOUNDRY_SINGLE_LINE��point_boundray�����δ��ֱ�ߵ������յ�
		BOUNDRY_MULTIPLE_LINE��point_boundray�����δ��ֱ��1����㡢�յ㣬ֱ��2
		              ����㡢�յ㡣
		BOUNDRY_POLYGON��point_boundray�����δ�ŵ��Ƕ���εĸ������㡣*/
/************************************************************************/
struct Boundary
{
	int flag;
	vector<Point> point_boundray;
};

struct ScenePara
{
	int id;
	int detectionType;
	Boundary bd;
	Direction direction;
};


Boundary setBoundary(const Mat& img,int bdType);   //���ñ߽�
Direction setDirection(const Mat& img); //���÷���
int setDetectionType();         //���ü������
int setSceneNum();              //���ó������


#endif