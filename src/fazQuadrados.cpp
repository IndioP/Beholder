#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstring>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include <deque>
#include <math.h>
#include <chrono>
#include "Histogram.h"

#define GRANULARIDADE 5	
#define MAX_ITERATIONS 10
#define MAX_ID 10000
#define DISTANCIA 100


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
	int key;
}blob;

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

std::deque< std::vector< blob > > filaBlob;
std::deque< cv::Mat > filaImage;


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

void salvaSubImagem(long int countFrame, int countBlobs, cv::Mat &original, char pasta[], blob varBlob){
	char name[100];
	strcpy(name,pasta); 
	strcat(name,"/image");
	char aux[150];
	sprintf(aux,"%ld_%d",countFrame,countBlobs);
	strcat(name,aux);
	strcat(name,".jpg");
	cv::Mat m(original,cv::Rect(varBlob.minx,varBlob.miny,(varBlob.maxx-varBlob.minx),(varBlob.maxy-varBlob.miny)));
	cv::resize(m,m,cv::Size(200,200));
	cv::imwrite(name,m);
}

void findBlobs(std::vector< std::vector<Run> > &runs, cv::Mat &debugFrame, cv::Mat &original, char pasta[], Histogram &histPosX, Histogram &histPosY){
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
	 std::vector < blob > v;
	 static long int countFrame = 0;
    static int countBlobs = 0;
    for(unsigned long i = 0 ; i < runs.size() ; i++){

        for(unsigned long j  = 0 ; j < runs[i].size() ; j++){
            r = runs[i][j];
            
            if(r.papai == NULL) { //se não tem pai, ele é o pai
                cor = runs[i][j].color;

					 if(r.areaBlob){
	                varBlob.posx = r.sumY / r.areaBlob;
					 }

					 if(runs[i][j].areaBlob){
					 	 varBlob.posy = r.sumX / runs[i][j].areaBlob;
					 }

                varBlob.minx = r.inix;
                varBlob.miny = r.iniy;
                varBlob.maxx = r.endx;
                varBlob.maxy = r.endy;
					 varBlob.key = countBlobs;     
					          
                
               if(((varBlob.maxx - varBlob.minx) > 110) && ((varBlob.maxy - varBlob.miny) > 110)){
					
						
						             
					   v.push_back(varBlob);
                  countBlobs++;
               }
                
                
            }

        }
    }
    countFrame++;
	 filaBlob.push_front(v);
	 filaImage.push_front(original);
	 static std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	 if(countBlobs > MAX_ID){
	 	 countBlobs = 0;
	 }
	 if(filaBlob.size() > MAX_ITERATIONS){
	    for(blob &b : filaBlob.back()){
 	 	 	 histPosX.removeHist(b.posx);
		 	 histPosY.removeHist(b.posy);
		 }

		 filaBlob.pop_back();
		 filaImage.back().release();
		 filaImage.pop_back();
		 
	 }
	 if(filaBlob.size() > 1){
	 	 for(int i = 0; i < filaBlob[1].size(); i++){
			 blob &b(filaBlob[1][i]);
			 		 
			 if(i >= filaBlob[0].size()){
	 			 break;
			 }
			 blob &bk(filaBlob[0][i]);
			 double distancek = sqrt(((b.posx - bk.posx)*(b.posx - bk.posx))+((b.posy - bk.posy)*(b.posy - bk.posy)));
			 int j;
			 for(j = i; j < filaBlob[0].size(); j++){
				 blob &bi(filaBlob[0][j]);
			 	 double distancei = sqrt(((b.posx-bi.posx) * (b.posx-bi.posx))+((b.posy-bi.posy) * (b.posy-bi.posy)));
			 	 if(distancei <= distancek){
					 std::swap(filaBlob[0][i],filaBlob[0][j]);
					 std::swap(distancek,distancei);
				 }
			 }
			 blob &bi(filaBlob[0][i]);
		 	 double distancei = sqrt(((b.posx-bi.posx) * (b.posx-bi.posx))+((b.posy-bi.posy) * (b.posy-bi.posy)));
			 if(distancei < DISTANCIA){
			 	bi.key = b.key;
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		 		float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
			 	if(elapsedTime)
		 		std::cout << "velocidade = " << (bi.posx - b.posx)*1000/elapsedTime <<" , " << (bi.posy - b.posy)*1000/elapsedTime << std::endl;
				t1 = t2; 
			 }

		 }
		 
		 
	 }
	 for(int i = 0; i < filaBlob[0].size(); i++){
		 blob &b(filaBlob[0][i]);
		 histPosX.insertHist(b.posx);
		 histPosY.insertHist(b.posy);
		 cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),cv::Point(b.maxx,b.maxy),cv::Scalar(255,255,255));
		 char nameAux[10];
		 sprintf(nameAux,"%d",b.key);
		 cv::putText(debugFrame,nameAux,cv::Point(b.minx,b.miny),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,255),2);
 		 //salvaSubImagem(countFrame, b.key, original, pasta, b);
		 
  
	 }
    runs.clear();
}

int main(int argc, char *argv[]){
	if(argc < 3){
		std::cout << "passe o video e a pasta que salvaremos as imagens" << std::endl;
		exit(1);	
	}
	
	static_assert(MAX_ITERATIONS > 1,"max iterations precisa ser maior que 1");
	cv::VideoCapture cap(argv[1]);
	if(!cap.isOpened()){
	    std::cout << "Error opening video stream orddd file" << std::endl;
	    return -1;
  	}
	cv::Mat mat;
	cv::Mat matRGB;
	cv::Ptr<cv::BackgroundSubtractor> pBackSub = cv::createBackgroundSubtractorMOG2(1,16,true);

	cap >> matRGB;
	
	pBackSub->apply(matRGB, mat);
	cv::Mat element = cv::getStructuringElement(0, cv::Size( 2*1 + 1, 2*1+1 ), cv::Point( 1, 1 ) );
	Histogram histPosX(matRGB.rows,50);
	Histogram histPosY(matRGB.cols,50);
	
	

  	while(true){


  		for(int i = 0; i < GRANULARIDADE; i++){
			cap >> matRGB;
			if (matRGB.empty())
      		break;

		}
		if (matRGB.empty())
      	break;

		pBackSub->apply(matRGB, mat);

		cv::morphologyEx(mat, mat, cv::MORPH_OPEN,element);		
		
		cv::Mat original = matRGB.clone();
  		
     	std::vector< std::vector<Run> > R = run(mat);
	
		findBlobs(R,matRGB,original,argv[2], histPosX, histPosY);

		cv::imshow("testado e aprovado",matRGB);
		//cv::imshow("testado2",mat);
		cv::imshow("posx",histPosX.debug());
		cv::imshow("histPosY",histPosY.debug());

		int k = cv::waitKey(1);
		if(k == 27){
			std::cout << "encerrando o programa" << std::endl;
			break;
		}	
  	}
	

	for(std::vector < blob > &v : filaBlob){
		v.clear();	
	}	
	filaBlob.clear();
	for(cv::Mat &m : filaImage){
		m.release();	
	}
	filaImage.clear();
	cap.release();
	cv::destroyAllWindows();
	return 0;
}
