#include "Scene.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

bool needSetPara = false;       //�Ƿ���Ҫ���ó���
bool needSetBoundary = false;   //�Ƿ���Ҫ���ñ߽�
bool needSetDirection = false;  //�Ƿ���Ҫ���÷���
bool completeSetPara = false;   //�Ƿ�������ó���
bool completeSetBoundary = false;//�Ƿ�������ñ߽�
bool completeSetDirection = false;//�Ƿ�������÷���



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
	if (event == CV_EVENT_LBUTTONDBLCLK) //�����Ҫ������ͼ����ѡһ�㣬�ж����Ƿ���ĳ���߽��ڲ�����Ϊ����ʹ��
	{

	}
}

Boundary setBoundary(const Mat& img,int bdType)//���ñ߽�
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
				if (completeSetBoundary)//����������Ҽ�ʱ��ѡ�������յ㣬�����յ�����㡣//
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
/**********************ֱ�������趨�ĽǶ�*****************************************************/
Direction setDirection(const Mat& img)//���÷���
{
	double angle;
	double x,y;
	completeSetDirection = false;

	if (!completeSetDirection)
	{
		cout << "����Ƕ�:" <<endl;
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
		waitKey(0);//������
		completeSetDirection = true;
	}
	return angle;
}