#include "Histogram.h"

#define DBG 1
#define DBGY 10

Histogram::~Histogram(){
	free(hist);
	cv::destroyAllWindows();

}

Histogram::Histogram(int t_tamanho,int t_passo){
	tamanho = t_tamanho;
	passo = t_passo;
	hist = (int*) malloc(tamanho*sizeof(int));
	memset(hist,0,tamanho*sizeof(int));
	if(passo < 1 || passo > tamanho){
		std::cout << "passo invalido" << std::endl;
		exit(1);
	}
}

void Histogram::insertHist(int num){
	int aux = num/passo;
	hist[aux]++;
}

void Histogram::removeHist(int num){
	int aux = num/passo;
	hist[aux]--;
}

cv::Mat Histogram::debug(){
	cv::Mat image = cv::Mat::zeros(cv::Size(tamanho,720), CV_64FC1);
	for(int i = 0; i < tamanho;i++){
		cv::rectangle(image,cv::Point((int)(i*tamanho/passo),0*DBGY),cv::Point((int)((i+1)*tamanho/passo),hist[i]*DBGY),cv::Scalar(255,0,255));		
	}
	return image;
	
}

