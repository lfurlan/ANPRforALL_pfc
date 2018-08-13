#include <iostream>

#include <string.h>
#include <string>
#include <math.h>
#include <fstream>

#include "Clases\ANPR.hpp"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <math.h>
#include <fstream>
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include "Utils\pdi_functions.h"
#include "libxl.h"



using namespace cv;
using namespace std;
using namespace pdi;
using namespace tesseract;
using namespace libxl;

/*VARIABLES*/


Mat img_origHSV;
Mat img_orig_roi;
Mat img_orig_gris;
Mat img_orig_roi_seg;
Mat img_orig;
Mat img_orig_busqueda_salidaANPR1,img_orig_busqueda_salidaANPR2;
Mat img_aux;
float gamma=0.5;
float varAjusteY=1.6;
Point maximo_i;
string direccion =
		"C:/Users/Lucas/Google Drive/Facultad/PROYECTO FINAL DE CARRERA/Proyecto/Letras/";

String direccionSalidas="D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/CoordenadasPatentes.xls";

int numero;
String s;
stringstream out;

const int ee_slider_max = 10;
int ee_slider;
int ee;

const int umbral_slider_max = 15;
int umbral_slider;
int umbral;

double beta;
ANPR *anpr;
String nombre;
//ANPR_OpenCVLibro *anpr2;
Size dimAdaptado;


