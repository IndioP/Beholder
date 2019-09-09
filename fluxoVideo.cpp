#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

int main(int argc, char *argv[]){


	cv::Mat firstFrameColor = cv::imread("mascara.png");
	cv::GaussianBlur(firstFrameColor, firstFrameColor, cv::Size(3,3),0);
	
	cv::Mat firstFrameGray;	
	
	cv::cvtColor(firstFrameColor, firstFrameGray, cv::COLOR_BGR2GRAY);

	cv::VideoCapture cap(argv[1]);

	if(!cap.isOpened()){
   	std::cout << "Error opening video stream" << std::endl; 
   	return -1; 
	} 

	cv::Ptr<cv::BackgroundSubtractor> pBackSub = cv::createBackgroundSubtractorMOG2(1,16,true);

	cv::VideoWriter out("videoSubtraction_gray.avi",CV_FOURCC('M','J','P','G'), 10, firstFrameColor.size());

	cv::VideoWriter out2("videoSubtraction_rgb.avi",CV_FOURCC('M','J','P','G'), 10, firstFrameColor.size());

	cv::Mat frame, fgMask;
	cap >> frame;

	//cv::GaussianBlur(frame,frame,cv::Size(3,3),0);
	
	pBackSub->apply(frame, fgMask);
	while(true){
		
		cap >> frame;

		if(frame.empty()){
			break;
		}	

		cv::Mat frame2 = frame;

		//cv::GaussianBlur(frame,frame,cv::Size(5,5),0);

		pBackSub->apply(frame, fgMask);

		
  		cv::Mat element = cv::getStructuringElement(0, cv::Size( 2*1 + 1, 2*1+1 ), cv::Point( 1, 1 ) );
		cv::morphologyEx(fgMask, fgMask, cv::MORPH_OPEN,element);

		cv::imshow("Original",frame2);
		cv::imshow("backSub",fgMask);		
		
		if(cv::waitKey(1) == 27){
			break;
		}
		cv::cvtColor(fgMask,fgMask,cv::COLOR_GRAY2BGR);
		cv::resize(fgMask,fgMask,firstFrameColor.size());
		cv::resize(frame2,frame2,firstFrameColor.size());		
		out.write(fgMask);
		out2.write(frame2);
	}

	out.release();
	out2.release();
	
	return 0;
}
