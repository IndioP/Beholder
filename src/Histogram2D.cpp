#include "Histogram2D.h"

#define DBG 500
#define DBGY 5

Histogram2D::~Histogram2D(){
	int i;	
	for(i = 0; i < tamanhoX;i++){
		free(hist[i]);	
	}
	free(hist);
	cv::destroyAllWindows();
	//image.release();

}

Histogram2D::Histogram2D(int t_maxX,int t_passoX, int t_maxY, int t_passoY){
	acumulador = 0;
	tamanhoX = (int)t_maxX/t_passoX;
	maxX = t_maxX;
	maxY = t_maxY;
	passoX = t_passoX;
	tamanhoY = (int)t_maxY/t_passoY;
	passoY = t_passoY;

	hist = (int **) malloc(tamanhoX*sizeof(int *));
	int i;	
	for(i = 0; i < tamanhoX; i++){
		hist[i] = (int*) malloc(tamanhoY*sizeof(int));
		memset(hist[i],0,tamanhoY*sizeof(int));
	}

	
	if((passoX < 1) || (passoX > t_maxX) || (passoY < 1) || (passoY > t_maxY)){
		std::cout << "passo invalido" << std::endl;
		exit(1);
	}

}

bool Histogram2D::insertHist(int numX, int numY){
	int auxX = numX/passoX;
	int auxY = numY/passoY;
	if(auxX >= tamanhoX|| auxY >= tamanhoY){
		std::cout <<" vai dar errado " << std::endl;	
		return false;	
	}
	hist[auxX][auxY]++;
	
	acumulador++;
	double media = acumulador/(tamanhoX*tamanhoY);
	return hist[auxX][auxY] > media;
}

void Histogram2D::removeHist(int numX, int numY){
	int auxX = numX/passoX;	
	int auxY = numY/passoY;
	if(auxX >= tamanhoX|| auxY >= tamanhoY){
		std::cout <<" vai dar errado " << std::endl;
		return;		
	}
	hist[auxX][auxY]--;
	acumulador--;
}

cv::Mat Histogram2D::debug(){
	
	cv::Mat image = cv::Mat::zeros(cv::Size(DBG,DBG), CV_8UC3);
	int i, j;
	double media = acumulador / (tamanhoX*tamanhoY);
	for(i = 0; i < tamanhoX; i++){
		for(j = 0; j < tamanhoY; j++){
			if(hist[i][j] < media){
				cv::rectangle(image, cv::Point((int)(i*DBG/tamanhoX),j*DBG/tamanhoY),cv::Point((int)((i+1)*DBG/tamanhoX),(j+1)*DBG/tamanhoY),cv::Scalar(0,0,255),CV_FILLED);
			}else{
				cv::rectangle(image, cv::Point((int)(i*DBG/tamanhoX),j*DBG/tamanhoY),cv::Point((int)((i+1)*DBG/tamanhoX),(j+1)*DBG/tamanhoY),cv::Scalar(0,255,0),CV_FILLED);
			}
			//cv::rectangle(image, cv::Point((int)(i*DBG/tamanhoX),j*DBG/tamanhoY),cv::Point((int)((i+1)*DBG/tamanhoX),(j+1)*DBG/tamanhoY),cv::Scalar(hist[i][j]*2),CV_FILLED); //cuidado com esse parametro do Scalar
		}
	}
	//cv::applyColorMap(image, image, cv::COLORMAP_JET);
	/*
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
	}*/
	return image;
	
}