int main() {

	numero =306;
	int datos=0; //0-Localización: recorrer todo; 1-Localización: guardar datos; 2-Localización: ver_negativos
					//3-Segmentacion: recorrer positivos localizacion
	anpr=new ANPR();
	vector<inicio_fin> candidatos;

	/*------OBJETO EXCEL--------*/
	Book* book;
	Sheet* sheet;
	Sheet* sheet2;
	Sheet* sheet3;
	Sheet* sheet4;
	Sheet* sheet5;
	Sheet* sheet6;
	Sheet* sheetAux;



	bool mostrar_imagenes=false;






	otra_vez:

	inicio_fin inifin;
	Point centro, centroCand;
	int altoPatente, anchoPatente, altoCand, anchoCand;
	float difInicioX,difInicioY,difFinX,difFinY,difCentroX,difCentroY,difAncho,difAlto;
	bool esPatente=0;
	bool esPatenteC1, esPatenteC2, esPatenteC3, esPatenteC4;
	int cuenta, cuentaImagen;
	bool finAuto=false;


	unsigned t0, t1;



	while(numero<391){


		cout<<endl<<endl<<"imagen "<<numero<<endl;

		out.str("");
		out << numero;
		s = out.str();
		nombre ="D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO"
				"/"+ s + ".jpg";
		//nombre ="C:/Users/Lucas/Google Drive/Facultad/PROYECTO FINAL DE CARRERA/Proyecto/Corpus de Imagenes/ASUS2"
		//		"/"+ s + ".jpg";
		if(datos==0){	//recorrer}
			book = xlCreateBook();
			book->load("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/CoordenadasPatentes.xls");
			sheet= book->getSheet(0);
			//leo datos posiciones reales
			inifin.inicio.x  = sheet->readNum(numero+1, 3);
			inifin.inicio.y  = sheet->readNum(numero+1, 4);
			inifin.fin.x  = sheet->readNum(numero+1, 5);
			inifin.fin.y  = sheet->readNum(numero+1, 6);

			sheet6= book->getSheet(5);
			int esPatente=sheet6->readNum(numero+1, 7);

			book->release();
			img_orig = imread(nombre,IMREAD_COLOR);
			Mat imgSalida;
			candidatos=anpr->ejecutar(nombre,img_orig, imgSalida,2,0);
			cvtColor(imgSalida,imgSalida,CV_GRAY2BGR);
			rectangle(imgSalida, inifin.inicio,inifin.fin, Scalar(255,0,0),4);
			imwrite("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/Para mostrar/Patente.jpg",imgSalida);

			dimAdaptado.height=250;
			dimAdaptado.width=((float)img_orig.cols/img_orig.rows)*dimAdaptado.height;
			resize(imgSalida, imgSalida,dimAdaptado, 0, 0,CV_INTER_CUBIC);

			cout<<"esPatente: "<<esPatente<<endl;
//			if(esPatente) putText(imgSalida, "Patente encontrada", cvPoint(10,20), FONT_HERSHEY_COMPLEX, 0.8, cvScalar(0, 255, 0), 2, CV_AA);
//			else putText(imgSalida, "Patente NO encontrada", cvPoint(10,20), FONT_HERSHEY_COMPLEX, 0.8, cvScalar(0, 0, 255), 2, CV_AA);
			imshow("ubicacion patente", imgSalida);
			imwrite("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/Para mostrar/Patente.jpg",imgSalida);

			cvWaitKey(0);
		}


		if(datos==1){	//guardar
			img_orig = imread(nombre,IMREAD_COLOR);
			Mat imgSalida;

			t0=clock();
			candidatos=anpr->ejecutar(nombre,img_orig, imgSalida,datos,0);
			t1 = clock();

			double time = (double(t1-t0)/CLOCKS_PER_SEC);
			cout << "Execution Time: " << time << endl;


//
//			book = xlCreateBook();
//			if(book->load("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/CoordenadasPatentes.xls"))
//			{
//			  //sheet = book->addSheet("CoordenadasPatentes");
//			  sheet = book->getSheet(0);
//			  sheet2= book->getSheet(1);
//			  sheet3= book->getSheet(2);
//			  sheet4= book->getSheet(3);
//			  sheet5= book->getSheet(4);
//			  sheet6= book->getSheet(5);
//			}
//			if(sheet){
//				//leo datos posiciones reales
//				inifin.inicio.x  = sheet->readNum(numero+1, 3);
//				inifin.inicio.y  = sheet->readNum(numero+1, 4);
//				inifin.fin.x  = sheet->readNum(numero+1, 5);
//				inifin.fin.y  = sheet->readNum(numero+1, 6);
//				centro.x= sheet->readNum(numero+1, 7);
//				centro.y= sheet->readNum(numero+1, 8);
//				anchoPatente= sheet->readNum(numero+1, 9);
//				altoPatente= sheet->readNum(numero+1, 10);
//
//			}else {
//				cout<<"error"<<endl;
//			}
//
//			dimAdaptado.height=675;
//			dimAdaptado.width=((float)img_orig.cols/img_orig.rows)*dimAdaptado.height;
//
//			for (std::vector<inicio_fin>::size_type i = 0; i < candidatos.size(); i++) {
//
//				switch(i){
//				case 0:sheetAux=sheet2; break;
//				case 1:sheetAux=sheet3; break;
//				case 2:sheetAux=sheet4; break;
//				case 3:sheetAux=sheet5; break;
//				}
//
//				centroCand.x=candidatos[i].inicio.x+ceil((float)(candidatos[i].fin.x-candidatos[i].inicio.x) / 2);
//				centroCand.y=candidatos[i].inicio.y+ceil((float)(candidatos[i].fin.y-candidatos[i].inicio.y) / 2);
//				anchoCand=candidatos[i].fin.x-candidatos[i].inicio.x;
//				altoCand=candidatos[i].fin.y-candidatos[i].inicio.y;
//
//				difInicioX=(float)(inifin.inicio.x-candidatos[i].inicio.x)/dimAdaptado.width*100;
//				difInicioY=(float)(inifin.inicio.y-candidatos[i].inicio.y)/dimAdaptado.height*100;
//				difFinX=(float)(inifin.fin.x-candidatos[i].fin.x)/dimAdaptado.width*100;
//				difFinY=(float)(inifin.fin.y-candidatos[i].fin.y)/dimAdaptado.height*100;
//				difCentroX=(float)(centro.x-centroCand.x)/dimAdaptado.width*100;
//				difCentroY=(float)(centro.y-centroCand.y)/dimAdaptado.height*100;
//				difAncho=(float)(anchoPatente-anchoCand)/dimAdaptado.width*100;
//				difAlto= (float)(altoPatente-altoCand)/dimAdaptado.height*100;
//
//
//				//sheetAux->writeNum(numero+1, 1, numero);
//				sheetAux->writeNum(numero+1, 3, candidatos[i].inicio.x);
//				sheetAux->writeNum(numero+1, 4, candidatos[i].inicio.y);
//				sheetAux->writeNum(numero+1, 5, candidatos[i].fin.x);
//				sheetAux->writeNum(numero+1, 6, candidatos[i].fin.y);
//				sheetAux->writeNum(numero+1, 7, centroCand.x);
//				sheetAux->writeNum(numero+1, 8, centroCand.y);
//				sheetAux->writeNum(numero+1, 9, anchoCand);
//				sheetAux->writeNum(numero+1, 10, altoCand);
//				sheetAux->writeNum(numero+1, 11, difInicioX);
//				sheetAux->writeNum(numero+1, 12, difInicioY);
//				sheetAux->writeNum(numero+1, 13, difFinX);
//				sheetAux->writeNum(numero+1, 14, difFinY);
//				sheetAux->writeNum(numero+1, 15, difCentroX);
//				sheetAux->writeNum(numero+1, 16, difCentroY);
//				sheetAux->writeNum(numero+1, 17, difAncho);
//				sheetAux->writeNum(numero+1, 18, difAlto);
//				esPatente=0;
//				if(-2<difInicioX && difInicioX<2 && -2<difInicioY && difInicioY<2 && -2<difFinX && difFinX<2  && -2<difFinY && difFinY<2)
//				{
//					esPatente=1;
//				}
//
//				sheetAux->writeNum(numero+1, 19, esPatente);
//
//
//				//escribo en RESULTADO FINAL
//				sheet6->writeNum(numero+1, i+3,esPatente);
//
//
//
//				if(i==candidatos.size()-1){
//					cuenta=0;
//					for(int j=candidatos.size()-1;j>=0;j--){
//						  cuenta+= sheet6->readNum(numero+1, j+3);
//					}
//					if(cuenta>0){
//						sheet6->writeNum(numero+1, 7,1);
//					}else {
//						sheet6->writeNum(numero+1, 7,0);
//					}
//				}
//
//				//ultima imagen del auto (ultimo angulo)
//				if(finAuto){
//					cuentaImagen=0;
//					for(int j=6;j>0;j--){
//						cuentaImagen+= sheet6->readNum(numero-j+1, 7);
//					}
//					sheet6->writeNum(numero, 8,cuentaImagen);
//					finAuto=false;
//				}
//			}
//
//			if(book->save((direccionSalidas).c_str()))
//			{
//				//::ShellExecute(NULL, "open", (direccionSalidas+"/CoordenadasPatentes.xls").c_str(), NULL, NULL, SW_SHOW);
//			}
//			else
//			{
//				std::cout << book->errorMessage() << std::endl;
//			}
//			book->release();
		}


//		if(datos==2){	//mostrar imagenes de no encontradas
//				book = xlCreateBook();
//				if(book->load("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/CoordenadasPatentes.xls"))
//				{
//					sheet= book->getSheet(0);
//					if(sheet){
//						//leo datos posiciones reales
//						inifin.inicio.x  = sheet->readNum(numero+1, 3);
//						inifin.inicio.y  = sheet->readNum(numero+1, 4);
//						inifin.fin.x  = sheet->readNum(numero+1, 5);
//						inifin.fin.y  = sheet->readNum(numero+1, 6);
//					}
//
//				  sheet6= book->getSheet(5);
//				  if(sheet6->readNum(numero+1, 7)==0){
//					  cout<<"No se encontro patente"<<endl;
//						img_orig = imread(nombre,IMREAD_COLOR);
//						Mat imgSalida;
//						candidatos=anpr->ejecutar(nombre,img_orig, imgSalida,datos,0);
//						cvtColor(imgSalida,imgSalida,CV_GRAY2BGR);
//						rectangle(imgSalida, inifin.inicio,inifin.fin, Scalar(255,0,0),4);
//
//						dimAdaptado.height=250;
//						dimAdaptado.width=((float)img_orig.cols/img_orig.rows)*dimAdaptado.height;
//						resize(imgSalida, imgSalida,dimAdaptado, 0, 0,CV_INTER_CUBIC);
//
//						if(esPatente) putText(imgSalida, "Patente encontrada", cvPoint(10,20), FONT_HERSHEY_COMPLEX, 0.8, cvScalar(0, 255, 0), 2, CV_AA);
//						else putText(imgSalida, "Patente NO encontrada", cvPoint(10,20), FONT_HERSHEY_COMPLEX, 0.8, cvScalar(0, 0, 255), 2, CV_AA);
//
//						imshow("ubicacion patente", imgSalida);
//						cvWaitKey(0);
//				  }
//				  book->release();
//				}
//		}


		if(datos==3){	//recorrer}
			book = xlCreateBook();
			book->load("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/CoordenadasPatentes.xls");

			sheet6= book->getSheet(5);
			int PrimerCand=sheet6->readNum(numero+1, 9);

			img_orig = imread(nombre,IMREAD_COLOR);
			Mat imgSalida;

			int SegX=sheet6->readNum(numero+1, 13);
			int SeArregla=sheet6->readNum(numero+1,19);
			book->release();

	//		if(SeArregla==1){
				if(PrimerCand>0){
					candidatos=anpr->ejecutar(nombre,img_orig, imgSalida,datos,PrimerCand-1);
					cvtColor(imgSalida,imgSalida,CV_GRAY2BGR);
					rectangle(imgSalida,candidatos[PrimerCand-1].inicio,candidatos[PrimerCand-1].fin, Scalar(255,0,0),4);
					dimAdaptado.height=250;
					dimAdaptado.width=((float)img_orig.cols/img_orig.rows)*dimAdaptado.height;
					putText(imgSalida, s, cvPoint(40,60), FONT_HERSHEY_SIMPLEX, 3, cvScalar(255, 255, 0), 3, CV_AA);

					resize(imgSalida, imgSalida,dimAdaptado, 0, 0,CV_INTER_CUBIC);
					imshow("ubicacion patente", imgSalida);
					//imwrite("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/Para mostrar/Patente.jpg",imgSalida);

					cvWaitKey();
					cvDestroyAllWindows();
				}
		//	}


		}



			/*----------control de teclado----------------*/
//			int cod = cvWaitKey(0);
//				switch (cod) {
//
//				case 32: {
//
//					goto otra_vez;
//					break;
//				}
//
//				case 2555904: {
//					if (numero < 487)
//						numero++;
//					else
//						numero = 1;
//					cvDestroyAllWindows();
//					goto otra_vez;
//					break;
//				}
//
//				case 2424832: {
//					if (numero > 1)
//						numero--;
//					else
//						numero = 486;
//					cvDestroyAllWindows();
//					goto otra_vez;
//					break;
//				}
//
//				case 2490368: {
//
//					goto otra_vez;
//					break;
//				}
//				case 2621440: {
//
//					goto otra_vez;
//					break;
//				}
//			///
//				case 56: {
//
//					goto otra_vez;
//					break;
//				}
//				case 50: {
//
//					goto otra_vez;
//					break;
//				}
//				case 55: { //tecla 7
//
//					goto otra_vez;
//					break;
//				}
//				case 49: {
//
//					goto otra_vez;
//					break;
//				}
//				case 57: {
//
//					goto otra_vez;
//					break;
//				}
//				case 51: {
//
//					goto otra_vez;
//					break;
//				}
//
//					goto otra_vez;
//				}

			//	cvDestroyAllWindows();
	if(numero%6==0) {
		finAuto=true;

	}


	numero++;
	}








	return 0;
}


/*--------------------histograma-----------------------------*/





