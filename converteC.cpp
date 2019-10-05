#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    string imageName("cat.jpg"); // by default
    if (argc > 1) {
        imageName = argv[1];
    }

    Mat image;
    image = imread(imageName.c_str(), IMREAD_COLOR);

    if (image.empty()) {
      cout <<  "Could not open or find the image" << std::endl ;
      return -1;
    }
    //cout << "Initial image dimension: " << image.cols << " X " << image.rows << endl;

    namedWindow( "Display window", WINDOW_AUTOSIZE );
    imshow( "Display window", image );
    waitKey(0);
	 if(image.rows < image.cols){
	 	 cv::resize(image,image,cv::Size((int)(256*image.cols/image.rows),256));
	 }else{
	 	 cv::resize(image,image,cv::Size(256,(int)256*image.rows/image.cols));
    }
    const int cropSize = 224;
    const int offsetW = (image.cols - cropSize) / 2;
    const int offsetH = (image.rows - cropSize) / 2;
    const Rect roi(offsetW, offsetH, cropSize, cropSize);
    imshow( "Display window", image );
    waitKey(0);
    image = image(roi).clone();
	 cv::cvtColor(image,image,cv::COLOR_BGR2RGB);
    //cout << "Cropped image dimension: " << image.size() << endl;
	 //unsigned char *input = image.ptr<unsigned char>();

	 std::cout << "float sample[] = {";
	 for(int j = 0;j < image.rows;j++){
	 	 for(int i = 0;i < image.cols;i++){
	     	 
			 std::cout << ((((float)image.at<Vec3b>(j,i)[0]/255) - 0.485)/0.229) << ", ";

	 	 }
	 }
	
	 for(int j = 0;j < image.rows;j++){
	 	 for(int i = 0;i < image.cols;i++){
	     	 
			 std::cout << ((((float)image.at<Vec3b>(j,i)[1]/255) - 0.456)/0.224) << ", ";

	 	 }
	 }
	
	 for(int j = 0;j < image.rows;j++){
	 	 for(int i = 0;i < image.cols;i++){
	     	 
			 std::cout << ((((float)image.at<Vec3b>(j,i)[2]/255) - 0.406)/0.225) << ", ";

	 	 }
	 }

	 std::cout << "};";

    imshow( "Display window", image );
    waitKey(0);
    return 0;
}
