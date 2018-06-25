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

//定义有矩形、圆、单条直线、多条直线、多边形5种边界类型
#define BOUNDARY_UNKNOWN            0
#define BOUNDRY_RECT                1
#define BOUNDRY_CICLE               2
#define BOUNDRY_SINGLE_LINE         3
#define BOUNDRY_MULTIPLE_LINE       4
#define BOUNDRY_POLYGON             5

/************************************************************************/
/**结构体 Boundary：
flag:
        BOUNDRY_RECT:point_boundray中依次存放的是矩形的四个顶点。
		BOUNDRY_CICLE：point_boundray依次存放的是圆心坐标（center.x,center.y）
		              和（center.x+R，center.y）,其中R为圆的半径
		BOUNDRY_SINGLE_LINE：point_boundray中依次存放直线的起点和终点
		BOUNDRY_MULTIPLE_LINE：point_boundray中依次存放直线1的起点、终点，直线2
		              的起点、终点。
		BOUNDRY_POLYGON：point_boundray中依次存放的是多边形的各个顶点。*/
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


Boundary setBoundary(const Mat& img,int bdType);   //设置边界
Direction setDirection(const Mat& img); //设置方向
int setDetectionType();         //设置检测类型
int setSceneNum();              //设置场景序号


#endif