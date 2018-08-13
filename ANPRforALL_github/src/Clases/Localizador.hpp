
#ifndef LOCALIZADOR_H_
#define LOCALIZADOR_H_

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/ml/ml.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <math.h>
#include <fstream>
#include "estructuras.hpp"


#include "../Utils/pdi_functions.h"

using namespace cv;
using namespace std;
using namespace pdi;

class ResultadosLocalizacion {

public:

	Mat imgEntrada;
	Mat imgSalida;
	Mat imagAux;
	Mat imagMorfologica;
	Mat imagFranjasY;
	Mat imagFranjasYEnmasc;
	Mat imagPatOrig;
	Mat ProyeccionY;
	vector<franja> Franjas;
	vector<inicio_fin> pCandidatosY;
	vector<inicio_fin> pCandidatosX;
	vector<Mat> ProyeccionX;
	vector<double> resConvPatron;
	int mejorCandidatoY;
	vector<inicio_fin> pCandPatente;
	vector<int> indiceXCandPatente;
	int altoCandY;
	int mejorCandidato=-1;
	codigoError codError;

	bool recorrido;

	ResultadosLocalizacion(){
		recorrido=false;

	};

	virtual ~ResultadosLocalizacion(){};
};





class Localizador
{

  public:
	Localizador(Mat imagenEntrada,Mat imagenSalida);
    virtual ~Localizador();


    vector<inicio_fin> ejecutar(int tamanioEE1=7, int umbralBin=70);
    Mat obtener_candidato(int cand);
    void mostrar_pasos();
    Mat imagenEntrada;
    Mat imagenSalida;
    Mat imagenMostrar;

    ResultadosLocalizacion resultados[4];

  private:

    vector<candidato> Candidatos;
	vector<inicio_fin> candidatosPrincipales;
    double convProyXmin;
	double convProyXmax;
	float umbralProyY;
	int error;
	int pasada;
	Mat busquedaPorTopHat(Mat imagMod, Mat EE, int umbralBin);
	Mat busquedaPorBlackHat(Mat imagMod, Mat EE, int umbralBin);
	Mat busquedaPorTopHatYBlackHat(Mat imagTopHat, Mat imagBlackHat, Mat EE, int umbralBin);
	Mat busquedaPorContornos(Mat imagMod);
	Mat busquedaPorContornos2(Mat imagMod);
    Mat sacar_lineas_horizontales(Mat imagMod,int largo = 10,int umbralBin2=102);
    Mat proyeccion_eje_Y(Mat imagMod);
    Mat enmascarar_franjas(Mat imagMod, vector<inicio_fin> pCandidatosY);
    Mat enmascarar_ruido(Mat ProyeccionY, int ancho, float varAjusteY, int alto_min_cand);
    vector<inicio_fin> buscar_candidatos_Y(Mat ProyeccionY, int ancho, float varAjusteY = 1.4, int alto_min_cand = 10);
    Mat proyeccion_eje_X(Mat img);
    vector<double> pasar_patron(vector<inicio_fin> pCandidatosY, vector<Mat> ProyeccionX);
    double pasar_patron2(int alto,Mat ProyeccionX);
    int seleccionar_proyeccion_Y(vector<double> resConvPatron);
    vector<inicio_fin> buscar_candidatos_X(int alto, Mat ProyeccionX, int ancho);
    int organizar_candidatos(vector<inicio_fin> pCandPatente, int ancho, int alto);
    void organizar_candidatos2();
    void postproceso(Mat imgPatente);
    promedio buscar_promedios(Mat valores, int rangLibertad, int mostrar);

};

#endif /* LOCALIZADOR_H_ */
