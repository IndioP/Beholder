import numpy as np
import commands
import cv2 

def contornos(imagem):
	imagem2, contornos, hierarquia = cv2.findContours(imagem, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
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
    cv2.imshow('Imagem',imagem)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def mask_frame(video):
    cap = cv2.VideoCapture(video+'.mp4')

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

def click_and_crop(event, x, y, flags, param):
	# grab references to the global variables
	global refPt, cropping

	# if the left mouse button was clicked, record the starting
	# (x, y) coordinates and indicate that cropping is being
	# performed
	if event == cv2.EVENT_LBUTTONDOWN:
		refPt = [(x, y)]
		cropping = True

	# check to see if the left mouse button was released
	elif event == cv2.EVENT_LBUTTONUP:
		# record the ending (x, y) coordinates and indicate that
		# the cropping operation is finished
		refPt.append((x, y))
		#count+=1
		cropping = False

		# draw a rectangle around the region of interest
		cv2.rectangle(gray_frame, refPt[0], refPt[1], (0, 255, 0), 2)
		cv2.imshow("Video_Rodando", gray_frame)

count = 0
crops = " "
stop_condition = True

cv2.namedWindow("Video_Rodando")
cv2.setMouseCallback("Video_Rodando", click_and_crop)

cap = cv2.VideoCapture("video2.mp4")

while True:
    _, frame = cap.read()
    break

imagem_mascara = np.ones(frame.shape, dtype=np.uint8)


while True:
	stop_condition = True

	_, frame = cap.read()
	gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	gray_frame = cv2.GaussianBlur(gray_frame, (5, 5), 0)
	#gray_frame = cv2.resize(gray_frame,(720,480))
	cv2.imshow("Video_Rodando", gray_frame)
	#print("chegou aqui")

	k = cv2.waitKey(1)
	if (k == ord('q')):
		
		cv2.imwrite('mascara.png',imagem_mascara)
		cap.release()
		cv2.destroyAllWindows()  
		break

	if (k == ord('p')):
		while stop_condition:

			#cv2.imshow("Video_Rodando", gray_frame)
			#imagem_mascara = cv2.resize(imagem_mascara,(720,480))
			cv2.imshow("Video", imagem_mascara)

			key = cv2.waitKey(1)
			if key == ord("n"): 
				crops = " "
				count = 0
				stop_condition = False
				break
			elif key == ord("s"): #press 's' to save the current selected region
				try:
					count+=1
					crops += str(refPt[0][0]) +" "+ str(refPt[0][1]) +" "+ str(refPt[1][0] - refPt[0][0])  + " " +str(refPt[1][1]-refPt[0][1])+" "
					x0 = refPt[0][0]
					y0 = refPt[0][1]
					x1 = x0 + (refPt[1][0] - refPt[0][0])
					y1 = y0 + (refPt[1][1]-refPt[0][1])

					print("[%d:%d , %d:%d]"%(y0,y1,x0,x1))
					imagem_mascara[y0:y1,x0:x1] = frame[y0:y1,x0:x1]
				except Exception as e:
					pass

			elif key == 27: #Aperte esc para encerrar o programa
				print("encerrando o programa")
				stop_condition = False
						  
