#include "Reconocedor.hpp"

Reconocedor::Reconocedor(){

}

Reconocedor::~Reconocedor(){
}


String Reconocedor::ejecutar(Mat imagenEntrada){

	if(!imagenEntrada.data){
		return("Error, no hay imagen segmentada");
	}

	imagenEntrada.copyTo(this->imagenEntrada);
	imwrite("imgPatente.jpg",imagenEntrada);
	//imwrite(direccionSalidas+"/"+s+ "_sec_busqueda.png",img_orig_busqueda_salida);
	system("tesseract imgPatente.jpg out -psm 7 -l lgf");

	ifstream ficheroEntrada;

	ficheroEntrada.open ("out.txt");
	getline(ficheroEntrada, this->numeracion);
	ficheroEntrada.close();

	return(numeracion);
}


void Reconocedor::mostrar_pasos(){
	Mat aux2(imagenEntrada.rows*1.5, imagenEntrada.cols,imagenEntrada.type(),Scalar(0,0,0));
	imagenEntrada.copyTo(aux2(Rect(0,0,imagenEntrada.cols,imagenEntrada.rows)));

	cvtColor(aux2,aux2,CV_GRAY2BGR);
	cv::putText(aux2,numeracion, Point(ceil(2*aux2.cols/18), aux2.rows-10),FONT_HERSHEY_DUPLEX, 0.5, Scalar(0,255,0), 1, 0.1);

	imshow("Resultado OCR",aux2);
}
