/*
 * experimento1.h
 *
 *  Created on: 21/10/2017
 *      Author: Lucas
 */

#ifndef EXPERIMENTO0_H_
#define EXPERIMENTO0_H_
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

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
#include "..\Utils\histogram.h"
#include "..\Utils\pdi_functions.h"
#include "libxl.h"



using namespace libxl;
using namespace cv;
using namespace std;
using namespace pdi;


class experimento0 {

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
	Mat img_orig, img_orig_gris,img_orig_hist,img_orig_salida,img_orig_busqueda_salida;
	Mat img_aux;
	float brilloMedioAntes;
	float brilloMedioDespues;
	float varAjusteY=1.6;
	float gamma=0.5;

  public:

	experimento0(string direccionEntradas,string direccionSalidas, int numeroInicio, int numeroFin)
    {
		this->numeroInicio=numeroInicio;
		this->numeroFin = numeroFin;
		this->direccionEntradas=direccionEntradas;
		this->direccionSalidas=direccionSalidas;

    }



    void probarExperimento() {

    	for(int i=numeroInicio;i<numeroFin; i++){

			out.str("");
			out << i;
			s = out.str();

			//---------leer imagen-------------
			nombre =direccionEntradas+"/"+ s + ".jpg";
			img_orig = imread(nombre,IMREAD_COLOR);

			cout<<"Procesando "<<nombre<<endl;
			resize(img_orig, img_orig, Size(((float)img_orig.cols/img_orig.rows)*675, 675), 0, 0, INTER_CUBIC);

//
//			//---------cambio a gris-------------
			cvtColor(img_orig, img_orig_gris, CV_BGR2GRAY);
			float media = mean(img_orig_gris)[0];
			gamma = 0.194037 * exp(0.01136269 * media);

			img_orig.copyTo(img_orig_busqueda_salida);
			int tamanio;

			for(int j=0;j<12; j+=3){
				tamanio=600;
				anpr=new ANPR(img_orig,varAjusteY, gamma);
				anpr->preproceso();
				anpr->localizar();


				img_aux=anpr->sacar_imagen(j);

				img_orig_busqueda_salida=concatenar_Mat_H(img_orig_busqueda_salida,img_aux,tamanio);

				anpr->~ANPR();

			}
			//imwrite(direccionSalidas+"/"+s+ "_sec_busqueda.png",img_orig_busqueda_salida);
			imshow("salida",img_orig_busqueda_salida);

			int cod = cvWaitKey(0);
			i++;
    		}


    }




};


#endif /* EXPERIMENTO0_H_ */
