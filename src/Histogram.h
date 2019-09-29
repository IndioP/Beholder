#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <cstring>

class Histogram{
	public:
		void insertHist(int num);
		void removeHist(int num);
		Histogram(int t_tamanho, int t_passo);
		cv::Mat debug();
		~Histogram();
		
	private:
		int *hist;
		int tamanho;
		int passo;


};
