#include "Histogram.h"

#define DBG 1000
#define DBGY 5

Histogram::~Histogram(){
	free(hist);
	cv::destroyAllWindows();
	//image.release();

}

Histogram::Histogram(int t_tamanho,int t_passo){
	tamanho = (int)t_tamanho/t_passo;
	passo = t_passo;
	hist = (int*) malloc(tamanho*sizeof(int));
	memset(hist,0,tamanho*sizeof(int));
	if(passo < 1 || passo > (tamanho*passo)){
		std::cout << "passo invalido" << std::endl;
		exit(1);
	}
	
}

void Histogram::insertHist(int num){
	if(num > tamanho * passo){
		std::cout << "saida invalida" << std::endl;
		return;	
	}
	int aux = num/passo;
	hist[aux]++;
}

void Histogram::removeHist(int num){
	if(num > tamanho *passo){
		std::cout << "entrada invalida" << std::endl;		
		return;	
	}
	int aux = num/passo;
	hist[aux]--;
}

cv::Mat Histogram::debug(){
	
	cv::Mat image = cv::Mat::zeros(cv::Size(DBG,DBGY*60), CV_64FC1);
	
	for(int i = 0; i < tamanho;i++){
		cv::rectangle(image,cv::Point((int)((i*DBG/tamanho)),(0*DBGY)+(DBGY*50)),cv::Point((int)((i+1)*DBG/tamanho),((DBGY*50)- hist[i]*DBGY)),cv::Scalar(255,0,255));
		char nameAux[50];
		sprintf(nameAux,"%d",1);
		//terminar de colocar o texto
		cv::putText(image,"funciona por favor",cv::Point(i,i),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,255,255),4);		
	}
	return image;
	
}

