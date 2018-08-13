#include "Reconocedor.hpp"

Reconocedor::Reconocedor(){

}

Reconocedor::~Reconocedor(){
}


String Reconocedor::ejecutar(Mat imgEntrada){
	imwrite("Imagenes/imgPatente.png",imgEntrada);
	//imwrite(direccionSalidas+"/"+s+ "_sec_busqueda.png",img_orig_busqueda_salida);
	system("tesseract Imagenes/imgPatente.png out -psm 7 -l lgf");

	ifstream ficheroEntrada;
	string numeracionPatente;

	ficheroEntrada.open ("out.txt");
	getline(ficheroEntrada, numeracionPatente);
	ficheroEntrada.close();

	cvtColor(imgEntrada,imgEntrada,CV_GRAY2BGR);
	cv::putText(imgEntrada,numeracionPatente, Point(ceil(imgEntrada.cols/2)-10, 15),
								FONT_HERSHEY_DUPLEX, 0.7, Scalar(0,0,255), 1, 0.5);

	imshow("patenteSeg",imgEntrada);


	return("dasdas");
}
