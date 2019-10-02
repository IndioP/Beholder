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
#include "Histogram2D.h"

#define GRANULARIDADE 5	
#define MAX_ITERATIONS 500
#define MAX_ID 10000
#define DISTANCIA 100


	/*😁️🌚️

	*/

int contaVerificados = 0;

typedef struct{
	int Area;
	int posx;
	int posy;
	int minx;
	int miny;
	int maxx;
	int maxy;
	int key;
	bool verificado;
	float velocidadex;
	float velocidadey;
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

void unionFind(std::vector< std::vector<Run> > &runs){
	int diff;
	Run *r1, *r2;
   unsigned long i1, i2; //indexes for first and second row
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

}

void findBlobs(std::vector< std::vector<Run> > &runs, cv::Mat &debugFrame, cv::Mat &original, char pasta[], Histogram2D &histPos1, Histogram2D &histPos2, Histogram2D &histPos3, Histogram2D &histPos4, Histogram &histVel/*X, Histogram &histVelY*/){
    uchar cor;
   
    // union find
	 unionFind(runs);

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
					 varBlob.verificado = false;
					          
                
               if(((varBlob.maxx - varBlob.minx) > 50) && ((varBlob.maxy - varBlob.miny) > 50)){
					
						
						             
					   v.push_back(varBlob);
                  countBlobs++;
               }
                
                
            }

        }
		  runs[i].clear();
    }
    countFrame++;
	 filaBlob.push_front(v);
	 filaImage.push_front(original);
	 static std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	 if(countBlobs > MAX_ID){
	 	 countBlobs = 0;
	 }
	 if(filaBlob.size() > MAX_ITERATIONS){
	    if(contaVerificados > MAX_ITERATIONS && filaBlob[0].size()){
			 for(blob &b : filaBlob.back()){
				 if(b.verificado){
					if((b.velocidadex > 0) && (b.velocidadey > 0)){
				 		histPos1.removeHist(b.posx,b.posy);
					}else if((b.velocidadex > 0) &&(b.velocidadey < 0)){
						histPos2.removeHist(b.posx,b.posy);
					}else if((b.velocidadex < 0) &&(b.velocidadey < 0)){
						histPos3.removeHist(b.posx,b.posy);
					}else{
						histPos4.removeHist(b.posx,b.posy);					
					}
					histVel.removeHist(sqrt((b.velocidadex*b.velocidadex)+(b.velocidadey*b.velocidadey)));
					//histVelY.removeHist(fabs(b.velocidadey));
				 }
			 }
			contaVerificados--;
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
				bi.verificado = true;	
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		 		float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
			 	if(elapsedTime){
					contaVerificados++;
					bi.velocidadex = (bi.posx - b.posx)*1000/elapsedTime;
					bi.velocidadey = (bi.posy - b.posy)*1000/elapsedTime;
					if((b.velocidadex > 0) && (b.velocidadey > 0)){
				 		if(histPos1.insertHist(b.posx,b.posy)){
							std::cout << "Quad1 dirigindo na mão correta " << std::endl;
							 cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																cv::Point(b.maxx,b.maxy),cv::Scalar(0,255,0));						
						}else{
							std::cout << "Quad1 dirigindo na contra mão " << std::endl;
							cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																		cv::Point(b.maxx,b.maxy),cv::Scalar(0,0,255));							
						}
					}else if((b.velocidadex > 0) &&(b.velocidadey < 0)){
						if(histPos2.insertHist(b.posx,b.posy)){
							std::cout << "Quad2 dirigindo na mão correta " << std::endl;
							cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																cv::Point(b.maxx,b.maxy),cv::Scalar(0,255,0));						
						}else{
							std::cout << "Quad2 dirigindo na contra mão " << std::endl;
							cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																		cv::Point(b.maxx,b.maxy),cv::Scalar(0,0,255));							
						}
					}else if((b.velocidadex < 0) &&(b.velocidadey < 0)){
						if(histPos3.insertHist(b.posx,b.posy)){
							std::cout << "Quad3 dirigindo na mão correta " << std::endl;
							cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																cv::Point(b.maxx,b.maxy),cv::Scalar(0,255,0));						
						}else{
							std::cout << "Quad3 dirigindo na contra mão " << std::endl;
							cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																		cv::Point(b.maxx,b.maxy),cv::Scalar(0,0,255));							
						}
					}else{
						if(histPos4.insertHist(b.posx,b.posy)){
							std::cout << "Quad4 dirigindo na mão correta " << std::endl;
							cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																cv::Point(b.maxx,b.maxy),cv::Scalar(0,255,0));						
						}else{
							std::cout << "Quad4 dirigindo na contra mão " << std::endl;
							cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),
																		cv::Point(b.maxx,b.maxy),cv::Scalar(0,0,255));							
						}					
					}
					if(histVel.insertHist(sqrt((b.velocidadex*b.velocidadex)+(b.velocidadey*b.velocidadey)))){
						cv::circle(debugFrame,cv::Point(b.posx,b.posy),10,cv::Scalar(0,255,0),-5);					
					}else{
						cv::circle(debugFrame,cv::Point(b.posx,b.posy),10,cv::Scalar(0,0,255),-5);
					}
					//histVelY.insertHist(fabs(bi.velocidadey));
					//std::cout << bi.velocidadex <<" , " << bi.velocidadey << std::endl;
				}else{
					bi.verificado = false;				
				}
				t1 = t2; 
			 }

		 }
		 
		 
	 }
	 for(int i = 0; i < filaBlob[0].size(); i++){
		 blob &b(filaBlob[0][i]);
//		 cv::rectangle(debugFrame,cv::Point(b.minx,b.miny),cv::Point(b.maxx,b.maxy),cv::Scalar(255,255,255));
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
	Histogram2D histPos1(matRGB.cols,100,matRGB.rows,100);
	Histogram2D histPos2(matRGB.cols,100,matRGB.rows,100);
	Histogram2D histPos3(matRGB.cols,100,matRGB.rows,100);
	Histogram2D histPos4(matRGB.cols,100,matRGB.rows,100);
	Histogram histVel(matRGB.cols*2,50);
	//Histogram histVelY(matRGB.cols*2,50);
	
	

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
	
		findBlobs(R,matRGB,original,argv[2], histPos1, histPos2, histPos3, histPos4, histVel/*X, histVelY*/);

		cv::imshow("testado e aprovado",matRGB);
		cv::imshow("testado2",mat);
		cv::imshow("Pos1",histPos1.debug());
		cv::imshow("Pos2",histPos2.debug());
		cv::imshow("Pos3",histPos3.debug());
		cv::imshow("Pos4",histPos4.debug());
		cv::imshow("velocidade",histVel.debug());
		//cv::imshow("VelocidadeY",histVelY.debug());

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
