#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

struct codigoError {
	int error;
	int tipoPatente;
	int tipoFalloPatente;

};

struct promedio {

	int cantidad;
	float promedio;
};


struct inicio_fin {
	Point inicio;
	Point fin;

};


struct franja {
	Point inicio;
	Point fin;
	Mat imagen;
	Mat ProyeccionX;
	Mat ProyeccionY;
	int altoCandY;
	vector<inicio_fin> pCandidatosX;
	double convProyX;
};

struct candidato{
	int pasada;	//0_Top-Hat 1_BlackHat
	int franja;
	int candX;
	Point inicio;
	Point fin;
	Mat imagen;
	Mat ProyeccionX;
	Mat ProyeccionY;
	double convProyX;
	double convProyFranjaX;
	float mediaPX;
	float mediaPY;
	float desvEstPX;
	float desvEstPY;
	int ancho;
	int alto;
	float relacion;
	int centroX;
	int centroY;
	promedio promProyX;
	float valor_ubicacion;
	float valor_relacion;
	float valor_medY;
	float valoracion;

    bool operator<(const candidato& a) const
    {
        return valoracion < a.valoracion;
    }


};


struct caracter {
	Mat imagen;
	Point inicio;
	Point fin;
	int tipo; //0-Letra, 1-Numero
};
