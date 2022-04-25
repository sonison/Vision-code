/* //**********************************************
220420_4 폰으로 찍은
cutter handcream mandarin phonestand toothbrush(_d,b) .jpg
파일로 직접 실행
toothbrush cutter phonestand  -45 잘됨.
mandarin 잘 안됨. -20정도가 최선.
handcream 하얀부분때문에 힘듦
*/ //**********************************************

#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

Mat paint_b_2(Mat paint_input);
Mat brighter(Mat a, int b);
int gray_mean_2(Mat a);
Mat ad_bin(Mat input, int hm);

int main()
{
	Mat img_01 = imread("mandarin.jpg");	//원본이미지
	Mat img_01_re;							//사이즈변경
	Size size(400, 400);					//사진 크기
	Mat img_01_g;							//흑백,(평균 245_?)
	Mat img_01_b;							//이진
	resize(img_01, img_01_re, size);		//사이즈조절
	cvtColor(img_01_re, img_01_g, COLOR_BGR2GRAY);	//g 만들기
	//하얀부분이 많으면 - 를 해줘야하고 검은게 많으면 + 를 해준다
	img_01_b = ad_bin(img_01_g, -20);		//g 이진화

	imshow("img_01", img_01);
	imshow("img_01_g", img_01_g);
	imshow("img_01_b", img_01_b);

	waitKey();
	return 0;	//종료
}


Mat paint_b_2(Mat paint_input)
//가로 길이에 맞게 비례해서 -1(0또는 더 큰숫자) 하는함수. cols 가로
{
	float cc = (float)255 / (float)paint_input.cols;	//가로 한칸당 scale 증가분
	cc = round(cc * 1000) / 1000;
	int count = 0;	// 가로(cols)좌표 역할
	// 가로길이만큼 반복 260까지인 이유는 반올림문제..
	for (long double i = 0;i <= 260;i += cc) {
		i = round(i * 1000000) / 1000000;
		//		printf("%.9f ", i);
		for (int j = 0;j < paint_input.rows;j++) {	// 세로길이만큼 반복
			paint_input.at<uchar>(j, count) -= (int)round(i);
			//위에서 아래로 쭉쭉 내려가면서 뺀다(하얀색에서 시작했으니깐)
		}
		//		printf("%.0f ", (round(i)));
		count++;
		if (count == paint_input.cols) break;
	}
	//	printf("\n\n%d ", count);
	imshow("paint_b_2 output", paint_input);
	return paint_input;
}

Mat brighter(Mat a, int b)
{
	Mat aa = a.clone();
//	imshow("brighter_input", a);			//받은사진원본보여주기
	for (int i = 0;i < aa.cols;i++) {		// 가로길이만큼 반복
		for (int j = 0;j < aa.rows;j++) {	// 세로길이만큼 반복
			if (aa.at<uchar>(j, i) > (255 - b))	//255보다 큰숫자가 들어가지않게
				aa.at<uchar>(j, i) = 255;		//최대를 255로 capping
			else if (aa.at<uchar>(j, i) < (0 - b)) //반대로 b가 음수일때 0보다 작아지지않게
				aa.at<uchar>(j, i) = 0;			//최소를 0로 capping
			else aa.at<uchar>(j, i) += b;
		}
	}
//	imshow("brighter_output", aa);		// 밝게한거 보여주기
	return aa;
}

int gray_mean_2(Mat a)
{
	//	imshow("gray_mean_input", a);
	int aa = 0;
	for (int i = 0;i < a.cols;i++) {		// 가로길이만큼 반복
		for (int j = 0;j < a.rows;j++) {	// 세로길이만큼 반복
			aa += a.at<uchar>(j, i);	//위에서 아래로 쭉쭉 내려가면서 더한다.t
		}
	}
	aa /= (a.cols*a.rows);
//	printf("grayscale 평균은 %d\n", aa);
	return aa;
}

Mat ad_bin(Mat input, int hm)
//전체 적응형 이진화 하기. how much
{
	Mat ad_bin_output;
	threshold(input, ad_bin_output, gray_mean_2(input) + hm, 255, THRESH_BINARY_INV);
	printf("%d+ (%d) 로 이진화", gray_mean_2(input), hm);
	return ad_bin_output;
}
