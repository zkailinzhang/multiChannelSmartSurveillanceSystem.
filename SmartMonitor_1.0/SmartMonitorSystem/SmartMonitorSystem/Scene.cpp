#include "Scene.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

bool needSetPara = false;       //是否需要设置场景
bool needSetBoundary = false;   //是否需要设置边界
bool needSetDirection = false;  //是否需要设置方向
bool completeSetPara = false;   //是否完成设置场景
bool completeSetBoundary = false;//是否完成设置边界
bool completeSetDirection = false;//是否完成设置方向



static void onMouse(int event,int x,int y,int flags,void* p)
{
	completeSetBoundary = false;
	Boundary *temp_boundary = (Boundary*)p;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		temp_boundary->point_boundray.push_back(Point(x,y));
	}
	if (event == CV_EVENT_RBUTTONDOWN)
	{
		temp_boundary->point_boundray.push_back(Point(x,y));
		completeSetBoundary = true;
	}
	if (event == CV_EVENT_LBUTTONDBLCLK) //这句主要用来在图面上选一点，判断其是否处在某个边界内部，仅为测试使用
	{

	}
}

Boundary setBoundary(const Mat& img,int bdType)//设置边界
{
	Boundary boundary_draw;
	Mat temp_img;
	resize(img,temp_img,Size(768,576));
	string winname = "img";
	imshow(winname,temp_img);
	waitKey(20);
	boundary_draw.flag = bdType;

	needSetBoundary = true;
	Boundary* p_boundary = &boundary_draw;
	setMouseCallback(winname,&onMouse,p_boundary);
	while (needSetBoundary)
	{
		int temp = boundary_draw.point_boundray.size();
		switch (boundary_draw.flag)
		{
		case BOUNDRY_RECT:
			break;
		case BOUNDRY_POLYGON:
			if (temp>1)
			{
				for (int i=1;i<temp;i++)
				{
					line(temp_img,
						boundary_draw.point_boundray[i-1],
						boundary_draw.point_boundray[i],
						Scalar(0,0,255),3);
				}
				if (completeSetBoundary)//当单击鼠标右键时，选定轮廓终点，连接终点与起点。//
				{
					line(temp_img,boundary_draw.point_boundray[temp-1],boundary_draw.point_boundray[0],Scalar(0,0,255),3);
					needSetBoundary = false;
				}
			}
			break;
		case BOUNDRY_SINGLE_LINE:
			if (boundary_draw.point_boundray.size()>1 && completeSetBoundary)
			{
				line(temp_img,boundary_draw.point_boundray[0],boundary_draw.point_boundray[1],Scalar(0,255,255));
				completeSetBoundary = false;
			}
			break;
		default:
			break;
		}
		imshow(winname,temp_img);
		waitKey(20);
	}
	destroyWindow(winname);
	return boundary_draw;
}

/*********************************************************************************************/
/**********************直接输入设定的角度*****************************************************/
Direction setDirection(const Mat& img)//设置方向
{
	double angle;
	double x,y;
	completeSetDirection = false;

	if (!completeSetDirection)
	{
		cout << "输入角度:" <<endl;
		cin >> angle;
		cout << endl;

		Mat temp_img = img.clone();
		x = 50*cos(angle*3.1415926/180);
		y = 50*sin(angle*3.1415926/180);

		circle(temp_img,
			Point(temp_img.cols/2,temp_img.rows/2),
			50,
			Scalar(0,255,0));

		line(temp_img,
			Point(temp_img.cols/2,temp_img.rows/2),
			Point(temp_img.cols/2+x,temp_img.rows/2-y),
			Scalar(0,0,255),
			2);
		imshow("temp",temp_img);
		waitKey(0);//测试用
		completeSetDirection = true;
	}
	return angle;
}