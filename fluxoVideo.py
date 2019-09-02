import numpy as np
import cv2 as cv



def contornos(imagem):
	imagem2, contornos, hierarquia = cv.findContours(imagem, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
	return (imagem2,contornos,hierarquia)

def listando(lista):

    listaX = []
    listaY = []
    
    for i in lista:
        listaX.append(i[0][0])
        listaY.append(i[0][1])
        
    limiteX = sorted(listaX)
    limiteY = sorted(listaY)

    return limiteX[0],limiteX[-1],limiteY[0],limiteY[-1]

def mostrar_imagem(imagem):
    cv.imshow('Imagem',imagem)
    cv.waitKey(0)
    cv.destroyAllWindows()

def mask_frame(video):
    cap = cv.VideoCapture(video+'.mp4')

    while cap.isOpened():
        ret, frame = cap.read()
        # if frame is read correctly ret is True
        if not ret:
            print("Can't receive frame (stream end?). Exiting ...")
            break
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        cv.imshow('frame', gray)
        if cv.waitKey(1) == ord('q'):
            frame_mask = gray
            first_gray = cv.GaussianBlur(frame_mask, (5, 5), 0) 
            return first_gray
            break
    cap.release()
    cv.destroyAllWindows()
    return 0

fourcc = cv.VideoWriter_fourcc(*'XVID')

if __name__ == "__main__":
    first_frame_color = cv.imread('mascara.png')
    first_frame_color = cv.GaussianBlur(first_frame_color, (5, 5), 0)
    first_frame_gray = cv.cvtColor(first_frame_color, cv.COLOR_BGR2GRAY)

    #first_frame = cv.resize(first_frame,(720,480))
    #cv.imshow('testando',first_frame_color)
    #cv.waitKey(0)
    #cv.destroyAllWindows()

    #cv.createTrackbar('min','testando',0,255,nothing)
    #cv.createTrackbar('max','testando',0,255,nothing)
    #cv.setTrackbarPos('min','testando',25)
    #cv.setTrackbarPos('max','testando',255)

    cap = cv.VideoCapture("video2.mp4")
    #cv.waitKey(0)
    #print("dsafjklasdjfk",first_frame.shape)

    fgbg = cv.createBackgroundSubtractorMOG2()
    first_frame_color = fgbg.apply(first_frame_color)
    #print(first_fame_color.get())
    out = cv.VideoWriter('videoSubtraction_gray.avi',cv.VideoWriter_fourcc('M','J','P','G'), 10, (640,480))
    out2 = cv.VideoWriter('videoSubtraction_rgb.avi',cv.VideoWriter_fourcc('M','J','P','G'), 10, (640,480))
    while True:

        ret , frame = cap.read()
        frame2 = frame
        frame = cv.GaussianBlur(frame, (5, 5), 0)


        fgmask = fgbg.apply(frame2)

        gray_frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        gray_frame = cv.GaussianBlur(gray_frame, (5, 5), 0)
    
      
        #difference2 = cv.absdiff(first_frame_color,frame)
        difference2 = cv.absdiff(gray_frame,first_frame_gray)

        #minimo = cv.getTrackbarPos('min','testando')
        #maximo = cv.getTrackbarPos('max','testando')
        _, difference = cv.threshold(difference2, 25, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)
        kernel_1 = np.ones((5,5),np.uint8)
        
        opening = cv.morphologyEx(fgmask, cv.MORPH_OPEN,kernel_1)

        #kernel = np.ones((5,5),np.uint8)
        #closing = cv.morphologyEx(difference, cv.MORPH_CLOSE, kernel)
        #opening = cv.morphologyEx(closing, cv.MORPH_OPEN, kernel)
        #imagem_teste = cv.Canny(difference,100,200)

        #_ , cont , _ = contornos(imagem_teste)


        #print("TAMANHO DE :",len(cont))

        #cv.imshow("Difference2", difference2)
        #cv.imshow("Difference", difference)
        cv.imshow("Closing", opening)
        cv.imshow("Imagem", difference2 )
        opening = cv.resize(opening,(640,480))
        frame2 = cv.resize(frame2,(640,480))
        opening = cv.cvtColor(opening,cv.COLOR_GRAY2BGR)
        out.write(opening)
        out2.write(frame2)
            
        k = cv.waitKey(100)
        if (k == ord('q')):
            cap.release()
            cv.destroyAllWindows()  
            break
        if (k == ord('s')):
            cv.imwrite("subtraction_rgb.png",frame2)
            cv.imwrite("subtraction_gray.png",opening)
            
            break
        
        #cv.waitKey(200)
    print("chegou aqui")
    out.release()
    out2.release()
        
    
