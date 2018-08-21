#ifndef PREPROCESADOR_H_
#define PREPROCESADOR_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

class Preprocesador {
public:
	Preprocesador();
	virtual ~Preprocesador();
	int ejecutar(Mat imgEntrada, Mat &imgSalida);
private:
	int error;
	Size dimAdaptado;
	float gamma;
	Mat imgGris;
	Mat imgResize;
};

#endif /* PREPROCESADOR_H_ */
