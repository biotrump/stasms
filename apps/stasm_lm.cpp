// minimal2.cpp: Display the landmarks of possibly multiple faces in an image.

#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>

#include "stasm.h"
#include "stasm_lib.h"
#include "appmisc.h"

using namespace std;
using namespace cv;

static int cam_idx;
static cv::String MyWin_Name = "stasm_lm";

static void error(const char* s1, const char* s2)
{
    printf("Stasm version %s: %s %s\n", stasm_VERSION, s1, s2);
    exit(1);
}

int option(int argc, char **argv)
{
	printf("argc=%d\n",argc);
	for(int i=1;(i< argc) && (argv[i][0]=='-') ; i++){
		switch(argv[i][1]){
		case 'C':
			sscanf(argv[i]+2,"%d",&cam_idx);
			printf("%d=%s\n", cam_idx, argv[i]+2);
			break;
		case 'F':
			break;
		default:
			break;
		}
	}
	return cam_idx;
}

int main(int argc,char **argv)
{
	cv::VideoCapture vc;
    char path[] = "../data/testface.jpg";
    if (!stasm_init("../data", 0 /*trace*/))
        error("stasm_init failed: ", stasm_lasterr());

	option(argc, argv);
	if(!vc.open(cam_idx)){
		std::cout << " open cam device index:"<< cam_idx <<"failed." <<std::endl;
		exit(-1);
	}

	if(!vc.isOpened()){
		cout << "cam is not open!" << endl;
		exit(-1);
	}
		
	namedWindow(MyWin_Name);
	for(;;){
		cv::Mat colorMat, greyMat;
		vc >> colorMat; 	//get one frame
	  	if(!colorMat.empty() ){
			cv::cvtColor(colorMat, greyMat, CV_BGR2GRAY);
	//		cv::Mat_<unsigned char> img(cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE));
			cv::Mat_<unsigned char> img(greyMat);
			if (!img.data)
				error("Cannot load", path);

			if (!stasm_open_image((const char*)img.data, img.cols, img.rows, path,
					              1 /*multiface*/, 10 /*minwidth*/))
				error("stasm_open_image failed: ", stasm_lasterr());

			int foundface;
			float landmarks[2 * stasm_NLANDMARKS]; // x,y coords (note the 2)

			int nfaces = 0;
			while (1)
			{
				if (!stasm_search_auto(&foundface, landmarks))
					 error("stasm_search_auto failed: ", stasm_lasterr());

				if (!foundface)
					break;      // note break

				// for demonstration, convert from Stasm 77 points to XM2VTS 68 points
				stasm_convert_shape(landmarks, 68);

				// draw the landmarks on the image as white dots
				stasm_force_points_into_image(landmarks, img.cols, img.rows);
				for (int i = 0; i < stasm_NLANDMARKS; i++)
					img(cvRound(landmarks[i*2+1]), cvRound(landmarks[i*2])) = 255;

				nfaces++;
			}
			printf("%s: %d face(s)\n", path, nfaces);
			fflush(stdout);
	
			//cv::imwrite("minimal2.bmp", img);
			cv::imshow("stasm minimal2", img);
		  	int c = waitKey(5);
		  	if( c == 27 || c == 'q' ) break;
		}
	}
	
    return 0;
}
