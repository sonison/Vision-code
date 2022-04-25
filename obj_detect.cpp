#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;
//내가따로 공구부터
int main()
{
	Mat img1 = imread("driver01.jpg");	//원본이미지선언
	Mat img_re;							//사이즈변경이미지
	Mat img_gray;						//흑백이미지
	Mat img_bin;						//이진화이미지

	Size img_size(500, 500);	//사용할 사진 크기

	resize(img1, img_re, img_size);		//사이즈조절
	cvtColor(img_re, img_gray, COLOR_BGR2GRAY);	//흑백으로
	threshold(img_gray, img_bin, 220, 255, THRESH_BINARY_INV);	//이진화

	//********************************************** 팽창
	Mat dst4;
	dilate(img_bin, dst4, Mat(), Point(-1, -1), 4);		//4번 팽창
	//**********************************************
	//Mat img_contour;		//외곽선이미지
	vector<vector<Point>> contours;	//외곽선정보는 vector<vector<Point>> 타입임.
	findContours(dst4, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);	//외곽선정보를 contours 에 저장
//	findContours(img_bin, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);	//외곽선정보를 contours 에 저장 @테스트
	drawContours(dst4, contours, -1, Scalar(100, 100, 100), 2);	//외곽선 그리기
	RotatedRect a = minAreaRect(contours[0]);	//280페이지 참조해서 그림 @문제해결
	cout << a.center << endl << a.size << endl << a.angle << endl;	
	//235.127 , 239.846 중심좌표 526.452 * 67.0615 크기 -41.0091 각도
	//**********************************************

	circle(dst4, a.center, 10, Scalar(100, 100, 100), 2); //중앙에 원 표시
	Point2f points[4];
	a.points(points);
	for (int i = 0;i < 4;i++)
		line(dst4, points[i], points[(i + 1) % 4], Scalar(100, 100, 100), 2);
	//line(dst4, Point(235, 239), Point(235, 500), Scalar(50, 50, 50));	//선 그리기
	imshow("bin", img_bin);		//사진 보여주기(이진화)
//	imshow("dilate4", img_re);	//4번팽창
	imshow("dilate4", dst4);	//4번팽창

	waitKey();	//키입력받기
	

	return 0;	//종료
}

/*
220407
일단 사각형까지는 했는데 다 흑백이라 그거를 해결해야한다. 새로 Matrix를 만들어서 grayscale사진정보를 복사하고 위에 추가로 그릴 중앙원
외곽선 사각형은 컬러로 위에다가 그릴 수 있도록 수정해야함
*/
