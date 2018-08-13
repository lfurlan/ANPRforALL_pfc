#include "preprocesador.hpp"

Preprocesador::Preprocesador(){
	error=0;
}

Preprocesador::~Preprocesador(){

	imgGris.release();
	imgResize.release();
}


int Preprocesador::ejecutar(Mat imgEntrada,Mat &imgSalida){
	//0-imagOrig,1-imagOrigGris,2-imagEscalada,3-imagEscaladaGris;
	error=0;

//	if(res2->error) return(res2);
//	res=res2;
	imgGris=Mat(imgEntrada.cols, imgEntrada.rows, CV_8UC1);
//	imagenes.clear();
//	imagenes.push_back(res->imgEntrada);
//	imagenes.push_back(Mat(imgEntrada.cols, imgEntrada.rows, CV_8UC1));


	if (imgEntrada.channels() == 1){
		imgEntrada.copyTo(imgGris);
		cvtColor(imgEntrada, imgEntrada, CV_GRAY2BGR);
		}
	else {
		cvtColor(imgEntrada, imgGris, CV_BGR2GRAY);
	}




	float media = mean(imgGris)[0];
	float gamma = 0.194037 * exp(0.01136269 * media);



	/*1.1 Redimensionado imagen*/
	this->dimAdaptado.height=675;
	this->dimAdaptado.width=((float)imgEntrada.cols/imgEntrada.rows)*dimAdaptado.height;

	resize(imgGris, imgResize,Size(this->dimAdaptado.width, this->dimAdaptado.height), 0, 0,CV_INTER_CUBIC);

	/*1.2 Correccion Gamma*/
	imgResize.convertTo(imgResize, CV_32F, 1. / 255.);
	pow(imgResize, gamma, imgResize);
	imgResize.convertTo(imgResize, CV_8UC1, 255);


	/*1.3 Actualizacion imagEscaladaGris e imagMod*/
//	imagenes.push_back(Mat(dimAdaptado.width, dimAdaptado.height, CV_8UC1));
//	if (imagenes[2].channels() == 3)
//		cvtColor(imagenes[2], imagenes[3], CV_BGR2GRAY);
//	else
//		imagenes[2].copyTo(imagenes[3]);

	imgResize.copyTo(imgSalida);
	return(error);
}
