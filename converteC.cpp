#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstring>
#include <cstdio>

using namespace cv;
using namespace std;

void convertImage(cv::Mat image, float sample[]){
	if(image.rows < image.cols){
	 	 cv::resize(image,image,cv::Size((int)(256*image.cols/image.rows),256));
	}else{
	 	 cv::resize(image,image,cv::Size(256,(int)256*image.rows/image.cols));
   }
   const int cropSize = 224;
   const int offsetW = (image.cols - cropSize) / 2;
   const int offsetH = (image.rows - cropSize) / 2;
   const Rect roi(offsetW, offsetH, cropSize, cropSize);

   image = image(roi).clone();
	cv::cvtColor(image,image,cv::COLOR_BGR2RGB);
	int countIndexSample = 0;

	for(int j = 0;j < image.rows;j++){
		for(int i = 0;i < image.cols;i++){
	    	sample[countIndexSample] = ((((float)image.at<Vec3b>(j,i)[0]/255) - 0.485)/0.229);
			std::cout << ((((float)image.at<Vec3b>(j,i)[0]/255) - 0.485)/0.229) << " ";
			countIndexSample++;
	 	} 
	}
	
	for(int j = 0;j < image.rows;j++){
	 	for(int i = 0;i < image.cols;i++){
	      sample[countIndexSample] = ((((float)image.at<Vec3b>(j,i)[1]/255) - 0.456)/0.224); 
			std::cout << ((((float)image.at<Vec3b>(j,i)[1]/255) - 0.456)/0.224) << " ";
			countIndexSample++;
	 	}
	}
	
	for(int j = 0;j < image.rows;j++){
	 	for(int i = 0;i < image.cols;i++){
	     	sample[countIndexSample] = ((((float)image.at<Vec3b>(j,i)[2]/255) - 0.406)/0.225);
			std::cout << ((((float)image.at<Vec3b>(j,i)[2]/255) - 0.406)/0.225) << " ";
			countIndexSample++;
	 	}
	}


}

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


	float sample[224*224*3];
	convertImage(image,sample);
	
	imshow( "Display window", image );
	waitKey(0);
	return 0;
}
