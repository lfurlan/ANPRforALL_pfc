/*
 * experimento1.h
 *
 *  Created on: 21/10/2017
 *      Author: Lucas
 */

#ifndef EXPERIMENTO1_H_
#define EXPERIMENTO1_H_
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


class experimento1 {

  private:

	/*VARIABLES*/

	int numeroInicio, numeroFin;
	Mat img_origHSV;
	Mat img_orig_roi;
	Mat img_orig_roi_seg;
	Point maximo_i;
	string direccionEntradas;
	string direccionSalidas;
	int numero;
	Histogram1D h;
	Sheet* sheet;
	Book* book;

	String nombre;
	String s;
	stringstream out;
	ANPR *anpr;
	Mat img_orig, img_orig_gris,img_orig_hist,img_orig_salida,img_orig_busqueda_salida;
	Mat img_gamma,img_gamma_gris,img_gamma_hist,img_gamma_salida,img_gamma_busqueda_salida;
	float brilloMedioAntes;
	float brilloMedioDespues;
	float varAjusteY=1.6;
	float gamma=0.5;

  public:

	experimento1(string direccionEntradas,string direccionSalidas, int numeroInicio, int numeroFin)
    {
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



    void probarExperimento() {

    	for(int i=numeroInicio;i<numeroFin; i++){

				out.str("");
				out << i;
				s = out.str();


				//---------leer imagen-------------
				nombre =direccionEntradas+"/"+ s + ".jpg";
				//nombre ="C:/Users/Lucas/Google Drive/Facultad/PROYECTO FINAL DE CARRERA/Proyecto/Corpus de Imagenes/ASUS_mejoradas/"+ s + ".jpg";
				img_orig = imread(nombre,IMREAD_COLOR);
				resize(img_orig, img_orig, Size(((float)img_orig.cols/img_orig.rows)*675, 675), 0, 0, INTER_CUBIC);


				//---------cambio a gris-------------
				cvtColor(img_orig, img_orig_gris, CV_BGR2GRAY);
				//----------mostrar proceso localizacion post-gamma-------------------------------------
				anpr=new ANPR(img_orig,varAjusteY, gamma);
				anpr->preproceso();
				anpr->localizar();
				//imshow("img_orig_salida",concatenar_Mat(concatenar_Mat(concatenar_Mat(concatenar_Mat(concatenar_Mat(img_orig,pantalla->sacar_imagen(0),255),pantalla->sacar_imagen(1),255),pantalla->sacar_imagen(2),255),pantalla->sacar_imagen(3),255),pantalla->sacar_imagen(4),255));
				//img_orig_busqueda_salida=concatenar_Mat(concatenar_Mat(concatenar_Mat(concatenar_Mat(concatenar_Mat(img_orig,pantalla->sacar_imagen(0),255),pantalla->sacar_imagen(1),255),pantalla->sacar_imagen(2),255),pantalla->sacar_imagen(3),255),pantalla->sacar_imagen(4),255);
				//imwrite(direccionSalidas+"/"+s+ "_original_busqueda.png",img_orig_busqueda_salida);

				//---------brillo medio de imagen original-------------
				brilloMedioAntes=mean(img_orig_gris)[0];

		//
		//    	//----------tranformación gamma----------------
		//
		//    	img_gamma=aplicar_gama_ant(img_orig, brilloMedioAntes);
				cvtColor(anpr->sacar_imagen(0), img_gamma_gris, CV_BGR2GRAY);
				brilloMedioDespues=mean(img_gamma_gris)[0];
		//
				//--------histogramas----------------
				img_orig_hist=h.getHistogramImage(img_orig_gris);
				img_gamma_hist=h.getHistogramImage(img_gamma_gris);


		//    	//----------mostrar proceso localizacion post-gamma-------------------------------------
				//imshow("img_orig_histograma",concatenar_Mat(img_orig,img_orig_hist,255));
				imwrite(direccionSalidas+"/"+s+ "_original_histograma.png",concatenar_Mat_H(img_orig,img_orig_hist,255));

				//imshow("img_histograma",concatenar_Mat(pantalla->sacar_imagen(0),img_gamma_hist,255));
				imwrite(direccionSalidas+"/"+s+ "_gamma_histograma.png",concatenar_Mat_H(anpr->sacar_imagen(0),img_gamma_hist,255));

//
//


    	//-----------mostrar resultados----------
    				cout<<"se procesó imagen "<<s<<endl;
    				if(book){
    					if(numero==1){

    						sheet->writeStr(1, 1, "Imagen");
    						sheet->writeStr(1, 2, "Brillo medio antes");
    						sheet->writeStr(1, 3, "Brillo medio despues");
    					}
    					sheet->writeNum(numero+1, 1, numero);
    					sheet->writeNum(numero+1, 2, brilloMedioAntes);
    					sheet->writeNum(numero+1, 3, brilloMedioDespues);
    				}

    				char resultado;
    		}


    		if(book->save((direccionSalidas+"/Resultados.xls").c_str()))
    		{
    			::ShellExecute(NULL, "open", (direccionSalidas+"/Resultados.xls").c_str(), NULL, NULL, SW_SHOW);
    		}
    		else
    		{
    			std::cout << book->errorMessage() << std::endl;
    		}

    		book->release();
    		cvDestroyAllWindows();

    }




};


#endif /* EXPERIMENTO1_H_ */
