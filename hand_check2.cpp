#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include "std_msgs/Float64.h"
#include <sstream>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

using namespace cv;
using namespace std;

#define PI 3.141592

//Video Matrix
Mat frame1, frame2;

// Function
Scalar HSV_mean(Mat img, int X, int width, int Y, int height);

int main(int argc, char **argv)
{
    ros::init(argc, argv, "hand_number_publisher");
    ros::NodeHandle nh;
    ros::Publisher hand_num = nh.advertise<std_msgs::Float64>("hand_number", 10);

    ros::Rate loop_rate(50);

    VideoCapture cap(0);

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    printf("Start");

    while (ros::ok())
    {
        int cnt = 0;

        waitKey(1);
        cap >> frame1;
        if (!cap.isOpened())
        {
            cerr << "finish!\n" << endl;
        }

        frame2 = frame1.clone();
        imshow("Original", frame1);

        Mat img_resize;
        cv::resize(frame1, img_resize, cv::Size(640, 480), 0, 0);

        Mat hsv_img;
        cvtColor(frame2, hsv_img, COLOR_BGR2HSV);

        //HSV mean
        Mat hsv_img_clone = hsv_img.clone();
        rectangle(hsv_img, Rect(Point(hsv_img.cols * 3 / 5, hsv_img.rows * 3 / 5), Point(hsv_img.cols * 4 / 5, hsv_img.rows * 4 / 5)), Scalar(255, 0, 0), 10, 8, 0);
        Scalar mean_color = HSV_mean(hsv_img_clone, hsv_img_clone.rows * 3 / 5, hsv_img_clone.rows / 5, hsv_img_clone.cols * 3 / 5, hsv_img_clone.cols / 5);
        imshow("hsv_img", hsv_img);

        // img binary
        Mat img_binary;
        inRange(hsv_img_clone, mean_color - cv::Scalar(30, 30, 30), mean_color + cv::Scalar(30, 30, 30), img_binary);
        imshow("img_binary", img_binary);

        //convexityDefects

        
        // contours
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(img_binary, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        Mat circle_img;
        circle_img = Mat::zeros( img_binary.size(), CV_8UC3 );

        Scalar color(rand() & 255, rand() & 255, rand() & 255);

        vector<vector<int>> ihull(contours.size());                 // for convexityDefects
        vector<vector<Point>> phull(contours.size());              // for drawing
        vector<vector<Vec4i>> defects(contours.size());
        for( size_t i = 0; i < contours.size(); i++ )
        {
            convexHull( contours[i], ihull[i], false, false);   // for convexityDefects
            convexHull( contours[i], phull[i], false);          // for drawing

            convexityDefects(contours[i], ihull[i], defects[i]);

            for (int k = 0; k < ihull[i].size(); k++)
            {
                for (int j = 0; j < defects[i].size(); j++)
                {
                    // 제 2 코사인 법칙 적용
                    float ind_0 = defects[i][j][0];//start point
                    float ind_1 = defects[i][j][1];//end point
                    float ind_2 = defects[i][j][2];//defect point

                    Point ptStart = contours[i][ind_0];
                    Point ptEnd = contours[i][ind_1];
                    Point ptFar = contours[i][ind_2];

                    float line1 = sqrt(pow((ptStart.x - ptEnd.x), 2) + pow((ptStart.y - ptEnd.y), 2));
                    float line2 = sqrt(pow((ptStart.x - ptFar.x), 2) + pow((ptStart.y - ptFar.y), 2));
                    float line3 = sqrt(pow((ptFar.x - ptEnd.x), 2) + pow((ptFar.y - ptEnd.y), 2));

                    float angle = acos((pow(line2, 2) + pow(line3, 2) - pow(line1, 2)) / (2*line2*line3));

                    if(angle <= PI / 2)
                    {
                        cnt += 1;
                        circle(circle_img, ptFar, 10, color, 3);
                    }

                    if(cnt > 0)
                    {
                        cnt = cnt + 1;
                    }

                }
            }
        }        
        
        //drawing contours, convexes
        Mat drawing = Mat::zeros( img_binary.size(), CV_8UC3 );
        for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
        {
            drawContours( drawing, contours, idx, color, 2, LINE_8, hierarchy);
            drawContours( drawing, phull, idx, color, 2, LINE_8, hierarchy);
        }
        imshow( "Hull demo", drawing );

        Mat Final = Mat::zeros( img_resize.size(), CV_8UC3 );
        Final = img_resize + circle_img;

        string disp = "hand_num : " + std::to_string(cnt);
        putText(Final, disp, Point(0, 50), FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 0, 255), 3);

        imshow("Final", Final);

        std_msgs::Float64 msg;

        msg.data = cnt;
        ROS_INFO("%f", msg.data);
        hand_num.publish(msg);


        ros::spinOnce();
        loop_rate.sleep();
    }
    
    return 0;
}

Scalar HSV_mean(Mat img, int X, int width, int Y, int height)
{
	Mat img_roi = img(Rect(Point(Y, X), Point(Y + height, X + width)));
	// imshow("img_roi",img_roi);
	Scalar average = mean(img_roi);
	// std::cout << average << std::endl;
	return average;
}
