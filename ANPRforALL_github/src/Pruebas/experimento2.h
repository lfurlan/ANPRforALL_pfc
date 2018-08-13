/*
 * experimento1.h
 *
 *  Created on: 21/10/2017
 *      Author: Lucas
 */

#ifndef EXPERIMENTO2_H_
#define EXPERIMENTO2_H_
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
#include "..\Utils\histogram.h"
#include "..\Utils\pdi_functions.h"
#include "..\ANPR.cpp"
#include "libxl.h"



using namespace libxl;
using namespace cv;
using namespace std;
using namespace pdi;


class experimento2 {

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

	experimento2(string direccionEntradas,string direccionSalidas, int numeroInicio, int numeroFin)
    {
		this->numeroInicio=numeroInicio;
		this->numeroFin = numeroFin;
		this->direccionEntradas=direccionEntradas;
		this->direccionSalidas=direccionSalidas;
//		/*------OBJETO EXCEL--------*/
//		book = xlCreateBook();
//		if(book)
//		{
//		  sheet = book->addSheet("Experimento1");
//		}
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


			//---------cambio a gris-------------
			cvtColor(img_orig, img_orig_gris, CV_BGR2GRAY);

			img_orig.copyTo(img_orig_busqueda_salida);
			for(int j=4;j<10; j++){
				anpr=new ANPR(img_orig,varAjusteY, gamma,j);
				anpr->preproceso();
				anpr->localizar();

				escritura.str("");
				escritura<<"EE: "<<j;
				img_aux=anpr->sacar_imagen(9);

				img_orig_busqueda_salida=concatenar_Mat_H(img_orig_busqueda_salida,img_aux,250);

				cv::putText(img_orig_busqueda_salida, escritura.str(), Point((j-3)*250, 30),
							FONT_HERSHEY_DUPLEX, 0.7, Scalar(0,255,0), 1, 0.5);
				anpr->~ANPR();

			}
			//imwrite(direccionSalidas+"/"+s+ "_sec_busqueda.png",img_orig_busqueda_salida);
			imshow(s+ "_sec_busqueda.png",img_orig_busqueda_salida);
    		}


    }




};


#endif /* EXPERIMENTO2_H_ */
