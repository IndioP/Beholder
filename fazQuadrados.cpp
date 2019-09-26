#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstring>
#include <cstdio>
#include "opencv2/opencv.hpp"


	/*😁️🌚️

	*/



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

struct Run{
     int start;
     int width;
     int areaBlob;
     int parent;
     int parentRow;
     int sumX;
     int sumY;
     int endx;
     int endy;
     int inix;
     int iniy;
     uchar color;
     Run* papai;
};

std::vector< std::vector<Run> > run(const cv::Mat &matrix){
    std::vector< std::vector<Run> > runs;
   

    int runLenght;
    for (int i=0; i < matrix.rows; i++) {

        const uchar * pt = matrix.ptr<uchar>(i);
        std::vector<Run> line;

        for (int j=0; j < matrix.cols; j++) {

            for (runLenght = 0; runLenght + j < matrix.cols && pt[j + runLenght]; runLenght++);
				
            Run current;
            current.start = j;
            current.color = pt[j];
            current.width = runLenght;
            current.areaBlob = current.width;
            current.parent = -1;
            current.sumX = i * current.width;
            current.sumY = current.width * ((current.width/2) + current.start); 
            current.endx = current.start+current.width;
            current.endy = i;
            current.inix = j;
            current.iniy = i;
            current.papai = NULL;

            if(current.color != 0)line.push_back(current);
				if(runLenght > 0){
					runLenght--;
				}
            j+=runLenght;
        }

        runs.push_back(line);
    }
    return runs;

}

Run* HxH(Run *aux){

	while(aux->papai!=NULL){
		aux = aux->papai;	
	}
	
	return aux;
}

void findBlobs(std::vector< std::vector<Run> > &runs, cv::Mat &debugFrame, cv::Mat &original){
    uchar cor;
    int diff;

    Run *r1, *r2;
    unsigned long i1, i2; //indexes for first and second row

    // union find
    for(unsigned long row = 1 ; row < runs.size() ; row++){
        i1=0, i2=0;

        while(i2 < runs[row].size() && i1 < runs[row-1].size()) {
            r1 = &runs[row-1][i1], r2 = &runs[row][i2];

            if (( ((*r2).start <= (*r1).start && (*r1).start <  ((*r2).start+(*r2).width))
                      || ((*r1).start <= (*r2).start  && (*r2).start < ((*r1).start+(*r1).width)) ) ) {

    		
				Run *aux = HxH(r1);
				  
				Run *aux2 = HxH(r2);
				if(aux != aux2){
				  		 
        		 	aux->parent = i2;
        		 	aux->parentRow = row; 
        
       		 		aux2->areaBlob += aux->areaBlob;
      		  
					aux2->sumX += aux->sumX;
        		 	aux2->sumY += aux->sumY;
        		 	if(aux2->iniy > aux->iniy){
        		 		aux2->iniy = aux->iniy;
        			}
        			if(aux->endx > aux2->endx){
        				aux2->endx = aux->endx;
        			}
        			if(aux->inix < aux2->inix){
        				aux2->inix = aux->inix;
        			}
        			aux->papai = aux2;
				}

            } 

            diff = ((*r2).start + (*r2).width) - ((*r1).start + (*r1).width);

            if(diff >= 0) i1++;
            if(diff <= 0) i2++;

        }

    }

    // turning fathers into blobs
    blob varBlob;
    Run r;
    static long int countBlobs = 0;
    for(unsigned long i = 0 ; i < runs.size() ; i++){

        for(unsigned long j  = 0 ; j < runs[i].size() ; j++){
            r = runs[i][j];
            
            if(r.papai == NULL) {
                cor = runs[i][j].color;
                varBlob.posx = r.sumY / r.areaBlob;
                varBlob.posy = r.sumX / runs[i][j].areaBlob;
                varBlob.minx = r.inix;
                varBlob.miny = r.iniy;
                varBlob.maxx = r.endx;
                varBlob.maxy = r.endy;
                
                
                
               if(((varBlob.maxx - varBlob.minx) > 110) && ((varBlob.maxy - varBlob.miny) > 110)){
					
					char name[100] = "dataSet2/image";
					char aux[150];
					sprintf(aux,"%ld",countBlobs);
					strcat(name,aux);
					strcat(name,".jpg");
					cv::Mat m(original,cv::Rect(varBlob.minx,varBlob.miny,(varBlob.maxx-varBlob.minx),(varBlob.maxy-varBlob.miny)));
					cv::resize(m,m,cv::Size(200,200));
					cv::imwrite(name,m);
cv::rectangle(debugFrame,cv::Point(varBlob.minx,varBlob.miny),cv::Point(varBlob.maxx,varBlob.maxy),cv::Scalar(255,255,255));               
					     //cv::circle(debugFrame,cv::Point(varBlob.posx,varBlob.posy),5,cv::Scalar(0,0,0),1, CV_AA);
                    //std::cout << "area = " << r.areaBlob <<" "<< varBlob.posx<<" "<<varBlob.posy << std::endl;
                    countBlobs++;
                }
                
                
            }

        }
    }
    
    runs.clear();
}


int main(void){
	cv::VideoCapture capRGB("video2Subtraction_rgb.avi");
	if(!capRGB.isOpened()){
	    std::cout << "Error opening video stream orddd file" << std::endl;
	    return -1;
  	}

 
	cv::VideoCapture capGRAY("video2Subtraction_gray.avi");
	if(!capGRAY.isOpened()){
    	std::cout << "Error opening video stream or file" << std::endl;
    	return -1;
  	}
  	while(true){
  		cv::Mat mat;
  		cv::Mat matRGB;

  		for(int i = 0; i < 5; i++){
			capGRAY >> mat;
			if (mat.empty())
      		break;
			capRGB >> matRGB;
		}
		if (mat.empty())
      	break;
		cv::cvtColor(mat,mat,cv::COLOR_BGR2GRAY);
  		
		
		cv::Mat original = matRGB.clone();
  		
  

     	std::vector< std::vector<Run> > R = run(mat);
	
	
		findBlobs(R,matRGB,original);

		cv::imshow("testado e aprovado",matRGB);
		cv::imshow("testado2",mat);
		//std::cout << "chegou aqui " << std::endl;
		int k = cv::waitKey(1);
		if(k == 27){
			std::cout << "encerrando o programa" << std::endl;
			break;
		}	
  	}
	/*cv::Mat mat = cv::imread("subtraction_gray.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat matRGB = cv::imread("subtraction_rgb.png", CV_LOAD_IMAGE_COLOR);

	std::vector< std::vector<Run> > R = run(mat);
	
	
	findBlobs(R,mat);

	cv::imshow("testado e aprovado",matRGB);
	cv::imshow("testado2",mat);
	cv::waitKey(0);*/
	capRGB.release();
	capGRAY.release();
	cv::destroyAllWindows();
	return 0;
}
