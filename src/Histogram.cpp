#include "Histogram.h"

#define DBG 1000
#define DBGY 5

Histogram::~Histogram(){
	free(hist);
	cv::destroyAllWindows();
	//image.release();

}

Histogram::Histogram(int t_tamanho,int t_passo){
	max = t_tamanho;
	acumulador = 0;
	tamanho = (int)t_tamanho/t_passo;
	passo = t_passo;
	hist = (int*) malloc(tamanho*sizeof(int));
	memset(hist,0,tamanho*sizeof(int));
	if(passo < 1 || passo > (tamanho*passo)){
		std::cout << "passo invalido" << std::endl;
		exit(1);
	}
	
}

bool Histogram::insertHist(float num){

	int aux = ((int) num)/passo;
	if(aux >= tamanho-1){
		std::cout << "entrada invalida" << std::endl;
		return false;	
	}
	float media = acumulador/tamanho;
	hist[aux]++;
	acumulador++;
	return hist[aux] > media;
}

void Histogram::removeHist(float num){

	int aux = ((int) num)/passo;
	if(aux >= tamanho-1){
		std::cout << "entrada invalida" << std::endl;
		return;	
	}
	hist[aux]--;
	acumulador--;
}

cv::Mat Histogram::debug(){
	
	cv::Mat image = cv::Mat::zeros(cv::Size(DBG,DBGY*60), CV_64FC3);
	double media = acumulador / tamanho;
	for(int i = 0; i < tamanho;i++){
		if(hist[i] < media){
			cv::rectangle(image,cv::Point((int)((i*DBG/tamanho)),(0*DBGY)+(DBGY*50)),cv::Point((int)((i+1)*DBG/tamanho),((DBGY*50)- hist[i]*DBGY)),cv::Scalar(0,0,255));
		}else{
			cv::rectangle(image,cv::Point((int)((i*DBG/tamanho)),(0*DBGY)+(DBGY*50)),cv::Point((int)((i+1)*DBG/tamanho),((DBGY*50)- hist[i]*DBGY)),cv::Scalar(0,255,0));
		}
		char nameAux[50];
		sprintf(nameAux,"%d",i*tamanho);
		//terminar de colocar o texto
		cv::putText(image,nameAux,cv::Point(i*DBG/tamanho,(DBGY*55)),cv::FONT_HERSHEY_SIMPLEX,0.3,cv::Scalar(255,255,255),1);		
	}
	return image;
	
}

