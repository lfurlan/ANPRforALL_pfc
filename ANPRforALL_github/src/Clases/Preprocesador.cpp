#include "preprocesador.hpp"

Preprocesador::Preprocesador() {
	error = 0;
}

Preprocesador::~Preprocesador() {
	imgGris.release();
	imgResize.release();
}

int Preprocesador::ejecutar(Mat imgEntrada, Mat &imgSalida) {
	error = 0;
	imgGris = Mat(imgEntrada.cols, imgEntrada.rows, CV_8UC1);
	if (imgEntrada.channels() == 1) {
		imgEntrada.copyTo(imgGris);
		cvtColor(imgEntrada, imgEntrada, CV_GRAY2BGR);
	} else {
		cvtColor(imgEntrada, imgGris, CV_BGR2GRAY);
	}
	float media = mean(imgGris)[0];
	float gamma = 0.194037 * exp(0.01136269 * media);
	/*1.1 Redimensionado imagen*/
	this->dimAdaptado.height = 675;
	this->dimAdaptado.width = ((float) imgEntrada.cols / imgEntrada.rows)
			* dimAdaptado.height;
	resize(imgGris, imgResize,
			Size(this->dimAdaptado.width, this->dimAdaptado.height), 0, 0,
			CV_INTER_CUBIC);

	/*1.2 Correccion Gamma*/
	imgResize.convertTo(imgResize, CV_32F, 1. / 255.);
	pow(imgResize, gamma, imgResize);
	imgResize.convertTo(imgResize, CV_8UC1, 255);

	imgResize.copyTo(imgSalida);
	return (error);
}
