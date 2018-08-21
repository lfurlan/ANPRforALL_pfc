
#ifndef SEGMENTADOR_H_
#define SEGMENTADOR_H_

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <math.h>
#include <fstream>
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include "../Utils/pdi_functions.h"
#include "estructuras.hpp"




using namespace cv;
using namespace std;
using namespace pdi;




class ResultadosSegmentacion {

public:

	Mat imagPatOrig;
	Mat imagTopHat;
	Mat imagBlackHat;
	Mat imagBinaria;
	Mat imagBinariaRotada;
	Mat imagBinariaRotadaSegY;
	Mat imagBinariaRotadaSegX;
	Mat imagSegmentada;
	vector<caracter> Caracteres;//vector<caracter> Caracteres;
	Mat imagBinariaSeg;

	Mat imagBinariaSegX;

	Mat imagGrisRotadaSegmY;

	Mat imagBordes;
	Mat imagBordesHough;
	Mat imagBordesRotada0;
	Mat imagBordesRotada;
	Mat imagBordesRotada2;

	Mat imagBordesRotadaSegY;

	Mat imagPatOrigGris;
	Mat imagPatEscalada;
	Mat imagProyY;
	Mat imagProyX;
	Mat imagPatMod;
	Mat imagPatAux;
	Mat ProyeccionY_patente;
	Mat ProyeccionY_patente2;
	Mat ProyeccionX_patente;
	Mat ProyeccionX_patente_Seg;
	Mat ProyeccionX_patente_compPatente;
	int umbralProyYpat;
	int umbralProyYpat2;
	int umbralProyXpat;
	int umbralProyXpatBin;
	vector<inicio_fin> pCandidatosY_patente;
	vector<inicio_fin> pCandidatosY_patente2;
	vector<inicio_fin> pCandidatosX_patente;
	codigoError codError;
	int mayor_alto;
	int ind_mayor_alto;
	float giro;
	float anguloLetras;
	vector<double> angulosVerticales;
	vector<double> angulosHorizontales;
	promedio mejorpromedioHor;
	promedio mejorpromedioVer;
	promedio mejorAnchoCaracter;
	Rect roiSegY;

	ResultadosSegmentacion(){

	};

	virtual ~ResultadosSegmentacion(){};
};

class Segmentador
{

  public:
	Segmentador();
    virtual ~Segmentador();
    Mat sacarImagen();
    codigoError ejecutar(Mat imagenEntrada);
	void mostrar_pasos();

  private:
	int paso;
	ResultadosSegmentacion resultados;
	int ind_mayor_alto;
    int preprocesar_patente();
    void corregir_inclinacion();
    int cambio_perspectiva();
    Mat cambio_angulo(Mat imagen);
    int quePatenteEs();
    int segmentarCaracteres();
    promedio buscar_promedios(vector<double> valores, int rangLibertad,int mostrar);
    Mat warpPerspective(Mat patente, int dx);
    Mat proyeccion_eje_Y_patente(Mat img);
    vector<inicio_fin> segmentar_proyeccion_Y_patente(Mat ProyeccionY_patente, int umbralProyYpat, int cols);
    Mat sacar_lineas_horizontales_patente(Mat patente,int umbralBin2 = 102);
    Mat proyeccion_eje_X_patente(Mat img);
    int segmentar_proyeccion_X_patente();
    bool controlar_caracter(Mat caracter);
    double pasar_patron(Mat Patente, Mat ProyeccionX);
    int comprobar_proyeccionY();
    int comprobar_proyeccionX();
    int postproceso();
};

#endif /* SEGMENTADOR_H_ */
