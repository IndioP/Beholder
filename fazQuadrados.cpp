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

typedef struct{
     int start;
     int width;
     int areaBlob;
     int parent;
     int parentRow;
     int sumX;
     int sumY;
     uchar color;
}Run;

std::vector< std::vector<Run> > run(const cv::Mat &matrix){
    std::vector< std::vector<Run> > runs;
   

    int runLenght;
    for (int i=0; i < matrix.rows; i++) {

        const uchar * pt = matrix.ptr<uchar>(i);
        std::vector<Run> line;

        for (int j=0; j < matrix.cols; j++) {

            for (runLenght = 0; runLenght + j < matrix.cols && pt[j + runLenght]!=0; runLenght++);

            Run current;
            current.start = j;
            current.color = pt[j];
            current.width = runLenght;
            current.areaBlob = current.width;
            current.parent = -1;
            current.sumX = i * current.width;
            current.sumY = current.width * ((current.width/2) + current.start);

            if (current.color != 0) line.push_back(current);
            j+=runLenght;
        }

        runs.push_back(line);
    }
    return runs;

}

void findBlobs(std::vector< std::vector<Run> > &runs, cv::Mat &debugFrame){
    uchar cor;
    int diff;

    Run r1, r2;
    unsigned long i1, i2; //indexes for first and second row

    // union find
    for(unsigned long row = 1 ; row < runs.size() ; row++){
        i1=0, i2=0;

        while(i2 < runs[row].size() && i1 < runs[row-1].size()) {
            r1 = runs[row-1][i1], r2 = runs[row][i2];

            if (( (r2.start <= r1.start && r1.start < r2.start+r2.width)
                      || (r1.start <= r2.start && r2.start < r1.start+r1.width) ) ) {

                if (r1.parent == -1) {
                    r1.parent = i2;
                    r1.parentRow = row;

                    r2.areaBlob += r1.areaBlob;
                    r2.sumX += r1.sumX;
                    r2.sumY += r1.sumY;
                }
            }

            runs[row-1][i1] = r1;
            runs[row][i2] = r2;

            diff = (r2.start + r2.width) - (r1.start + r1.width);

            if(diff >= 0) i1++;
            if(diff <= 0) i2++;

        }

    }

    // turning fathers into blobs
    blob varBlob;
    Run r;
    int countBlobs =0;
    for(unsigned long i = 0 ; i < runs.size() ; i++){

        for(unsigned long j  = 0 ; j < runs[i].size() ; j++){
            r = runs[i][j];

            if(r.parent == -1) {
                cor = runs[i][j].color;
                varBlob.posx = r.sumY / r.areaBlob;
                varBlob.posy = r.sumX / runs[i][j].areaBlob;

                countBlobs++;
                cv::circle(debugFrame,cv::Point(varBlob.posx,varBlob.posy),5,cv::Scalar(255,0,0),1, CV_AA);
            }

        }
    }
    
    runs.clear();
}


int main(void){
	cv::Mat mat = cv::imread("subtraction_gray.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat matRGB = cv::imread("subtraction_rgb.png", CV_LOAD_IMAGE_COLOR);
	cv::Mat mat_ref(mat.size(), CV_64FC1, cv::Scalar(0));


	/*😁️

	*/
	
	std::vector< std::vector<Run> > R = run(mat);
	findBlobs(R,matRGB);
  /*
	sort(b.begin(),b.end(),compare);
	for(blob &b : blobs){
		if(b.Area < 10) continue;
		//std::cout << b.minx << ", " << b.miny << "    " << b.maxx << ", " << b.maxy << std::endl;
		cv::rectangle(matRGB,cv::Point(b.miny,b.minx),cv::Point(b.maxy,b.maxx),cv::Scalar(0,0,255));
	}*/
	//cv::rectangle(matRGB, cv::Point(1,1), cv::Point(mat.cols-1,mat.rows-1),cv::Scalar(0,0,255));
	cv::imshow("testado e aprovado",matRGB);
	cv::imshow("mat",mat);
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}
