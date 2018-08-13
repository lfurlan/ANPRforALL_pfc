#include "Localizador.hpp"


Localizador::Localizador(Mat imagenEntrada,Mat imagenSalida){
	this->imagenEntrada=imagenEntrada;
	this->imagenSalida=imagenSalida;
	imagenEntrada.copySize(resultados[0].imgEntrada);
	imagenEntrada.copySize(resultados[1].imgEntrada);
	imagenEntrada.copySize(resultados[2].imgEntrada);
	pasada=1;
}

Localizador::~Localizador(){
	imagenEntrada.release();
	imagenSalida.release();
    imagenMostrar.release();
    delete [] resultados;
    Candidatos.clear();
    candidatosPrincipales.clear();
}

vector<inicio_fin> Localizador::ejecutar(int tamanioEE1, int umbralBin){
	//imagenEntrada.copyTo(imagOrig);
	int centro_img_x = ceil((float)imagenEntrada.cols / 2);
	int centro_img_y = ceil((float)imagenEntrada.rows / 2);
	float distancia_max = sqrt(pow(centro_img_x, 2) + pow(centro_img_y, 2));

	imagenEntrada.copyTo(imagenMostrar);
	error=0;
	//if(pasada>2)return(-1);

	Mat EE1;
	Mat EE2;
	EE1 = getStructuringElement(CV_SHAPE_ELLIPSE, cv::Size(tamanioEE1, tamanioEE1));
	EE2 = getStructuringElement(CV_SHAPE_RECT, cv::Size(2, 2));// el posta es 2

	/*--2.1-BUSQUEDA DE ELEMENTOS DE INTERES--*/

	convProyXmin=10;
	convProyXmax=0;

	for (int i=1;i<3;i++){
		resultados[i].codError.error=0;
		imagenEntrada.copyTo(resultados[i].imgEntrada);
		imagenEntrada.copyTo(resultados[i].imgSalida);
		cvtColor(resultados[i].imgSalida, resultados[i].imgSalida, CV_GRAY2BGR);

		switch (i) {
//				case 0: {
//					morphologyEx(resultados[i].imgEntrada, resultados[1].imagAux, MORPH_TOPHAT, EE1);
//					morphologyEx(resultados[i].imgEntrada, resultados[2].imagAux, MORPH_BLACKHAT, EE1);
//
//					Mat imagBlackHatinv =  cv::Scalar::all(255) - resultados[2].imagAux;
//					Mat imagTopYBlack;
//					bitwise_and(resultados[1].imagAux,imagBlackHatinv, imagTopYBlack);
//					resultados[i].imagAux=imagTopYBlack;
//					break;
//				}
				case 1: {
					morphologyEx(resultados[i].imgEntrada, resultados[i].imagAux, MORPH_TOPHAT, EE1);
					break;
				}
				case 2: {
					morphologyEx(resultados[i].imgEntrada, resultados[i].imagAux, MORPH_BLACKHAT, EE1);
					break;
				}
				case 3: {
					/*ver bien como sigue este camino*/
					resultados[3].imagAux=busquedaPorContornos(imagenEntrada);

					break;
				}

			}

		threshold(resultados[i].imagAux, resultados[i].imagAux, umbralBin, 255, cv::THRESH_BINARY);


		resultados[i].imagFranjasY=resultados[i].imagAux;


		cvtColor(resultados[i].imagFranjasY, resultados[i].imagFranjasY, CV_GRAY2BGR);

		/*2.1.3 Erosion*/
		morphologyEx(resultados[i].imagAux, resultados[i].imagAux, MORPH_ERODE, EE2);
//		medianBlur(resultados[i].imagAux,resultados[i].imagAux,3);


		//imagAux.copyTo(imagOrig);

		/*2.1.4 Sacar lineas horizontales*/
		resultados[i].imagAux=sacar_lineas_horizontales(resultados[i].imagAux);



		/*2.2 Proyección sobre el eje Y*/
		resultados[i].ProyeccionY=proyeccion_eje_Y(resultados[i].imagAux);




		//Mat ProyeccionY=enmascarar_ruido(ProyeccionY);

		/*2.3 Busqueda de maximos en la proyección Y*/
		resultados[i].pCandidatosY=buscar_candidatos_Y(resultados[i].ProyeccionY,resultados[i].imagAux.cols);



		/*2.4 Proyecciones sobre el eje X para las franjas candidatas en el eje Y*/

		stringstream out;
		for (std::vector<int>::size_type j = 0; j < resultados[i].pCandidatosY.size(); j++) {
			//agrego elemento FRANJA a los resultados
			resultados[i].Franjas.push_back({});
			resultados[i].Franjas[j].inicio=resultados[i].pCandidatosY[j].inicio;
			resultados[i].Franjas[j].fin=resultados[i].pCandidatosY[j].fin;

			Rect r=Rect(resultados[i].pCandidatosY[j].inicio,resultados[i].pCandidatosY[j].fin);

			Mat aux;
			int ancholetra=ceil(float (resultados[i].Franjas[j].fin.y-resultados[i].Franjas[j].fin.y)*0.07);


			//Mat EE1 = getStructuringElement(CV_SHAPE_ELLIPSE, cv::Size(tamanioEE1, tamanioEE1));
			//morphologyEx(resultados.imagPatMod, resultados.imagBlackHat, MORPH_BLACKHAT, EE3);
			threshold(resultados[i].imgEntrada(r), aux, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
			Canny( aux, aux, 0, 0, 3 );

			/*-----------MOSTRAR FRANJAS--------------------*/

			resultados[i].Franjas[j].imagen=aux;
			resultados[i].Franjas[j].ProyeccionX=proyeccion_eje_X(resultados[i].imagAux(r));
			resultados[i].Franjas[j].convProyX=pasar_patron2(resultados[i].imagAux(r).rows,resultados[i].Franjas[j].ProyeccionX);
			if(resultados[i].Franjas[j].convProyX<convProyXmin)convProyXmin=resultados[i].Franjas[j].convProyX;
			if(resultados[i].Franjas[j].convProyX>convProyXmax)convProyXmax=resultados[i].Franjas[j].convProyX;

			out.str("");
			out << j;
			//cout<<endl<<"franja "<<out.str()<<endl;
			resultados[i].Franjas[j].pCandidatosX=buscar_candidatos_X(resultados[i].Franjas[j].imagen.rows, resultados[i].Franjas[j].ProyeccionX, resultados[i].Franjas[j].imagen.cols);

			Point inicio,fin;
			Mat imagen;
			Mat proyX;
			Mat proyY;
			Scalar medX;
			Scalar stdX;
			Scalar medY;
			Scalar stdY;
			int ancho;
			int alto;
			float relacion;
			int centroX;
			int centroY;
			float valor_ubicacion;
			float valor_relacion;
			float valor_medY;
			float valoracion=0;
			double convProyX;
			cvtColor(resultados[i].Franjas[j].imagen, resultados[i].Franjas[j].imagen, CV_GRAY2BGR);
			for(std::vector<inicio_fin>::size_type k=0;k<resultados[i].Franjas[j].pCandidatosX.size();k++){

				inicio=resultados[i].Franjas[j].pCandidatosX[k].inicio;
				fin=resultados[i].Franjas[j].pCandidatosX[k].fin;
				Rect r(inicio,fin);

				inicio.y+=resultados[i].Franjas[j].inicio.y;
				fin.y+=resultados[i].Franjas[j].inicio.y;

				imagen=resultados[i].Franjas[j].imagen(r);
				proyX=proyeccion_eje_X(imagen);
				proyY=proyeccion_eje_Y(imagen);
				meanStdDev(proyX, medX, stdX);
				meanStdDev(proyY, medY, stdY);
				/*--relacion--*/
				ancho=imagen.cols;
				alto=imagen.rows;
				relacion = (float) alto/ancho;
				if (relacion <= 0.3) {
					valor_relacion = relacion / 0.3;
				} else {
					if (relacion <= 0.6) {
						valor_relacion = 1.1 - ((0.3333333) * relacion);
					} else {
						if (relacion <= 1.2)
							valor_relacion = 1.8 - ((1.5) * relacion);
						else
							valor_relacion = 0;
					}
				}
				/*--ubicacion--*/
				centroX = inicio.x + ceil((float)ancho / 2);
				centroY = inicio.y + ceil((float)alto / 2);
				valor_ubicacion = 1	- (sqrt(pow(centroX - centro_img_x, 2) + pow(centroY - centro_img_y, 2))
								/ distancia_max);
				/*--proyeccY--*/
				valor_medY=0;
				if (medY.val[0] <= 10.0) {
					valor_medY = 0;
				} else
					if (medY.val[0] <= 17) {
						valor_medY=(medY.val[0] -10)/ 7;

					} else {
						if (medY.val[0] <= 26.0) valor_medY = 1;
						else {
							if (medY.val[0] <= 30.0){
								valor_medY=1-(medY.val[0] -26)/4;
							}
						}
					}


				//valoracion = (0.4 * valor_ubicacion) + (0.2 * valor_relacion)+(0.4 * valor_medY);

				promedio promProyY=buscar_promedios(proyY,3,0);

				convProyX=pasar_patron2(alto, proyX);

				Candidatos.push_back({i,j,k,inicio, fin,imagen,proyX,proyY,convProyX,resultados[i].Franjas[j].convProyX,medX.val[0],medY.val[0],stdX.val[0],stdY.val[0],ancho, alto, relacion,centroX,centroY,promProyY,valor_ubicacion,valor_relacion,valor_medY,valoracion});
			}

			//Proyeccion
			Point puntos_proy_pat_x[resultados[i].Franjas[j].imagen.cols][1];
			for (int k = 0; k < resultados[i].Franjas[j].imagen.cols; k++) {
				puntos_proy_pat_x[0][k] = Point(k, resultados[i].Franjas[j].ProyeccionX.at<float>(0, k));
			}
			const Point* pppx[1] = { puntos_proy_pat_x[0] };
			int nppx[] = { resultados[i].Franjas[j].imagen.cols };
			polylines(resultados[i].Franjas[j].imagen, pppx, nppx, 1, false, Scalar(0, 0, 255), 2);


			resultados[i].Franjas[j].imagen.copyTo(resultados[i].imagFranjasY(r));

			//candidatos X

			//imshow("Franjas "+out.str(),resultados[i].Franjas[j].imagen);

			/*-----------MOSTRAR FRANJAS--------------------*/


			resultados[i].ProyeccionX.push_back(proyeccion_eje_X(resultados[i].imagAux(r)));
			resultados[i].pCandidatosX.clear();
			}

		//cvWaitKey();


		//si saco el pasar_patron debo calcular proyeccion_eje_X solamente para la franja elegida
		/*2.5 Correlacion con patrón de patente*/
		resultados[i].resConvPatron=pasar_patron(resultados[i].pCandidatosY,resultados[i].ProyeccionX); //demora de 0.015 a 0.02 [s] mas

		/*2.6 Selección de la mejor franja candidata*/
		resultados[i].mejorCandidatoY=seleccionar_proyeccion_Y(resultados[i].resConvPatron);

		/*2.7 Busqueda de candidatos a patente en el eje X dentro de la Franja Y seleccionada*/


		if (resultados[i].mejorCandidatoY != -1) {

			resultados[i].altoCandY = resultados[i].pCandidatosY[resultados[i].mejorCandidatoY].fin.y
							- resultados[i].pCandidatosY[resultados[i].mejorCandidatoY].inicio.y;
			resultados[i].pCandidatosX=buscar_candidatos_X(resultados[i].altoCandY, resultados[i].ProyeccionX[resultados[i].mejorCandidatoY], resultados[i].imagAux.cols);


			for (std::vector<int>::size_type j = 0; j != resultados[i].pCandidatosX.size();
					j++) {
				resultados[i].pCandPatente.push_back(
						{ Point(resultados[i].pCandidatosX[j].inicio.x,
								resultados[i].pCandidatosY[resultados[i].mejorCandidatoY].inicio.y), Point(
										resultados[i].pCandidatosX[j].fin.x,
										resultados[i].pCandidatosY[resultados[i].mejorCandidatoY].fin.y) });
				resultados[i].indiceXCandPatente.push_back(resultados[i].mejorCandidatoY);
			}
		} else {
			error=-2;
		}



		/*2.8 organizado candidatos de patente y selección del mejor candidato*/
		resultados[i].mejorCandidato=organizar_candidatos(resultados[i].pCandPatente, resultados[i].imagAux.cols, resultados[i].imagAux.rows);


		/*dibujar pasos del procesamiento*/
			for (std::vector<int>::size_type j = 0; j != resultados[i].pCandidatosY.size();j++) {
				rectangle(resultados[i].imagFranjasY, resultados[i].pCandidatosY[j].inicio, resultados[i].pCandidatosY[j].fin,
						Scalar(50, 255, 0), 2, 1);
			}



			Point puntos_proy_y[1][resultados[i].imagFranjasY.rows];
			for (int j = 0; j < resultados[i].imagFranjasY.rows; j++) {
					puntos_proy_y[0][j] = Point(resultados[i].ProyeccionY.at<float>(0, j), j);
				}

			const Point* ppy[1] = { puntos_proy_y[0] };
			int npy[] = { resultados[i].imagFranjasY.rows };
			polylines(resultados[i].imagFranjasY, ppy, npy, 1, false, Scalar(255, 0, 0), 2);


			line(resultados[i].imagFranjasY, Point(umbralProyY, 0), Point(umbralProyY, resultados[i].imagFranjasY.rows),
					Scalar(0, 255,255), 2);


		/*2.9  inicializar imagen patente*/

		if (resultados[i].mejorCandidato != -1) {
				float offsetx = 0.1 * (resultados[i].pCandPatente[resultados[i].mejorCandidato].fin.x
						- resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.x);
				float offsety = 0.4 * (resultados[i].pCandPatente[resultados[i].mejorCandidato].fin.y
						- resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.y);
				float inicio_x = resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.x - offsetx;
				float inicio_y = resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.y - offsety;
				if (inicio_x < 0) {
					offsetx = 0;
					inicio_x = resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.x;
				}
				if (inicio_y < 0) {
					offsety = 0;
					inicio_y = resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.y;
				}
				float ancho = resultados[i].pCandPatente[resultados[i].mejorCandidato].fin.x
				- resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.x + (2 * offsetx);
				float alto = resultados[i].pCandPatente[resultados[i].mejorCandidato].fin.y
				- resultados[i].pCandPatente[resultados[i].mejorCandidato].inicio.y + (2 * offsety);

				if (inicio_x+ancho > resultados[i].imagAux.cols) {
					ancho = resultados[i].imagAux.cols-inicio_x;
				}
				if (inicio_y+alto > resultados[i].imagAux.rows) {
					alto = resultados[i].imagAux.rows-inicio_y;
				}
				//res->imgResultado.release();

				//resultados[i].imagPatOrig.push_back(imagenEntrada(Rect(inicio_x, inicio_y, ancho,alto)));
				resultados[i].imagPatOrig=(imagenEntrada(Rect(inicio_x, inicio_y, ancho,alto)));
				//imagenEntrada(Rect(inicio_x, inicio_y, ancho,alto)).copyTo(imagPatOrig);
				rectangle(resultados[i].imagFranjasY,Rect(inicio_x, inicio_y, ancho,alto),Scalar(255, 0, 0), 2, 2);


		}



		//resultados[i].imagFranjasYEnmasc=((Mat)enmascarar_franjas(imagenEntrada,resultados[i].pCandidatosY));


	} //fin de pasadas


	organizar_candidatos2();
	//return(error);
	return(candidatosPrincipales);
}


void Localizador::mostrar_pasos(){
	int x=0;
	int y=0;

	/*----------------IMAGEN BUSQUEDA TOP-HAT Y BLACK-HAT------*/
	int alto_imagen=250;
//	Mat aux=imagenEntrada;
//	aux=concatenar_Mat_H(aux,resultados[0].imagFranjasY,alto_imagen);
//	if(resultados[0].imagPatOrig.data){
//		aux=concatenar_Mat_H(aux,resultados[0].imagPatOrig,alto_imagen);
//	}
//
//	imshow("TOP-HAT Y BLACK-HAT",aux);
//	moveWindow("TOP-HAT Y BLACK-HAT", 0, 0);

	/*----------------IMAGEN BUSQUEDA TOP-HAT------*/
	Mat aux;
	resultados[1].imgEntrada.copyTo(aux);
	aux=concatenar_Mat_H(aux,resultados[1].imagFranjasY,alto_imagen);
	imwrite("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/Para mostrar/Candidatos_TOP-HAT.jpg",resultados[1].imgSalida);
//	if(resultados[1].imagPatOrig.data){
//		aux=concatenar_Mat_H(aux,resultados[1].imagPatOrig,alto_imagen);
//	}
	aux=concatenar_Mat_H(aux,resultados[1].imgSalida,alto_imagen);

	imshow("TOP-HAT",aux);
	moveWindow("TOP-HAT", x, y);
	y=y+aux.rows+40;

	/*----------------IMAGEN BUSQUEDA BLACK-HAT------*/
	aux=resultados[2].imgEntrada;
	aux=concatenar_Mat_H(aux,resultados[2].imagFranjasY,alto_imagen);
	imwrite("D:/DISCO E/Facultad/PROYECTO FINAL DE CARRERA/TRABAJO FINAL/ Corpus de Imagenes/TODO/Para mostrar/Candidatos_BLACK-HAT.jpg",resultados[2].imgSalida);
//	if(resultados[2].imagPatOrig.data){
//		aux=concatenar_Mat_H(aux,resultados[2].imagPatOrig,alto_imagen);
//	}
	aux=concatenar_Mat_H(aux,resultados[2].imgSalida,alto_imagen);
	imshow("BLACK-HAT",aux);
	moveWindow("BLACK-HAT", x, y);
	y=y+aux.rows+40;
	Mat dest;

}

bool verifySizes(RotatedRect mr){

	    float error=0.4;
	    //Spain car plate size: 52x11 aspect 4,7272
	    float aspect=3.0769;
	    //Set a min and max area. All other patchs are discarded
	    int min= 15*aspect*15; // minimum area
	    int max= 125*aspect*125; // maximum area
	    //Get only patchs that match to a respect ratio.
	    float rmin= aspect-aspect*error;
	    float rmax= aspect+aspect*error;

	    int area= mr.size.height * mr.size.width;
	    float r= (float)mr.size.width / (float)mr.size.height;
	    if(r<1)
	        r= (float)mr.size.height / (float)mr.size.width;

	    if(( area < min || area > max ) || ( r < rmin || r > rmax )){
	        return false;
	    }else{
	        return true;
	    }

	}

Mat Localizador::busquedaPorContornos(Mat imagMod){
	Mat imagAux;
	blur(imagMod, imagAux, Size(5,5));
	Sobel(imagAux, imagAux, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	threshold(imagAux, imagAux, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 3) );
	morphologyEx(imagAux, imagAux, CV_MOP_CLOSE, element);
	 vector< vector< Point> > contours;
			    findContours(imagAux,
			            contours, // a vector of contours
			            CV_RETR_EXTERNAL, // retrieve the external contours
			            CV_CHAIN_APPROX_NONE); // all pixels of each contours

			    //Start to iterate to each contour founded
			    vector<vector<Point> >::iterator itc= contours.begin();
			    vector<RotatedRect> rects;

			    //Remove patch that are no inside limits of aspect ratio and area.
			    while (itc!=contours.end()) {
			        //Create bounding rect of object
			        RotatedRect mr= minAreaRect(Mat(*itc));
			        if( !verifySizes(mr)){
			            itc= contours.erase(itc);
			        }else{
			            ++itc;
			            rects.push_back(mr);
			        }
			    }
			// Draw blue contours on a white image

			cvtColor(imagAux, imagAux, CV_GRAY2BGR);
			cv::drawContours(imagAux,contours,
					-1, // draw all contours
					cv::Scalar(255,0,0), // in blue
					2); // with a thickness of 1

	return(imagAux);
}


Mat Localizador::busquedaPorContornos2(Mat imagMod){
	Mat imagAux;
	imagMod.copyTo(imagAux);

	 vector< vector< Point> > contours;
			    findContours(imagAux,
			            contours, // a vector of contours
			            CV_RETR_EXTERNAL, // retrieve the external contours
			            CV_CHAIN_APPROX_NONE); // all pixels of each contours

			    //Start to iterate to each contour founded
			    vector<vector<Point> >::iterator itc= contours.begin();
			    vector<RotatedRect> rects;

			    //Remove patch that are no inside limits of aspect ratio and area.
			    while (itc!=contours.end()) {
			        //Create bounding rect of object
			        RotatedRect mr= minAreaRect(Mat(*itc));
			        if( !verifySizes(mr)){
			            itc= contours.erase(itc);
			        }else{
			            ++itc;
			            rects.push_back(mr);
			        }
			    }
			// Draw blue contours on a white image

			cvtColor(imagAux, imagAux, CV_GRAY2BGR);
			cv::drawContours(imagAux,contours,
					-1, // draw all contours
					cv::Scalar(255,0,0), // in blue
					2); // with a thickness of 1

	return(imagAux);
}

/*2.1.4 Sacar lineas horizontales*/
Mat Localizador::sacar_lineas_horizontales(Mat imagMod,int largo,int umbralBin2) {
	Mat EEHor;
	Mat imagAux;
	/*---horizontal----*/
	EEHor = getStructuringElement(CV_SHAPE_RECT, cv::Size(largo, 1));
	morphologyEx(imagMod, imagAux, MORPH_TOPHAT, EEHor);
	threshold(imagAux, imagAux, umbralBin2, 255, cv::THRESH_BINARY);

	return (imagAux);
}


/*2.2 Proyección sobre el eje Y*/
Mat Localizador::proyeccion_eje_Y(Mat imagMod) {
	Mat imagAuxInt;
	Mat ProyeccionY;

	imagMod.convertTo(imagAuxInt, CV_32F);
	normalize(imagAuxInt, imagAuxInt, 0, 1, CV_MINMAX);
	reduce(imagAuxInt, ProyeccionY, 1, CV_REDUCE_SUM);
	return(ProyeccionY);
}

Mat Localizador::enmascarar_ruido(Mat ProyeccionY, int ancho, float varAjusteY, int alto_min_cand){
	umbralProyY = varAjusteY * (mean(ProyeccionY).val[0]);
	int indIni=0;
	int indFin=0;
	int altoCandY = 0;
	vector<inicio_fin> pCandidatosY;

	for (int i = 1; i < ProyeccionY.rows; i++) {
		if (ProyeccionY.at<float>(0, i) > umbralProyY) {
			if (ProyeccionY.at<float>(0, i - 1) <= umbralProyY)
				indIni = i; //entra en cresta
		} else {
			if (ProyeccionY.at<float>(0, i - 1) >= umbralProyY) { //sale en cresta
				altoCandY = i - indIni;
				if (altoCandY > alto_min_cand) {
					indFin = i;
					pCandidatosY.push_back(
							{ Point(0, indIni), Point(ancho - 1,
									indFin) });
				}
				indIni = i;
			}
		}
	}
	//return(pCandidatosY);


	return(ProyeccionY);
}



Mat Localizador::enmascarar_franjas(Mat imagMod, vector<inicio_fin> pCandidatosY){

	Mat mascara=Mat::zeros(imagMod.rows, imagMod.cols, CV_8U);
	int alto, ancho;

	for (std::vector<int>::size_type i = 0; i != pCandidatosY.size();i++) {
		alto=pCandidatosY[i].fin.y-pCandidatosY[i].inicio.y;
		ancho=pCandidatosY[i].fin.x-pCandidatosY[i].inicio.x;
		imagMod(Rect(0, pCandidatosY[i].inicio.y,ancho, alto)).copyTo(mascara(Rect(0, pCandidatosY[i].inicio.y,ancho, alto)));
	}
	return(mascara);
}

/*2.3 Busqueda de maximos en la proyección Y*/
vector<inicio_fin> Localizador::buscar_candidatos_Y(Mat ProyeccionY, int ancho, float varAjusteY, int alto_min_cand) {
	umbralProyY = varAjusteY * (mean(ProyeccionY).val[0]);
	int indIni=0;
	int indFin=0;
	int altoCandY = 0;
	vector<inicio_fin> pCandidatosY;

	for (int i = 1; i < ProyeccionY.rows; i++) {
		if (ProyeccionY.at<float>(0, i) > umbralProyY) {
			if (ProyeccionY.at<float>(0, i - 1) <= umbralProyY)
				indIni = i; //entra en cresta
		} else {
			if (ProyeccionY.at<float>(0, i - 1) >= umbralProyY) { //sale en cresta
				altoCandY = i - indIni;
				if (altoCandY > alto_min_cand) {
					indFin = i;
					pCandidatosY.push_back(
							{ Point(0, indIni), Point(ancho - 1,
									indFin) });
				}
				indIni = i;
			}
		}
	}
	return(pCandidatosY);
}


/*2.4 Proyecciones sobre el eje X*/
Mat Localizador::proyeccion_eje_X(Mat img) {
	img.convertTo(img, CV_32F);
	normalize(img, img, 0, 1, CV_MINMAX);
	Mat proy_x(1, img.cols, CV_32FC1, 1);
	reduce(img, proy_x, 0, CV_REDUCE_SUM);
	return proy_x;
}

/*2.5 Correlacion con patrón de patente*/
vector<double> Localizador::pasar_patron(vector<inicio_fin> pCandidatosY, vector<Mat> ProyeccionX) {
	/*Se crea un patron para cada franja candidata según el alto de la misma*/
	/*Cada franja es convolucionado con su respectivo patron para ver si hay algun punto de maximo*/
	int ancho_letra;
	int ancho_espacio_menor;
	int ancho_espacio_mayor;
	int largo_total;
	Mat patron;
	vector<double> resConvPatron;

	ancho_letra = 0;
	ancho_espacio_menor = 0;
	ancho_espacio_mayor = 0;
	largo_total = 0;
	patron = Mat::zeros(1, 1, CV_32FC1);



	for (std::vector<int>::size_type j = 0; j < ProyeccionX.size(); j++) {
		int alto = pCandidatosY[j].fin.y - pCandidatosY[j].inicio.y;
		ancho_letra = round(alto * 0.5);
		ancho_espacio_menor = round(alto * 0.11);
		ancho_espacio_mayor = round(alto * 0.72);
		largo_total = (ancho_letra * 6) + (ancho_espacio_menor * 4)
				+ (ancho_espacio_mayor);
		patron = Mat::zeros(1, largo_total, CV_32FC1);
		patron.at<float>(0, ancho_letra - 1) = 255;
		patron.at<float>(0, ancho_letra + ancho_espacio_menor) = 255;
		patron.at<float>(0, (2 * ancho_letra) + ancho_espacio_menor - 1) =
				255;
		patron.at<float>(0, (2 * ancho_letra) + (2 * ancho_espacio_menor)) =
				255;
		patron.at<float>(0,
				(3 * ancho_letra) + (2 * ancho_espacio_menor) - 1) = 255;
		patron.at<float>(0,
				(3 * ancho_letra) + (2 * ancho_espacio_menor)
						+ ancho_espacio_mayor) = 255;
		patron.at<float>(0,
				(4 * ancho_letra) + (2 * ancho_espacio_menor)
						+ ancho_espacio_mayor - 1) = 255;
		patron.at<float>(0,
				(4 * ancho_letra) + (3 * ancho_espacio_menor)
						+ ancho_espacio_mayor) = 255;
		patron.at<float>(0,
				(5 * ancho_letra) + (3 * ancho_espacio_menor)
						+ ancho_espacio_mayor - 1) = 255;
		patron.at<float>(0,
				(5 * ancho_letra) + (4 * ancho_espacio_menor)
						+ ancho_espacio_mayor) = 255;
		patron.at<float>(0,
				(6 * ancho_letra) + (4 * ancho_espacio_menor)
						+ ancho_espacio_mayor - 1) = 255;
		int estado = 255;
		for (int i = 0; i < largo_total-1; i++) {
			if (patron.at<float>(0, i) == 255) {
				if (estado)
					estado = 0;
				else
					estado = 255;
				i++;
				patron.at<float>(0, i) = estado;
			}
			patron.at<float>(0, i) = estado;
		}

		Mat convolucion, autoconvolucion;

		double maxVal = 0;
		double minVal = 0;
		double maxValAuto = 0;
		double minValAuto = 0;

		filter2D(ProyeccionX[j], convolucion, CV_32FC1, patron);
		filter2D(patron, autoconvolucion, CV_32FC1, patron);

		cv::minMaxLoc(convolucion, &minVal, &maxVal, 0);
		cv::minMaxLoc(autoconvolucion, &minValAuto, &maxValAuto, 0);

		resConvPatron.push_back(maxVal / maxValAuto);

		Mat patron2 = Mat::zeros(alto, largo_total-1, CV_32FC1);
		for (int i = 0; i < largo_total-1; i++){
			for (int j = 0; j < alto; j++){
				patron2.at<float>(j, i)= patron.at<float>(0, i);
			}
		}
		/*imwrite(direccion+"img_Patron" + ".png",patron);
		imshow("patron",patron);
		imwrite(direccion+"img_Patron2" + ".png",patron2);
		imshow("patron2",patron2);*/


		}
		return(resConvPatron);
	}

double Localizador::pasar_patron2(int alto, Mat ProyeccionX) {
	/*Se crea un patron para cada franja candidata según el alto de la misma*/
	/*Cada franja es convolucionado con su respectivo patron para ver si hay algun punto de maximo*/

	if(ProyeccionX.channels()==3) cvtColor(ProyeccionX, ProyeccionX, CV_BGR2GRAY);

	int ancho_letra;
	int ancho_espacio_menor;
	int ancho_espacio_mayor;
	int largo_total;
	Mat patron;
	double resConvPatron;

	ancho_letra = 0;
	ancho_espacio_menor = 0;
	ancho_espacio_mayor = 0;
	largo_total = 0;
	patron = Mat::zeros(1, 1, CV_32FC1);


	ancho_letra = round(alto * 0.5);
	ancho_espacio_menor = round(alto * 0.11);
	ancho_espacio_mayor = round(alto * 0.72);
	largo_total = (ancho_letra * 6) + (ancho_espacio_menor * 4)
			+ (ancho_espacio_mayor);
	patron = Mat::zeros(1, largo_total, CV_32FC1);
	patron.at<float>(0, ancho_letra - 1) = 255;
	patron.at<float>(0, ancho_letra + ancho_espacio_menor) = 255;
	patron.at<float>(0, (2 * ancho_letra) + ancho_espacio_menor - 1) =
			255;
	patron.at<float>(0, (2 * ancho_letra) + (2 * ancho_espacio_menor)) =
			255;
	patron.at<float>(0,
			(3 * ancho_letra) + (2 * ancho_espacio_menor) - 1) = 255;
	patron.at<float>(0,
			(3 * ancho_letra) + (2 * ancho_espacio_menor)
					+ ancho_espacio_mayor) = 255;
	patron.at<float>(0,
			(4 * ancho_letra) + (2 * ancho_espacio_menor)
					+ ancho_espacio_mayor - 1) = 255;
	patron.at<float>(0,
			(4 * ancho_letra) + (3 * ancho_espacio_menor)
					+ ancho_espacio_mayor) = 255;
	patron.at<float>(0,
			(5 * ancho_letra) + (3 * ancho_espacio_menor)
					+ ancho_espacio_mayor - 1) = 255;
	patron.at<float>(0,
			(5 * ancho_letra) + (4 * ancho_espacio_menor)
					+ ancho_espacio_mayor) = 255;
	patron.at<float>(0,
			(6 * ancho_letra) + (4 * ancho_espacio_menor)
					+ ancho_espacio_mayor - 1) = 255;
	int estado = 255;
	for (int i = 0; i < largo_total-1; i++) {
		if (patron.at<float>(0, i) == 255) {
			if (estado)
				estado = 0;
			else
				estado = 255;
			i++;
			patron.at<float>(0, i) = estado;
		}
		patron.at<float>(0, i) = estado;
	}

	/*convolucion*/
	Mat convolucion, autoconvolucion, convoMT, convoMTAuto;

	double maxVal = 0;
	double minVal = 0;
	double maxValAuto = 0;
	double minValAuto = 0;

	filter2D(ProyeccionX, convolucion, CV_32FC1, patron);
	filter2D(patron, autoconvolucion, CV_32FC1, patron);

	cv::minMaxLoc(convolucion, &minVal, &maxVal, 0);
	cv::minMaxLoc(autoconvolucion, &minValAuto, &maxValAuto, 0);

	resConvPatron=maxVal / maxValAuto *100;

	Mat patron2 = Mat::zeros(alto, largo_total-1, CV_32FC1);
	for (int i = 0; i < largo_total-1; i++){
		for (int j = 0; j < alto; j++){
			patron2.at<float>(j, i)= patron.at<float>(0, i);
		}
	}
	/*imwrite(direccion+"img_Patron" + ".png",patron);
	imshow("patron",patron);
	imwrite(direccion+"img_Patron2" + ".png",patron2);
	imshow("patron2",patron2);*/

//	matchTemplate( ProyeccionX, patron, convoMT, 5 );
//	//normalize( convoMT, convoMT, 0, 1, NORM_MINMAX, -1, Mat() );
//	cv::minMaxLoc(convoMT, &minVal, &maxVal, 0);
//
//	cout<<"maxVal: "<<maxVal<<endl;
//	resConvPatron=maxVal; //1-matchTemplate modo 0

	return(resConvPatron);
	}


/*2.6 Selección de la mejor franja candidata*/
int Localizador::seleccionar_proyeccion_Y(vector<double> resConvPatron) {
	int mejorCandidatoY = -1;
	float valor_patron = 0;
	vector<double> resConvPatronOrd = resConvPatron;
	sort(resConvPatronOrd.begin(), resConvPatronOrd.end());

	for (std::vector<double>::size_type i = 0; i != resConvPatron.size();
			i++) {
		if (resConvPatron[i]
				== resConvPatronOrd[resConvPatronOrd.size() - 1]) {
			mejorCandidatoY = i;
			break;
		}
	}

	return(mejorCandidatoY);
}

/*2.7 Busqueda de candidatos a patente en el eje X dentro de la Franja Y seleccionada*/
vector<inicio_fin> Localizador::buscar_candidatos_X(int alto, Mat ProyeccionX, int ancho) {
	float varAjusteX;
	varAjusteX = 5;
	float umbralProyX=0;

	umbralProyX = varAjusteX * (mean(ProyeccionX).val[0]);
	int indIni=0;
	int indFin=0;
	vector<inicio_fin> pCandidatosX;
	Mat roiPatImag;

	for (int i = 1; i < ancho; i++) {
		float pi = ProyeccionX.at<float>(0, i);				//valor en i
		float pia = ProyeccionX.at<float>(0, i - 1);	//valor en i-1
		if (pi > umbralProyX) {
			if (pia <= umbralProyX) {					//entra en cresta
				indIni = i;						//inicio de nuevo candidato
			}
		} else {
			if (pia > umbralProyX) { 						//sale de cresta

				indFin = i;
				if (!pCandidatosX.empty()) {
					int fin_cand_ant =
							pCandidatosX[(int) pCandidatosX.size() - 1].fin.x;
					if ((indIni - fin_cand_ant) > ceil(float (alto * 1.2))) {

						/*--------*/
						pCandidatosX.push_back(
								{ Point(indIni, 0), Point(indFin,
										alto - 1) });
						/*--------*/
					} else {
						pCandidatosX[(int) pCandidatosX.size() - 1].fin.x =
								indFin;
					}
				} else {

					pCandidatosX.push_back(
							{ Point(indIni, 0), Point(indFin,
									alto - 1) });

				}
			}
		}
	}

	for(std::vector<inicio_fin>::size_type i=0;i<pCandidatosX.size();i++){
		if(pCandidatosX[i].fin.x-pCandidatosX[i].inicio.x < alto){
			pCandidatosX.erase(pCandidatosX.begin()+i);
			i=i-1;
		}
	}


	return(pCandidatosX);
}

;


int Localizador::organizar_candidatos(vector<inicio_fin> pCandPatente, int ancho, int alto) {

		float relacion;
		float valor_relacion = 0;
		float valor_ubicacion = 0;

		float centro_img_x = (ancho) / 2;
		float centro_img_y = (alto) / 2;
		float centro_cand_x;
		float centro_cand_y;
		float distancia_max = sqrt(pow(centro_img_x, 2) + pow(centro_img_y, 2));
		float valoracion = 0;
		int elegido_aux = -1;
		float valor_elegido = 0;

//		circle(imagMod, Point(centro_img_x, centro_img_y), 4, Scalar(0, 0, 255),
//				-1, 4);

		for (std::vector<int>::size_type i = 0; i != pCandPatente.size(); i++) {
			/*--------relacion------------*/
			alto = pCandPatente[i].fin.y - pCandPatente[i].inicio.y;
			ancho = pCandPatente[i].fin.x - pCandPatente[i].inicio.x;
			relacion = alto / ancho;
			if (relacion <= 0.3) {
				valor_relacion = relacion / 0.3;
			} else {
				if (relacion <= 0.6) {
					valor_relacion = 1.1 - ((0.3333333) * relacion);
				} else {
					if (relacion <= 1.2)
						valor_relacion = 1.8 - ((1.5) * relacion);
					else
						valor_relacion = 0;
				}
			}
			/*---------distancia----------*/
			centro_cand_x = pCandPatente[i].inicio.x + (ancho / 2);
			centro_cand_y = pCandPatente[i].inicio.y + (alto / 2);
			valor_ubicacion = 1
					- (sqrt(
							pow(centro_cand_x - centro_img_x, 2)
									+ pow(centro_cand_y - centro_img_y, 2))
							/ distancia_max);

			/*---------valoracion total-------*/

			valoracion = (0.4 * valor_ubicacion) + (0.6 * valor_relacion);

//			circle(imagMod, Point(centro_cand_x, centro_cand_y), 4,
//					Scalar(0, 255, 255), -1, 1);
//			line(imagMod, Point(centro_cand_x, centro_cand_y),
//					Point(centro_img_x, centro_img_y), Scalar(120, 120, 120),
//					1.7);
			if (valoracion > valor_elegido) {
				valor_elegido = valoracion;
				elegido_aux = (int) i;
			}
		}

		return(elegido_aux);

	}

void Localizador::organizar_candidatos2() {

//		cout<<"DESORDENADO"<<endl;
		for (std::vector<candidato>::size_type j = 0; j != Candidatos.size();j++) {
//			out.str("");
//			out << j;
//			cout<<Candidatos[j].relacion<<";"<<Candidatos[j].mediaPX<<";"<<Candidatos[j].desvEstPX;
//			cout<<";"<<Candidatos[j].mediaPY<<";"<<Candidatos[j].desvEstPY<<";";

//			valoracion = (0.4 * valor_ubicacion) + (0.6 * valor_relacion);

//			cout<<"mediaPY: "<<Candidatos[j].mediaPY<<"; valor_medY: "<<Candidatos[j].valor_medY<<" ; "<<endl;
//			cout<<Candidatos[j].valor_ubicacion<<" ; "<<Candidatos[j].valor_relacion<<" ; "<<Candidatos[j].valor_medY<<" ; "<<Candidatos[j].valoracion<<endl;

			Candidatos[j].convProyFranjaX=(Candidatos[j].convProyFranjaX-convProyXmin)/(convProyXmax-convProyXmin);

			Candidatos[j].valoracion = (0.4 * Candidatos[j].valor_ubicacion) + (0.2 * Candidatos[j].valor_relacion)+
					(0.4 * Candidatos[j].valor_medY)+(1.0 * Candidatos[j].convProyFranjaX);

			rectangle(resultados[Candidatos[j].pasada].imagFranjasY,Candidatos[j].inicio,Candidatos[j].fin,Scalar(29, 143, 248),4);
		}

		sort(Candidatos.rbegin(),Candidatos.rend());

//		cout<<"ORDENADO"<<endl;
		for (std::vector<candidato>::size_type j = 0; j != Candidatos.size();j++) {
			if(j==0){
				candidatosPrincipales.push_back({Candidatos[j].inicio,Candidatos[j].fin});
				rectangle(resultados[Candidatos[j].pasada].imgSalida,Candidatos[j].inicio,Candidatos[j].fin,Scalar(0, 0, 255),4);
				putText(resultados[Candidatos[j].pasada].imgSalida, "1", cvPoint(Candidatos[j].centroX,Candidatos[j].centroY+Candidatos[j].alto+30), FONT_HERSHEY_COMPLEX, 1.5, cvScalar(0, 0, 255), 2, CV_AA);
			}
			if(j==1){
				candidatosPrincipales.push_back({Candidatos[j].inicio,Candidatos[j].fin});
				rectangle(resultados[Candidatos[j].pasada].imgSalida,Candidatos[j].inicio,Candidatos[j].fin,Scalar(0, 0, 255),4);
				putText(resultados[Candidatos[j].pasada].imgSalida, "2", cvPoint(Candidatos[j].centroX,Candidatos[j].centroY+Candidatos[j].alto+30), FONT_HERSHEY_COMPLEX, 1.5, cvScalar(0, 0, 255), 2, CV_AA);
			}
			if(j==2){
				candidatosPrincipales.push_back({Candidatos[j].inicio,Candidatos[j].fin});
				rectangle(resultados[Candidatos[j].pasada].imgSalida,Candidatos[j].inicio,Candidatos[j].fin,Scalar(0, 0, 255),4);
				putText(resultados[Candidatos[j].pasada].imgSalida, "3", cvPoint(Candidatos[j].centroX,Candidatos[j].centroY+Candidatos[j].alto+30), FONT_HERSHEY_COMPLEX, 1.5, cvScalar(0, 0, 255), 2, CV_AA);
			}
			if(j==3){
				candidatosPrincipales.push_back({Candidatos[j].inicio,Candidatos[j].fin});
				rectangle(resultados[Candidatos[j].pasada].imgSalida,Candidatos[j].inicio,Candidatos[j].fin,Scalar(0, 0, 255),4);
				putText(resultados[Candidatos[j].pasada].imgSalida, "4", cvPoint(Candidatos[j].centroX,Candidatos[j].centroY+Candidatos[j].alto+30), FONT_HERSHEY_COMPLEX, 1.5, cvScalar(0, 0, 255), 2, CV_AA);
			}
			//cout<<"pasada: "<<Candidatos[j].pasada<<"  franja: "<<Candidatos[j].franja<<"  candidato: "<<Candidatos[j].candX<<"  valoracion: "<< Candidatos[j].valoracion;
			//cout<<Candidatos[j].valor_ubicacion<<" ; "<<Candidatos[j].valor_relacion<<" ; "<<Candidatos[j].valor_medY<<" ; "<<Candidatos[j].valoracion<<endl;
			//cout<<"  convProyX: "<<Candidatos[j].convProyX<<"  convProyFranjaX: "<<Candidatos[j].convProyFranjaX<<endl;
		}

	}



//AGREGAR MAS CONTROLES
void Localizador::postproceso(Mat imgPatente){
	if(imgPatente.cols<10)error=-3;

	//svm_iniciar_clasificador();


}


Mat Localizador::obtener_candidato(int cand){
	Mat salida=resultados[0].imgEntrada;

	float offsetx = 0.1 * (Candidatos[cand].fin.x - Candidatos[cand].inicio.x);
	float offsety = 0.5 * (Candidatos[cand].fin.y - Candidatos[cand].inicio.y);
	float inicio_x = Candidatos[cand].inicio.x - offsetx;
	float inicio_y = Candidatos[cand].inicio.y - offsety;
	if (inicio_x < 0) {
		offsetx = 0;
		inicio_x = Candidatos[cand].inicio.x;
	}
	if (inicio_y < 0) {
		offsety = 0;
		inicio_y = Candidatos[cand].inicio.y;
	}
	float ancho = Candidatos[cand].fin.x
	- Candidatos[cand].inicio.x + (2 * offsetx);
	float alto = Candidatos[cand].fin.y	- Candidatos[cand].inicio.y + (2 * offsety);

	if (inicio_x+ancho > imagenEntrada.cols) {
		ancho = imagenEntrada.cols-inicio_x;
	}
	if (inicio_y+alto > imagenEntrada.rows) {
		alto = imagenEntrada.rows-inicio_y;
	}


	salida=(imagenEntrada(Rect(inicio_x, inicio_y, ancho,alto)));


	//Candidatos[cand].imagen.copyTo(salida);
//
//
//
//	switch (pasada) {
//					case 1: {
//						if(resultados[1].mejorCandidato!=-1)resultados[1].imagPatOrig.copyTo(salida);
//						break;
//					}
//					case 2: {
//						if(resultados[2].mejorCandidato!=-1)resultados[2].imagPatOrig.copyTo(salida);
//						break;
//					}
//
//				}
//
//	pasada++;
	return(salida);
}



promedio Localizador::buscar_promedios(Mat valores, int rangLibertad, int mostrar){
	vector<promedio> vp;
	vector<float> valoresaux;
	for(int i=0;i<valores.rows;i++)valoresaux.push_back(valores.at<float>(i,0));

	//ordenar valores
	sort(valoresaux.begin(), valoresaux.end());
	float varAnt=valoresaux[0];
	int cantidad=1;
	//buscar promedios
	for(int i=1;i<valoresaux.size();i++){

		if(valoresaux[i]==varAnt){
			cantidad++;
		}else {
			vp.push_back({cantidad,varAnt});
			cantidad=1;
			varAnt=valoresaux[i];
		}
	}
	vp.push_back({cantidad,varAnt});

	int indMax;
	int CantidadMax=0;
	if(mostrar)cout<<"PROMEDIOS"<<endl;
	for(int i=0;i<vp.size();i++){
		//if(mostrar)cout<<vp[i].promedio<<" "<<vp[i].cantidad<<endl;
		if(vp[i].cantidad>CantidadMax){
			CantidadMax=vp[i].cantidad;
			indMax=i;
		}
	}

	return(vp[indMax]);
}
