#coding: utf-8
'''as imagens com exemplos positivos, deveram estar em positive/
as imagens com exemplos negativos, deveram estar em negative/
deverá ser selecionado a regiao com objetos clicando e arrastando de cima para baixo e da esquerda para a direita,
deve ser pressionado a tecla 's' para cada região definida, e 'n' para ir para a proxima imagem
se existirem mais de um objeto de interesse dentro da imagem selecione e pressione 's' mais de uma vez.
Quando um arquivo uma imagem for processada ela eh movida para a pasta done/

'''


#ATUALMENTE ESTÁ SALVANDO NA FORMA DE PATH_IMAGE X1 Y1 X2 Y1 (talvez essa ordem esteja um pouco alterada mas com a mesma ideia)


import commands
import cv2

positivos = "cars.info"
negativos = "bg.txt"


option = "w"

commands.getoutput("mkdir done")

files = commands.getoutput("ls")
files = files.split()
for fi in files:
	if(fi == positivos):
		option = "a"
		print("um arquivo cars.info foi encontrado, por isso vamos incrementar o arquivo, ao inves de criar um novo")


f = open(positivos,option) #arquivo de saida com a legenda das imagens positivas
f2 = open(negativos,"w") #arquivo de saida com a legenda das imagens negativas

count = 0
crops = " "

stop_condition = False

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
		cv2.rectangle(image, refPt[0], refPt[1], (0, 255, 0), 2)
		cv2.imshow("image", image)


cv2.namedWindow("image")
cv2.setMouseCallback("image", click_and_crop)


files = commands.getoutput("ls Carro/")
files = files.split()
for fi in files:
	if stop_condition:
		break	
	print("tentando ler: "+fi)
	image = cv2.imread("Carro/"+fi)	
	while True:
		cv2.imshow("image", image)
		key = cv2.waitKey(1)
		if key == ord("n"): #press 'n' to go to the next positive picture
			f.write("done/" + fi + " " + crops + "\n")
			commands.getoutput("mv Carro/"+fi+" done/")
			crops = " "
			count = 0
			break
		elif key == ord("s"): #press 's' to save the current selected region
			count+=1
			#crops += str(refPt[0][0])+ " " +str(refPt[0][1]) +" "+ str(refPt[1][0] - refPt[0][0])  + " " +str(refPt[1][1]-refPt[0][1])+" "
			crops += str(refPt[0][0]) +" "+ str(refPt[0][1]) +" "+ str(refPt[1][0] - refPt[0][0])  + " " +str(refPt[1][1]-refPt[0][1])+" "
			print(crops)
		elif key == 27: #Aperte esc para encerrar o programa
			print("encerrando o programa")
			stop_condition = True
			break
f.close()

files = commands.getoutput("ls negative/")
files = files.split()
for fi in files:
	f2.write("negative/"+fi+"\n")
f2.close()
