#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <cstring>

class Histogram{
	public:
		bool insertHist(float num);
		void removeHist(float num);
		Histogram(int t_tamanho, int t_passo);
		cv::Mat debug();
		~Histogram();
		
	private:
		int *hist;
		int tamanho;
		int passo;
		int max;
		long int acumulador;


};
