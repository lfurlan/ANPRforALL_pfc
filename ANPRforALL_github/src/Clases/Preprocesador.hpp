
#ifndef PREPROCESADOR_H_
#define PREPROCESADOR_H_

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


using namespace cv;
using namespace std;

class Preprocesador
{

  public:
	Preprocesador();
    virtual ~Preprocesador();

    int ejecutar(Mat imgEntrada,Mat &imgSalida);


  private:
//    Resultado *res;
    int error;
//    vector<Mat> imagenes;
	Size dimAdaptado;
	float gamma;
	Mat imgGris;
	Mat imgResize;

};

#endif /* PREPROCESADOR_H_ */
