
#ifndef RECONOCEDOR_H_
#define RECONOCEDOR_H_

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





using namespace cv;
using namespace std;


class Reconocedor
{

  public:
	Reconocedor();
    virtual ~Reconocedor();
    String ejecutar(Mat imagenEntrada);

};

#endif /* RECONOCEDOR_H_ */
