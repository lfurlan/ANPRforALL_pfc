#include "Clases\ANPR.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	Mat img_orig;
	char* filename;
	int mostrar = 1;
	ANPR *anpr;

	if (argc > 1) {
		filename = argv[1];
		if (argc > 2) {
			mostrar = strtol(argv[2], NULL, 10);
			;
		}
	} else {
		printf("No pasó el path de la imagen a ser analizada.");
		return 0;
	}
	img_orig = imread(filename, 1);
	anpr = new ANPR();
	vector<inicio_fin> candidatos;
	Mat imgSalida;
	anpr->ejecutar(filename, img_orig, imgSalida, mostrar, 0);
	Size dimAdaptado;
	dimAdaptado.height = 250;
	dimAdaptado.width = ((float) img_orig.cols / img_orig.rows)
			* dimAdaptado.height;
	resize(imgSalida, imgSalida, dimAdaptado, 0, 0, CV_INTER_CUBIC);
	cvWaitKey(0);
	return 0;
}

