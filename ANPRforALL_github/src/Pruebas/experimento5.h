/*
 * experimento1.h
 *
 *  Created on: 21/10/2017
 *      Author: Lucas
 */

#ifndef EXPERIMENTO5_H_
#define EXPERIMENTO5_H_
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <math.h>
#include <fstream>
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include "..\Utils\histogram.h"
#include "..\Utils\pdi_functions.h"
#include "..\ANPR.cpp"
#include "libxl.h"



using namespace libxl;
using namespace cv;
using namespace std;





class experimento5 {

  private:

	/*VARIABLES*/


	Mat img_origHSV;
	Mat img_orig_roi;
	Mat img_orig_roi_seg;
	Point maximo_i;
	string direccionEntradas;
	string direccionSalidas;
	int numeroInicio, numeroFin;
	Histogram1D h;

	Book* bookEscritura;
	Sheet* sheetInicioX;
	Sheet* sheetInicioY;
	Sheet* sheetFinX;
	Sheet* sheetFinY;
	Sheet* sheetDistancia;

	Book* bookLestura;
	Sheet* sheetPosiciones;

	int tamanioEE1;

	String nombre;
	String s;
	stringstream out, escritura;
	ANPR *anpr;
	Mat img_orig, img_orig_gris,img_orig_hist,img_orig_salida,img_orig_busqueda_salida;
	Mat img_orig_busqueda_salida_h,img_orig_busqueda_salida_v;
	Mat img_aux;
	float brilloMedioAntes;
	float brilloMedioDespues;
	float varAjusteY=1.6;
	float gamma=0.5;
	inicio_fin patenteIndicada;

  public:
	 experimento5(string direccionEntradas,string direccionSalidas, int numeroInicio, int numeroFin){
		 	this->numeroInicio=numeroInicio;
		 	this->numeroFin = numeroFin;
		 	this->direccionEntradas=direccionEntradas;
		 	this->direccionSalidas=direccionSalidas;

		 	/*------OBJETO EXCEL--------*/
		 	bookEscritura=xlCreateBook();
		 	if(bookEscritura->load((this->direccionSalidas+"/Resultados.xls").c_str())){
	 			sheetInicioX = bookEscritura->getSheet(0);
	 			sheetInicioY = bookEscritura->getSheet(1);
	 			sheetFinX = bookEscritura->getSheet(2);
	 			sheetFinY = bookEscritura->getSheet(3);
	 			sheetDistancia = bookEscritura->getSheet(4);
	 		}

		 	bookLestura=xlCreateBook();
		 	if(bookLestura->load((this->direccionEntradas+"/posicion_patentes.xls").c_str())){
		 		sheetPosiciones= bookLestura->getSheet(0);
		 	}
	 }
	 //static void mouseHandler(int event,int x,int y, int flags,void* param);
	 void probarExperimento();
	 float compararCandidatos(int i,inicio_fin mejorCandidato);
};





