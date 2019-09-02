#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>


//int mat[size][size] = {{0,1,0},{1,1,1},{2,2,3}}; //matriz de exemplo

//bool mat_ref[size][size];

typedef struct{
	int Area;
	int posx;
	int posy;
	int minx;
	int miny;
	int maxx;
	int maxy;
}blob;

void verifica(int linha, int coluna, int cor, blob &b, const cv::Mat mat, cv::Mat mat_ref){
	if(linha < 0 || coluna < 0 || linha >= mat.rows || coluna >=mat.cols){ //indices fora da matriz
		//std::cerr << "out of bounds" << std::endl;
		return;	
	}
	uchar *pt_ref = mat_ref.ptr< uchar > (linha);
	const uchar * pt = mat.ptr< uchar >(linha);
	if(pt_ref[coluna]) //pixel já visitado
		return;	
		
	if(pt[coluna]){
		pt_ref[coluna] = 1; //encontrei uma região nova
												 //Pesquisando a extensão da região encontrada
		b.Area++;
		b.posx = (b.posx+linha)/2;
		b.posy = (b.posy+coluna)/2;
		if(b.minx > linha){
			b.minx = linha;		
		}
		if(b.maxx < linha){
			b.maxx = linha;		
		}
		if(b.miny > coluna){
			b.miny = coluna;		
		}
		if(b.maxy < coluna){
			b.maxy = coluna;
		}
		verifica(linha-1,coluna,cor,b,mat,mat_ref);
		verifica(linha+1,coluna,cor,b,mat,mat_ref);
		verifica(linha, coluna-1,cor,b,mat,mat_ref);
		verifica(linha,coluna+1,cor,b,mat,mat_ref);
	}
}

bool compare(blob b1, blob b2){
	return b1.Area > b2.Area;
}

int main(void){
	cv::Mat mat = cv::imread("subtraction_gray.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat matRGB = cv::imread("subtraction_rgb.png", CV_LOAD_IMAGE_COLOR);
	cv::resize(mat,mat,cv::Size(500,500));
	cv::resize(matRGB,matRGB,mat.size());

	cv::Mat mat_ref(mat.size(), CV_64FC1, cv::Scalar(0));



	cv::imshow("testado e aprovado",mat);
	int i,j;
	std::vector < blob > blobs;
	/*for(i = 0; i < mat.rows; i++) //inicializando a matriz de referencia
		for(j=0;j < size;j++)😁️
			mat_ref[i][j] = false;
	*/
	for(i = 0; i < mat.cols;i++){
		const uchar * pt = mat.ptr< uchar >(i);
		uchar *pt_ref = mat_ref.ptr< uchar > (i);
		for(j = 0; j < mat.rows; j++){		
			printf("%3d ",pt[j]);
			//std::cout << (int) pt[j] << " ";			
			if(!pt[j] || pt_ref[j]){
				 pt_ref[j] = 1;
				 continue; //pixel de cor neutra, não é referente a nenhum objeto
			}
			blob b;		
			b.Area = 0;
			b.posx = 0;
			b.posy = 0;
			b.maxx = 0;//std::numeric_limits<int>::max();
			b.maxy = 0;
			b.minx = std::numeric_limits<int>::max();
			b.miny = std::numeric_limits<int>::max();
			verifica(i,j,pt[j],b,mat,mat_ref);
			blobs.push_back(b);
		}
		std::cout << j << std::endl;
	}
	std::cout << "valor de i = " << i << "valor de j = " << j << std::endl;
	for(i = 0; i < mat.rows;i++){
		uchar *pt_ref = mat_ref.ptr< uchar > (i);
		for(j = 0; j < mat.cols;j++){
			std::cout << (int)pt_ref[j] << " ";	
		}
		std::cout << std::endl;
	}
	std::cout <<"tamanho dos blobs " << blobs.size() << std::endl;
	std::vector < blob > b = blobs;
	sort(b.begin(),b.end(),compare);
	for(blob &b : blobs){
		if(b.Area < 10) continue;
		//std::cout << b.minx << ", " << b.miny << "    " << b.maxx << ", " << b.maxy << std::endl;
		cv::rectangle(matRGB,cv::Point(b.miny,b.minx),cv::Point(b.maxy,b.maxx),cv::Scalar(255,0,0));
	}
	//cv::rectangle(matRGB, cv::Point(1,1), cv::Point(mat.cols-1,mat.rows-1),cv::Scalar(0,0,255));
	cv::imshow("testado e aprovado",matRGB);
	
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}
