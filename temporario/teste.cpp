#include <iostream>
#include <vector>
#include <deque>
//#include <opencv2/opencv.hpp>
#include "Histogram2D.h"

#define MAX_ITERATIONS 5000


int main(void){

	Histogram2D h(20,2,10,5);

	std::deque < int > fila, fila2;
	srand(10);
	std::cout << "aparentemente deu certo" << std::endl;
	
	while(true){
		
		int aux = rand()%20;
		int aux2 = rand()%10;
		std::cout <<"executou " << std::endl;
		h.insertHist(aux,aux2);
		//h.insertHist(3,3);
		fila.push_back(aux);
		fila2.push_back(aux2);
		if(fila.size() > MAX_ITERATIONS){
			std::cout <<"chegou aqui?"<<std::endl;
			int x = fila.front();
			int y = fila2.front();
			h.removeHist(x,y);
			fila.pop_front();
			fila2.pop_front();		
		}
		
		cv::imshow("debugando",h.debug());
		
		int k = cv::waitKey(10);
		if(k == 27){
			std::cout << "encerrando o programa" << std::endl;
			break;
		}
		
	}

	return 0;
}
