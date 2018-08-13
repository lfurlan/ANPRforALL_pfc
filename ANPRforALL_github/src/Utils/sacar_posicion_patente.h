/*
 * experimento1.h
 *
 *  Created on: 21/10/2017
 *      Author: Lucas
 */

#ifndef SACAR_POSICION_PATENTE_H_
#define SACAR_POSICION_PATENTE_H_
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
#include "..\ANPR.cpp"
#include "histogram.h"
#include "libxl.h"



using namespace libxl;
using namespace cv;
using namespace std;


Mat img_orig, img_orig_mostrada;
bool clicled=false;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
	 	   {
	 		inicio_fin* inifin = (inicio_fin*) userdata;
	 	    switch(event){
	 		case EVENT_LBUTTONDOWN:

	 	        	clicled=true;
	 	        	inifin->inicio.x=x;
	 	        	inifin->inicio.y=y;
	 	        	break;

	 		case EVENT_LBUTTONUP:

	 	        	inifin->fin.x=x;
	 	        	inifin->fin.y=y;
	 	        	clicled=false;
	 	        	break;

	 		case EVENT_MOUSEMOVE:
	 	        	 if(clicled){
	 	 	        	inifin->fin.x=x;
	 	 	        	inifin->fin.y=y;
	 	        	 	 }
	 	        	break;

	 	    }


	 	    if(clicled)
	 	    {
			img_orig.copyTo(img_orig_mostrada);
			rectangle(img_orig_mostrada, Rect(inifin->inicio.x,inifin->inicio.y,x-inifin->inicio.x,y-inifin->inicio.y),Scalar(255,0,0),2,8,0);
			imshow("My Window", img_orig_mostrada);
	 	    }


	 	   }


class sacar_posicion_patente{

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
	Sheet* sheet;
	Book* book;
	int tamanioEE1;

	String nombre;
	String s;
	stringstream out, escritura;
	ANPR *anpr;
	Mat img_orig_gris,img_orig_hist,img_orig_salida,img_orig_busqueda_salida;
	Mat img_orig_busqueda_salida_h,img_orig_busqueda_salida_v;
	Mat img_aux;
	float brilloMedioAntes;
	float brilloMedioDespues;
	float varAjusteY=1.6;
	float gamma=0.5;
	inicio_fin patenteIndicada;

  public:
	sacar_posicion_patente(string direccionEntradas,string direccionSalidas, int numeroInicio, int numeroFin){
		 	this->numeroInicio=numeroInicio;
		 	this->numeroFin = numeroFin;
		 	this->direccionEntradas=direccionEntradas;
		 	this->direccionSalidas=direccionSalidas;

			/*------OBJETO EXCEL--------*/
			book = xlCreateBook();
			if(book)
			{
			  sheet = book->addSheet("Experimento1");
			}

	 }
	 //static void mouseHandler(int event,int x,int y, int flags,void* param);
	 void ejecutar(){

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


				imshow("My Window", img_orig);
				setMouseCallback("My Window", CallBackFunc,&patenteIndicada);
				waitKey(0);

		//		if(&patenteIndicada){
		//			cout<<"patenteIndicada.inicio.x: "<<patenteIndicada.inicio.x<<endl;
		//			cout<<"patenteIndicada.inicio.y: "<<patenteIndicada.inicio.y<<endl;
		//
		//			cout<<"patenteIndicada.fin.x: "<<patenteIndicada.fin.x<<endl;
		//			cout<<"patenteIndicada.fin.y: "<<patenteIndicada.fin.y<<endl;
		//		}


				if(book){
					if(i==numeroInicio){

						sheet->writeStr(1, 1, "Imagen");
						sheet->writeStr(1, 2, "inicio.x");
						sheet->writeStr(1, 3, "inicio.y");
						sheet->writeStr(1, 4, "fin.x");
						sheet->writeStr(1, 5, "fin.y");

					}
					sheet->writeNum(i+1, 1, i);
					sheet->writeNum(i+1, 2, patenteIndicada.inicio.x);
					sheet->writeNum(i+1, 3, patenteIndicada.inicio.y);
					sheet->writeNum(i+1, 4, patenteIndicada.fin.x);
					sheet->writeNum(i+1, 5, patenteIndicada.fin.y);

				}


				}


				if(book->save((direccionSalidas+"/posicion_patentes.xls").c_str()))
				{
					::ShellExecute(NULL, "open", (direccionSalidas+"/posicion_patentes.xls").c_str(), NULL, NULL, SW_SHOW);
				}
				else
				{
					std::cout << book->errorMessage() << std::endl;
				}

				book->release();
	 }




};






#endif /* SACAR_POSICION_PATENTE_H_ */
