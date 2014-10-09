// minimal2.cpp: Display the landmarks of possibly multiple faces in an image.

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include "opencv/highgui.h"
//#include "stasm_lib.h"

#include "stasm.h"
#include "opencv/highgui.h"
#include "appmisc.h"
using namespace std;
using namespace cv;

static int cam_index=-1;
	
static void error(const char* s1, const char* s2)
{
    printf("Stasm version %s: %s %s\n", stasm_VERSION, s1, s2);
    exit(1);
}

//-----------------------------------------------------------------------------

static void GetOptions(
    int&    argc, // in
    char**& argv) // io
{

    char usage[80];
#if 0   
    =
        "stasm [FLAGS] IMAGE [IMAGE2 ...]\n"
        "\n"
        "Landmark results are saved in stasm.log\n"
        "\n"
        "Example: stasm face.jpg\n"
        "\n"
        "Flags:\n"
        "    -m     allow multiple faces per image (default is just the largest)\n"
        "    -s     detect small faces (min width 10%% of image width, the default is 25%%)\n"
        "    -n N   save as 77, 76, 68, 22, 20, or 17 point shape\n"
        "    -c     save landmarks as CSVs (default saves in shapefile format)\n"
        "    -i     do not write landmarked images (faster)\n"
        "    -d     enable debug prints\n"
        "    -?     help\n"
        "\n"
        "Stasm version %s    www.milbo.users.sonic.net/stasm\n"; // %s is stasm_VERSION
#endif

    if (argc < 2)
        stasm::Err("No image.  Use stasm -? for help.");
	cout << "ooooo" << endl;
    while (--argc > 0 && (*++argv)[0] == '-')
    {
        switch ((*argv)[1])
        {
        case 'C':
        	printf("CCCC\n");
        	break;
#if 0
        case 'c':
            csv_g = true;
            break;
        case 'd':
            trace_g = true; // trace_g defined in print.cpp
            break;
        case 'i':
            writeimgs_g = false;
            break;
        case 'm':
            multiface_g = true;
            break;
        case 's':
            minwidth_g = 10;
            break;
        case 'n':
            if (argc < 3)
                Err("-n argument must be followed by NLANDMARKS.  For example -n 68");
            argc--;
            argv++;
            nlandmarks_g = -1;
            if (1 != sscanf(*argv, "%d", &nlandmarks_g) || nlandmarks_g < 1)
                Err("-n argument must be followed by NLANDMARKS.  For example -n 68");
            // validity of nlandmarks_g will be checked later after call to ConvertShape
            break;
#endif            
        case '?':
            printf(usage, stasm_VERSION);
            exit(1);
        default:
            stasm::Err("Invalid argument -%s.  Use stasm -? for help.", *argv + 1);
            break;
        }
    }
    if (argc < 1)
        stasm::Err("No g.  Use stasm -? for help.");
}

int main(int argc, char **argv)
{
	cv::VideoCapture vc;
    char path[80] = "../data/testface.jpg";
    if (!stasm_init("../data", 0 /*trace*/))
        error("stasm_init failed: ", stasm_lasterr());
	cout << "1122112" << endl;
	GetOptions(argc,argv);
	cout << "33333" << endl;
    cv::Mat_<unsigned char> img(cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE));

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
    cv::imwrite("minimal2.bmp", img);
    cv::imshow("stasm minimal2", img);
    cv::waitKey();

    return 0;
}
