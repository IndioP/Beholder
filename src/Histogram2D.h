#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cstdio>
#include <climits>

class Histogram2D{
	public:
		bool insertHist(int numx, int numY);
		void removeHist(int numX, int numY);
		Histogram2D(int t_maxX, int t_passoX,int t_maxY, int t_passoY);
		cv::Mat debug();
		~Histogram2D();
		
	private:
		int **hist;
		int tamanhoX;
		int maxX;
		int maxY;
		int passoX;
		int tamanhoY;
		int passoY;
		long int acumulador;
		
		bool getSurroundings(int i, int j, double media);


};