void experimento5::probarExperimento() {

	//Create a window
	//namedWindow("My Window", 1);
    //set the callback function for any mouse event



	for(int i=numeroInicio;i<numeroFin; i++){



		out.str("");
		out << i;
		s = out.str();

		//---------leer imagen-------------
		nombre =direccionEntradas+"/"+ s + ".jpg";
		img_orig = imread(nombre,IMREAD_COLOR);

		cout<<"Procesando "<<nombre<<endl;
		resize(img_orig, img_orig, Size(((float)img_orig.cols/img_orig.rows)*675, 675), 0, 0, INTER_CUBIC);


		//imshow("My Window", img_orig);
		//setMouseCallback("My Window", CallBackFunc,&patenteIndicada);
		//waitKey(0);

//		if(&patenteIndicada){
//			cout<<"patenteIndicada.inicio.x: "<<patenteIndicada.inicio.x<<endl;
//			cout<<"patenteIndicada.inicio.y: "<<patenteIndicada.inicio.y<<endl;
//
//			cout<<"patenteIndicada.fin.x: "<<patenteIndicada.fin.x<<endl;
//			cout<<"patenteIndicada.fin.y: "<<patenteIndicada.fin.y<<endl;
//		}

		//---------cambio a gris-------------
//		cvtColor(img_orig, img_orig_gris, CV_BGR2GRAY);
//
//		img_orig.copyTo(img_orig_busqueda_salida_h);

		int tamanio;
		img_orig.copyTo(img_orig_busqueda_salida_v);
		for(int j=4;j<10; j++){ //tamaño EE
			for(int k=6;k<15; k++){ //corte binarización
				tamanio=400;
				anpr=new ANPR(img_orig,varAjusteY, gamma,j,k*10);
				anpr->preproceso();
				anpr->localizar();
//				img_aux=pantalla->sacar_imagen(9);
//
//				img_orig_busqueda_salida_h=concatenar_Mat_H(img_orig_busqueda_salida_h,img_aux,tamanio);
//
//				if(j==4){
//					escritura.str("");
//					escritura<<"Corte: "<<k;
//					cv::putText(img_orig_busqueda_salida_h, escritura.str(), Point((k-5)*tamanio, 30),
//								FONT_HERSHEY_DUPLEX, 1, Scalar(0,0,255), 1, 0.5);
//				}

				inicio_fin inifin=anpr->sacar_mejor_candidatos();
				anpr->~ANPR();
//

	            if(sheetInicioX)sheetInicioX->writeNum((6*(i-1))+(j-4)+2, k-4, inifin.inicio.x);
	            if(sheetInicioY)sheetInicioY->writeNum((6*(i-1))+(j-4)+2, k-4, inifin.inicio.y);
	            if(sheetFinX)sheetFinX->writeNum((6*(i-1))+(j-4)+2, k-4, inifin.fin.x);
	            if(sheetFinY)sheetFinY->writeNum((6*(i-1))+(j-4)+2, k-4, inifin.fin.y);
	            if(sheetDistancia)sheetDistancia->writeNum((6*(i-1))+(j-4)+2, k-4, compararCandidatos(i,inifin));
	            bookEscritura->save((direccionSalidas+"/Resultados.xls").c_str());
			}

//			escritura.str("");
//			escritura<<"EE: "<<j;
//			cv::putText(img_orig_busqueda_salida_h, escritura.str(), Point(450, 30),
//						FONT_HERSHEY_DUPLEX, 1, Scalar(0,0,255), 1, 0.5);
//			if(j==4){
//
//				img_orig_busqueda_salida_h.copyTo(img_orig_busqueda_salida_v);
//			} else{
//				img_orig_busqueda_salida_v=concatenar_Mat_V(img_orig_busqueda_salida_v,img_orig_busqueda_salida_h,img_orig_busqueda_salida_h.cols);
//			}
//			img_orig.copyTo(img_orig_busqueda_salida_h);
//
//
//
//		 	patenteIndicada=inicio_fin{Point(0,0),Point(0,0)};

		}
//		imwrite(direccionSalidas+"/"+s+ "_sec_busqueda.png",img_orig_busqueda_salida_v);

		}


		bookEscritura->release();
		bookLestura->release();


}


float experimento5::compararCandidatos(int i,inicio_fin mejorCandidato){
	inicio_fin infin;
	infin.inicio.x=(int)sheetPosiciones->readNum(i+2, 3);
	infin.inicio.y=(int)sheetPosiciones->readNum(i+2, 4);
	infin.fin.x=(int)sheetPosiciones->readNum(i+2, 5);
	infin.fin.y=(int)sheetPosiciones->readNum(i+2, 6);

	float distancia=sqrt(pow(infin.inicio.x-mejorCandidato.inicio.x,2)+
			pow(infin.inicio.y-mejorCandidato.inicio.y,2)+
			pow(infin.fin.x-mejorCandidato.fin.x,2)+
			pow(infin.fin.y-mejorCandidato.fin.y,2));

	return(distancia);
}



#endif /* EXPERIMENTO5_H_ */
