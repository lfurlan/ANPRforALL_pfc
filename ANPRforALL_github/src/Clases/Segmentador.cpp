#include "Segmentador.hpp"

Segmentador::Segmentador() {
	paso = 0;
	contRota = 0;
	resultados.codError.error = 0;//-1: procedimiento no finalizado   0: sin error   1: imagen de entrada sin datos
								  //2: error al redimensionar  3: error al hacer Hough  4:no es una patente
								  //5: error al segmentar en Y (sin Franjas HOrizontales Candidatos)
								  //6: error al segmentar en X (sin Franjas Verticales Candidatos)
	resultados.codError.tipoPatente = -1; //-1: Indefinido    0: Patente Vieja   1: Patente Nueva   2: no es una patente
	resultados.mejorpromedioHor= {0,0};
	resultados.mejorpromedioVer= {0,0};
	resultados.giro = 0;
	resultados.anguloLetras = 90;
}

Segmentador::~Segmentador() {
}

codigoError Segmentador::ejecutar(Mat imagenEntrada) {
	paso = 0;
	contRota = 0;
	resultados.Caracteres.clear();
	resultados.codError.error = 0;
	resultados.codError.tipoPatente = -1; //-1: Indefinido    0: Patente Vieja   1: Patente Nueva
	if (!imagenEntrada.data) {
		resultados.codError.error = -1;
		return (resultados.codError);
	}
	paso++; //paso=1;
	imagenEntrada.copyTo(resultados.imagPatOrig);
	if (preprocesar_patente() != 0)
		return (resultados.codError);
	paso++;	//paso=2;
	/*------AMBITO DE BORDES---*/
	if (cambio_perspectiva() != 0)
		return (resultados.codError);
	paso++; //paso=3;
	if (quePatenteEs() != 0)
		return (resultados.codError);
	paso++;	//paso=4
	if (segmentarCaracteres() != 0)
		return (resultados.codError);
	paso++; //paso=5
	if (postproceso() != 0)
		return (resultados.codError);
	paso++; //paso=6
	return (resultados.codError);
}

Mat Segmentador::sacarImagen() {
	Mat result;
	resultados.imagSegmentada.copyTo(result);
	return (result);
}

void Segmentador::mostrar_pasos() {
	int x = 0;
	int y = 0;
	Mat concatenada;
	//IMAGEN ENTRADA
	imshow("SegmImagenEntrada", resultados.imagPatEscalada);
	moveWindow("SegmImagenEntrada", x, y);
	y += resultados.imagPatOrig.rows + 40;
	//BORDES IMAGEN ENTRADA
	imshow("SegmBordes", resultados.imagBordes);
	moveWindow("SegmBordes", x, y);
	y += resultados.imagBordes.rows + 40;
	//BORDES HOUGH
	imshow("SegmHough", resultados.imagBordesHough);
	moveWindow("SegmHough", x, y);
	y += resultados.imagBordesHough.rows + 40;
	//BORDES ROTADA
	imshow("SegmHoughRotacion", resultados.imagBordesRotada0);
	moveWindow("SegmHoughRotacion", x, y);
	y += resultados.imagBordesRotada0.rows + 40;
	//BORDES CORRECCION INCLINACION
	imshow("SegmHoughInclinacion", resultados.imagBordesRotada);
	moveWindow("SegmHoughInclinacion", x, y);
	y = 40;
	x += resultados.imagBordesRotada0.cols + 40;
	//BORDES EJES PARA CORRECCION INCLINACION
	imshow("SegmHoughInclinacion2", resultados.imagBordesRotada2);
	moveWindow("SegmHoughInclinacion2", x, y);
	y += resultados.imagBordesRotada2.rows + 40;
	//PROYECCION Y
	Mat aux;
	resultados.imagBordesRotada.copyTo(aux);
	cvtColor(aux, aux, CV_GRAY2BGR);
	Point puntos_proy_y[1][aux.rows];
	for (int j = 0; j < aux.rows; j++) {
		puntos_proy_y[0][j] = Point(
				resultados.ProyeccionY_patente.at<float>(0, j), j);
	}
	const Point* ppy[1] = { puntos_proy_y[0] };
	int npy[] = { aux.rows };
	polylines(aux, ppy, npy, 1, false, Scalar(255, 0, 0), 1);
	imshow("SegmProyY", aux);
	moveWindow("SegmProyY", x, y);
	y += aux.rows + 40;
	//SELECCION PROYECCION Y
	line(aux, Point(resultados.umbralProyYpat2, 0),
			Point(resultados.umbralProyYpat2, aux.rows), Scalar(0, 0, 255), 1);
	Rect r(resultados.pCandidatosY_patente[ind_mayor_altoGLOBAL1].inicio.x,
			resultados.pCandidatosY_patente[ind_mayor_altoGLOBAL1].inicio.y,
			resultados.imagBordesRotada.cols,
			resultados.pCandidatosY_patente[ind_mayor_altoGLOBAL1].fin.y
					- resultados.pCandidatosY_patente[ind_mayor_altoGLOBAL1].inicio.y);
	rectangle(aux, r, Scalar(0, 255, 0), 1, 1, 0);
	imshow("SegmProyYSeleccion", aux);
	moveWindow("SegmProyYSeleccion", x, y);
	y += aux.rows + 40;
	//SEGMENTACION Y
	imshow("SegmSegY", resultados.imagBordesRotadaSegY);
	moveWindow("SegmSegY", x, y);
	y += resultados.imagBordesRotadaSegY.rows + 40;
	//PROYECCION X Y DETERMINACION DE TIPO PATENTE
	resultados.imagBordesRotadaSegY.copyTo(aux);
	cvtColor(aux, aux, CV_GRAY2BGR);
	Point puntos_proy_pat_x[aux.cols][1];
	for (int i = 0; i < aux.cols; i++) {
		puntos_proy_pat_x[0][i] = Point(i,
				resultados.ProyeccionX_patente_compPatente.at<float>(0, i));
	}
	const Point* pppx[1] = { puntos_proy_pat_x[0] };
	int nppx[] = { aux.cols };
	polylines(aux, pppx, nppx, 1, false, Scalar(255, 0, 0), 1);
	cout << "resultados.umbralProyXpat: " << resultados.umbralProyXpat << endl;
	line(aux, Point(0, umbralTipoPatente), Point(aux.cols, umbralTipoPatente),
			Scalar(0, 0, 255), 1);
	rectangle(aux, Rect(Point(iInicial, 0), Point(iFinal, aux.rows)),
			Scalar(0, 255, 0), 2, 1, 0);
	Mat aux2(aux.rows * 1.5, aux.cols, aux.type(), Scalar(0, 0, 0));
	aux.copyTo(aux2(Rect(0, 0, aux.cols, aux.rows)));
	string numeracionPatente;
	switch (resultados.codError.tipoPatente) {
	case 0:			//patente vieja
		numeracionPatente = "Patente Argentina";
		break;
	case 1:			//patente nueva
		numeracionPatente = "Patente Mercosur";
		break;
	default:		//patente indefinida -->cortar proceso
		break;
	}
	cv::putText(aux2, numeracionPatente,
			Point(ceil(2 * aux2.cols / 18), aux2.rows - 10),
			FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 0), 1, 0.1);

	imshow("SegmSegYProyX", aux2);
	moveWindow("SegmSegYProyX", x, y);
	y = 40;
	x += aux2.cols + 40;
	//PATENTE GRIS ROTADA Y SEGMENTADA EN Y
	imshow("PatenteRotadaSegmY1", resultados.imagGrisRotadaSegmY);
	moveWindow("PatenteRotadaSegmY1", x, y);
	y += resultados.imagGrisRotadaSegmY.rows + 40;
	//OPERACION MORFOLOGICA (TOP-HAT O BLACK-HAT)
	imshow("imagBinariaRotadaSegY", resultados.imagBinariaRotada);
	moveWindow("imagBinariaRotadaSegY", x, y);
	y += resultados.imagBinariaRotadaSegY.rows + 40;
	//PROYECCION Y SOBRE IMAGEN BINARIA
	resultados.imagBinariaRotada.copyTo(aux2);
	cvtColor(aux2, aux2, CV_GRAY2BGR);
	Point puntos_proy_y2[1][aux2.rows];
	for (int j = 0; j < aux2.rows; j++) {
		puntos_proy_y2[0][j] = Point(
				resultados.ProyeccionY_patente2.at<float>(0, j), j);
	}
	const Point* ppy2[1] = { puntos_proy_y2[0] };
	int npy2[] = { aux2.rows };
	polylines(aux2, ppy2, npy2, 1, false, Scalar(255, 0, 0), 1);
	Rect r2(resultados.pCandidatosY_patente2[ind_mayor_altoGLOBAL2].inicio.x,
			resultados.pCandidatosY_patente2[ind_mayor_altoGLOBAL2].inicio.y,
			resultados.imagBordesRotada.cols,
			resultados.pCandidatosY_patente2[ind_mayor_altoGLOBAL2].fin.y
					- resultados.pCandidatosY_patente2[ind_mayor_altoGLOBAL2].inicio.y);
	rectangle(aux2, r2, Scalar(0, 255, 0), 1, 1, 0);
	line(aux2, Point(resultados.umbralProyYpat2, 0),
			Point(resultados.umbralProyYpat2, aux2.rows), Scalar(0, 0, 255), 1);
	imshow("PatenteProyeccionY", aux2);
	moveWindow("PatenteProyeccionY", x, y);
	y += resultados.imagBinariaRotada.rows + 40;
	//SEGMENTACION Y SOBRE IMAGEN BINARIA
	imshow("SegmBinariaReSegmY", resultados.imagBinariaRotadaSegY);
	moveWindow("SegmBinariaReSegmY", x, y);
	y += resultados.imagBinariaRotadaSegY.rows + 40;
	//PROYECCION X SOBRE IMAGEN BINARIA
	resultados.imagBinariaRotadaSegY.copyTo(aux);
	cvtColor(aux, aux, CV_GRAY2BGR);
	Point puntos_proy_pat_x1[aux.cols][1];
	for (int i = 0; i < aux.cols; i++) {
		puntos_proy_pat_x1[0][i] = Point(i,
				resultados.ProyeccionX_patente.at<float>(0, i));
	}
	const Point* pppx1[1] = { puntos_proy_pat_x1[0] };
	int nppx1[] = { aux.cols };
	polylines(aux, pppx1, nppx1, 1, false, Scalar(255, 0, 0), 1);
	line(aux, Point(0, resultados.umbralProyXpat),
			Point(aux.cols, resultados.umbralProyXpat), Scalar(0, 0, 255), 1);
	imshow("SegmBinariaProyX", aux);
	moveWindow("SegmBinariaProyX", x, y);
	y += aux.rows + 40;
	//SEGMENTACION X
	Scalar sc = Scalar(255, 0, 0);
	for (int i = 0; i < resultados.Caracteres.size(); i++) {
		switch (i) {
		case 0:
		case 1:
		case 5:
		case 6:
			sc = Scalar(0, 255, 0);
			rectangle(aux, resultados.Caracteres[i].inicio,
					resultados.Caracteres[i].fin, sc, 1, 1, 0);
			break;
		case 2:
		case 3:
		case 4:
			sc = Scalar(0, 255, 0);
			rectangle(aux, resultados.Caracteres[i].inicio,
					resultados.Caracteres[i].fin, sc, 1, 1, 0);
			break;
		}
	}
	imshow("SegmBinariaSegX", aux);
	moveWindow("SegmBinariaSegX", x, y);
	y += aux.rows + 40;
	// CARACTERES SEGMENTADOS
	imshow("caracteresSegmentada", resultados.imagSegmentada);
	moveWindow("caracteresSegmentada", x, y);
	y += aux.rows + 40;
}

/*--------------3-MODULO SEGMENTACIÓN--------------------------------------------------*/
/*3.1 Preprocesado Patente*/
int Segmentador::preprocesar_patente() {
	Size dimPatOriginal;
	Size dimPatAdaptado;
	/*3.1 Inicialización de Imagen interna BGR y GRAY*/
	resultados.imagPatOrig.copyTo(resultados.imagPatOrigGris);
	if (resultados.imagPatOrig.channels() == 1) {
		cvtColor(resultados.imagPatOrig, resultados.imagPatOrig, CV_GRAY2BGR);
	} else
		cvtColor(resultados.imagPatOrigGris, resultados.imagPatOrigGris,
				CV_BGR2GRAY);
	/*3.2 Redimensionado imagen*/
	dimPatOriginal.height = resultados.imagPatOrig.rows;
	dimPatOriginal.width = resultados.imagPatOrig.cols;
	dimPatAdaptado.height = 100;
	dimPatAdaptado.width =
			((float) dimPatOriginal.width / dimPatOriginal.height)
					* dimPatAdaptado.height;
	try {
		resize(resultados.imagPatOrigGris, resultados.imagPatEscalada,
				Size(dimPatAdaptado.width, dimPatAdaptado.height), 0, 0,
				INTER_CUBIC);
	} catch (Exception e) {
		resultados.codError.error = 2;	//error al redimencionar
		return (resultados.codError.error);
	}
	resultados.imagPatEscalada.copyTo(resultados.imagPatMod);
	threshold(resultados.imagPatMod, resultados.imagBinaria, 0, 255,
			CV_THRESH_OTSU + CV_THRESH_BINARY);
	Canny(resultados.imagBinaria, resultados.imagBordes, 0, 0, 3);
	resultados.imagBordes.copyTo(resultados.imagPatMod);
	return (0);
}

/*3.2 Corrección de Inclinación*/
int Segmentador::cambio_perspectiva() {
	resultados.giro = 0;
	resultados.imagBordes.copyTo(resultados.imagBordesRotada);
	resultados.angulosHorizontales.clear();
	resultados.angulosVerticales.clear();
	/*--------------HOUGH----------------*/
	vector<Vec4i> lineas2;
	try {
		HoughLinesP(resultados.imagBordes, lineas2, 1, CV_PI / 180, 13, 21, 6);
	} catch (Exception e) {
		resultados.codError.error = 3;
		return (resultados.codError.error);
	}
	/*----imagen para dibujar lineas-----*/
	resultados.imagBordes.copyTo(resultados.imagBordesHough);
	cvtColor(resultados.imagBordesHough, resultados.imagBordesHough,
			CV_GRAY2RGB);
	float giro = 0;
	Vec4i l;
	Point pt1;
	Point pt2;
	double angulo;
	double largo;
	/*----registro de lineas de interes-------*/
	for (size_t i = 0; i < lineas2.size(); i++) {
		l = lineas2[i];
		pt1 = Point(l[0], l[1]);
		pt2 = Point(l[2], l[3]);
		angulo = atan2(pt2.y - pt1.y, pt2.x - pt1.x) * 180.0 / CV_PI;
		largo = sqrt(pow((l[2] - l[0]), 2) + pow((l[3] - l[1]), 2));
		if (abs(angulo) < 30) { /*----lineas horizontales (posibles bordes superior/inferior de la patente)-------*/
			if (largo > 50) { /*----cantidad de puntos-------*/
				resultados.angulosHorizontales.push_back(angulo);
				line(resultados.imagBordesHough, pt1, pt2, Scalar(255, 0, 0), 1,
						CV_AA);
			}
		}
		if (abs(angulo) > 50 && abs(angulo) < 130) { /*----lineas verticales (posibles bordes laterales de la patente o lineas caracteres)-------*/
			resultados.angulosVerticales.push_back(angulo);
			line(resultados.imagBordesHough, pt1, pt2, Scalar(0, 0, 255), 1,
					CV_AA);
		}
	}
	/*----analisis de tendencias de lineas de interes-------*/
	/*------lineas horizontales-----*/
	if (resultados.angulosHorizontales.size() > 0) {
		/*------algoritmo que obtiene el promerio de angulos con mayor numero de ocurrencias--------*/
		resultados.mejorpromedioHor = buscar_promedios(
				resultados.angulosHorizontales, 2, 0);
		/*----corregir inclinación si el promedio de angulos mas ocurrentes es mayor a 2º-------*/
		//cout<<"angulo horizontal promedio: "<<resultados.mejorpromedioHor.promedio<<endl;
		resultados.giro = resultados.mejorpromedioHor.promedio;
	}
	/*------lineas verticales-----*/
	if (resultados.angulosVerticales.size() > 0) {
		/*------algoritmo que obtiene el promerio de angulos con mayor numero de ocurrencias--------*/
		resultados.mejorpromedioVer = buscar_promedios(
				resultados.angulosVerticales, 2, 0);
		resultados.anguloLetras = resultados.mejorpromedioVer.promedio
				- resultados.mejorpromedioHor.promedio;
	}
	resultados.imagBordesRotada = cambio_angulo(resultados.imagBordes);
	return (0);
}

Mat Segmentador::cambio_angulo(Mat imagen) {
	Mat aux;
	imagen.copyTo(aux);
	/*---------GIRO-----------*/
	Point2f src_center(imagen.cols / 2.0F, imagen.rows / 2.0F);
	Mat rot_mat = getRotationMatrix2D(src_center,
			(float) resultados.mejorpromedioHor.promedio, 1.0);
	warpAffine(aux, aux, rot_mat, imagen.size());
	if (contRota == 0)
		aux.copyTo(resultados.imagBordesRotada0);
	/*---------CORRECCION ANGULO LETRAS-----------*/
	if (resultados.angulosVerticales.size() > 0) {
		int dx = ceil(
				imagen.rows
						* tan((resultados.anguloLetras + 90) * CV_PI / 180));
		aux = warpPerspective(aux, dx);
	}
	contRota++;
	return (aux);
}

int Segmentador::quePatenteEs() {
	resultados.ProyeccionY_patente = proyeccion_eje_Y_patente(
			resultados.imagBordesRotada);

	if (comprobar_proyeccionY() != 0)
		return (resultados.codError.error);
	/*SEGMENTACION PROYECCION Y*/
	resultados.umbralProyYpat = 4;//0.5 * (mean(resultados.ProyeccionX_patente_compPatente).val[0]);
	resultados.pCandidatosY_patente = segmentar_proyeccion_Y_patente(
			resultados.ProyeccionY_patente, resultados.umbralProyYpat,
			resultados.imagBordesRotada.cols); //<-----VER EL CORTE POR ERROR--- falta control
	/*------AMBITO DE BORDES---*/
	int mayor_alto = 0;
	ind_mayor_alto = 0;
	int alto;
	for (std::vector<int>::size_type i = 0;
			i != resultados.pCandidatosY_patente.size(); i++) {
		alto = resultados.pCandidatosY_patente[i].fin.y
				- resultados.pCandidatosY_patente[i].inicio.y;
		if (alto > mayor_alto) {
			mayor_alto = alto;
			ind_mayor_alto = i;
		}
	}
	ind_mayor_altoGLOBAL1 = ind_mayor_alto;
	if (resultados.pCandidatosY_patente.size() > 0) {
		/*----corroboramos picos-------------------*/
		Mat py = resultados.ProyeccionY_patente;
		int maxSup = 0, maxInf = 0;
		float mediaInterna = 0;
		float mediaExterna = 0;
		int maxSupI, maxInfI;
		double maxVal = 0;
		double minVal = 0;
		Point maxValp, minValp;
		int extremoSuperior, extremoInferior;
		if (resultados.pCandidatosY_patente[ind_mayor_alto].inicio.y - 5 < 0)
			extremoSuperior = 0;
		else
			extremoSuperior =
					resultados.pCandidatosY_patente[ind_mayor_alto].inicio.y
							- 5;
		if (resultados.pCandidatosY_patente[ind_mayor_alto].fin.y + 5 > 0)
			extremoInferior = py.rows;
		else
			extremoInferior =
					resultados.pCandidatosY_patente[ind_mayor_alto].fin.y + 5;
		cv::minMaxLoc(
				py(
						Rect(0, extremoSuperior, 1,
								ceil(py.rows / 2) - extremoSuperior)), &minVal,
				&maxVal, &minValp, &maxValp);
		maxSupI = maxValp.y;
		resultados.pCandidatosY_patente[ind_mayor_alto].inicio.y =
				extremoSuperior + maxSupI;
		cv::minMaxLoc(
				py(
						Rect(0, ceil(py.rows / 2), 1,
								extremoInferior - ceil(py.rows / 2))), &minVal,
				&maxVal, &minValp, &maxValp);
		maxInfI = maxValp.y + ceil(py.rows / 2);
		resultados.pCandidatosY_patente[ind_mayor_alto].fin.y = maxInfI;
		Rect r(resultados.pCandidatosY_patente[ind_mayor_alto].inicio.x,
				resultados.pCandidatosY_patente[ind_mayor_alto].inicio.y,
				resultados.imagBordesRotada.cols,
				resultados.pCandidatosY_patente[ind_mayor_alto].fin.y
						- resultados.pCandidatosY_patente[ind_mayor_alto].inicio.y);
		resultados.roiSegY = r;
		resultados.imagBordesRotada(resultados.roiSegY).copyTo(
				resultados.imagBordesRotadaSegY);
		resultados.ProyeccionX_patente = proyeccion_eje_X_patente(
				resultados.imagBordesRotadaSegY);
		if (comprobar_proyeccionX() != 0)
			return (resultados.codError.error);
		resultados.ProyeccionX_patente.copyTo(
				resultados.ProyeccionX_patente_compPatente);
	} else {
		resultados.codError.error = 5;
	}
	resultados.imagBordesRotadaSegY.copyTo(resultados.imagProyX);
	return (resultados.codError.error);
}

int Segmentador::segmentarCaracteres() {
	/*--------------CAMBIO DE ANGULO SEGUN LO DETECTADO CON BORDES------------------*/
	resultados.imagPatAux = cambio_angulo(resultados.imagPatEscalada);
	/*--------------SEGMENTACION Y HECHA POR BORDES------------------*/
	resultados.imagPatAux(resultados.roiSegY).copyTo(resultados.imagPatAux);
	resultados.imagGrisRotadaSegmY = resultados.imagPatAux;
	/*------PASAR TOP-HAT o BLACK-HAT-------*/
	Mat EE3 = getStructuringElement(CV_SHAPE_RECT, cv::Size(25, 25));
	Mat EEHor1 = getStructuringElement(CV_SHAPE_RECT,
			cv::Size(ceil(resultados.imagPatAux.rows / 2.5), 1));
	switch (resultados.codError.tipoPatente) {
	case 0:			//patente vieja
		morphologyEx(resultados.imagPatAux, resultados.imagPatAux, MORPH_TOPHAT,
				EE3);
		break;
	case 1:			//patente nueva
		morphologyEx(resultados.imagPatAux, resultados.imagPatAux,
				MORPH_BLACKHAT, EE3);
		break;
	default:		//patente indefinida -->corto proceso
		return (resultados.codError.error);
		break;
	}
	threshold(resultados.imagPatAux, resultados.imagBinaria, 0, 255,
			CV_THRESH_OTSU + CV_THRESH_BINARY);
	resultados.imagBinariaRotada = resultados.imagBinaria;
	/*----------------PROYECCION Y--------------------------------------------------------------*/
	resultados.ProyeccionY_patente2 = proyeccion_eje_Y_patente(
			resultados.imagBinaria);
	resultados.umbralProyYpat2 = 0.5
			* (mean(resultados.ProyeccionY_patente2).val[0]);
	resultados.pCandidatosY_patente2 = segmentar_proyeccion_Y_patente(
			resultados.ProyeccionY_patente2, resultados.umbralProyYpat2,
			resultados.imagBinaria.cols); //<-----VER EL CORTE POR ERROR--- falta control
	int mayor_alto = 0;
	int ind_mayor_alto = 0;
	int alto;
	for (std::vector<int>::size_type i = 0;
			i != resultados.pCandidatosY_patente2.size(); i++) {
		alto = resultados.pCandidatosY_patente2[i].fin.y
				- resultados.pCandidatosY_patente2[i].inicio.y;
		if (alto > mayor_alto) {
			mayor_alto = alto;
			ind_mayor_alto = i;
		}
	}

	ind_mayor_altoGLOBAL2 = ind_mayor_alto;
	if (resultados.pCandidatosY_patente2.size() > 0) {
		Rect r(resultados.pCandidatosY_patente2[ind_mayor_alto].inicio.x,
				resultados.pCandidatosY_patente2[ind_mayor_alto].inicio.y,
				resultados.imagBordesRotada.cols,
				resultados.pCandidatosY_patente2[ind_mayor_alto].fin.y
						- resultados.pCandidatosY_patente2[ind_mayor_alto].inicio.y);
		resultados.imagBinariaRotada(r).copyTo(
				resultados.imagBinariaRotadaSegY);
	} else {
		resultados.codError.error = 5;
		return (resultados.codError.error);
	}
	/*--------------SEGMENTACION X------------------*/
	resultados.ProyeccionX_patente = proyeccion_eje_X_patente(
			resultados.imagBinariaRotadaSegY);
	segmentar_proyeccion_X_patente();
	resultados.umbralProyXpatBin = resultados.umbralProyXpat;
	return (resultados.codError.error);
}

/*3.3 Proyección eje Y*/
Mat Segmentador::proyeccion_eje_Y_patente(Mat img) {
	Mat aux;
	img.convertTo(resultados.imagPatAux, CV_32F);
	normalize(resultados.imagPatAux, resultados.imagPatAux, 0, 1, CV_MINMAX);
	reduce(resultados.imagPatAux, aux, 1, CV_REDUCE_SUM);
	return (aux);
}

/*3.4 Segmentar eje Y*/
vector<inicio_fin> Segmentador::segmentar_proyeccion_Y_patente(
		Mat ProyeccionY_patente, int umbralProyYpat, int cols) {//<------VER SI NO CONVIENE SEGMENTAR CON IMAGBINARIA CON PROYECCION NUEVA
	vector<inicio_fin> pCandidatosYAux;
	int indIni = 0;
	int indFin = 0;
	int altoCandY = 0;
	int alto_min_cand = 0.15 * ProyeccionY_patente.rows; //0.2 * 100 = 20
	for (int i = 1; i < ProyeccionY_patente.rows; i++) {
		if (ProyeccionY_patente.at<float>(0, i) > umbralProyYpat) {
			if (ProyeccionY_patente.at<float>(0, i - 1) <= umbralProyYpat)
				indIni = i; //entra en cresta
			if (i == ProyeccionY_patente.rows - 1) {//pregunto si termina recorrido bajo el umbral
				altoCandY = i - indIni;
				if (altoCandY > alto_min_cand) { //<<<<---------- 1) - corte fijado en 10
					indFin = i;
					pCandidatosYAux.push_back(
							{ Point(0, indIni), Point(cols - 1, indFin) });
				}
			}
		} else {
			if (ProyeccionY_patente.at<float>(0, i - 1) >= umbralProyYpat) { //sale en cresta
				altoCandY = i - indIni;
				if (altoCandY > alto_min_cand) { //<<<<---------- 1) - corte fijado en 10

					indFin = i;
					pCandidatosYAux.push_back(
							{ Point(0, indIni), Point(cols - 1, indFin) });
				}
				indIni = i;
			}
		}
	}

	return (pCandidatosYAux);

}

/*3.5 Sacar linear horizontales*/
Mat Segmentador::proyeccion_eje_X_patente(Mat img) {
	Mat proyX;
	img.convertTo(resultados.imagPatAux, CV_32FC1);
	normalize(resultados.imagPatAux, resultados.imagPatAux, 0, 1, CV_MINMAX);
	reduce(resultados.imagPatAux, proyX, 0, CV_REDUCE_SUM);
	return (proyX);
}

/*3.6 Segmentar eje X*/
int Segmentador::segmentar_proyeccion_X_patente() {
	resultados.umbralProyXpat = 0.08 * resultados.imagBinariaRotadaSegY.rows; //ancho del trazo segun el alto de la patente;    //antes usaba 0.25 * (mean(ProyeccionX[0]).val[0]);--->0.08
	int indIni = 0;
	int indFin = 0;
	inicio_fin aux;
	Point punto_inicio;
	Point punto_fin;
	int ancho = 0;
	int ancho_minimo = 0.25 * resultados.imagBinariaRotadaSegY.rows;
	bool band_letras = false;
	vector<inicio_fin> pCandidatosX_patente;
	for (int i = 1; i < resultados.ProyeccionX_patente.cols; i++) {
		float pi = resultados.ProyeccionX_patente.at<float>(0, i);	//valor en i
		float pia = resultados.ProyeccionX_patente.at<float>(0, i - 1);	//valor en i-1
		if (pi > resultados.umbralProyXpat) {
			if (pia <= resultados.umbralProyXpat) {
				indIni = i;
			}
			if (i == resultados.ProyeccionX_patente.cols - 1) {	//pregunto si termina recorrido bajo el umbral
				ancho = i - indIni;
				if (ancho > ancho_minimo) { //<<<<---------- 1) - corte fijado en 10
					indFin = i;
					pCandidatosX_patente.push_back(
							{ Point(indIni, 0), Point(indFin,
									resultados.imagBinariaRotadaSegY.rows - 1) });
				}
			}
		} else {
			if (pia >= resultados.umbralProyXpat) { 			//sale de cresta
				ancho = i - indIni;
				if (ancho > ancho_minimo) {
					indFin = i;
					pCandidatosX_patente.push_back(
							{ Point(indIni, 0), Point(indFin,
									resultados.imagBinariaRotadaSegY.rows - 1) });
				}
				indIni = i;
			}
		}
	}
	/*Dejar solamente los caracteres*/
	resultados.imagBinariaRotadaSegY.copyTo(resultados.imagBinariaRotadaSegX);
	if (pCandidatosX_patente.size() > 0) {
		resultados.imagBinariaRotadaSegX = 0 * resultados.imagBinariaRotadaSegX;
		vector<double> aux;
		Mat caracter;
		for (std::vector<int>::size_type i = 0;
				i != pCandidatosX_patente.size(); i++) {
			Rect r(pCandidatosX_patente[i].inicio.x,
					pCandidatosX_patente[i].inicio.y,
					pCandidatosX_patente[i].fin.x
							- pCandidatosX_patente[i].inicio.x,
					pCandidatosX_patente[i].fin.y
							- pCandidatosX_patente[i].inicio.y);
			if (controlar_caracter(resultados.imagBinariaRotadaSegY(r))) {
				resultados.imagBinariaRotadaSegY(r).copyTo(
						resultados.imagBinariaRotadaSegX(r));
				aux.push_back(
						pCandidatosX_patente[i].fin.x
								- pCandidatosX_patente[i].inicio.x);
				resultados.Caracteres.push_back(
						{ resultados.imagBinariaRotadaSegX(r),
								pCandidatosX_patente[i].inicio,
								pCandidatosX_patente[i].fin, 1 });
			}
		}
		resultados.mejorAnchoCaracter = buscar_promedios(aux, 5, 0);
	} else {
		resultados.codError.error = 6;
	}
	return (resultados.codError.error);
}

double Segmentador::pasar_patron(Mat Patente, Mat ProyeccionX) {
	int ancho_letra;
	int ancho_espacio_menor;
	int ancho_espacio_mayor;
	int largo_total;
	Mat patron;
	ancho_letra = 0;
	ancho_espacio_menor = 0;
	ancho_espacio_mayor = 0;
	largo_total = 0;
	patron = Mat::zeros(1, 1, CV_32FC1);
	int alto = Patente.rows;
	ancho_letra = round(alto * 0.5);
	ancho_espacio_menor = round(alto * 0.11);
	ancho_espacio_mayor = round(alto * 0.72);
	largo_total = (ancho_letra * 6) + (ancho_espacio_menor * 4)
			+ (ancho_espacio_mayor);
	patron = Mat::zeros(1, largo_total, CV_32FC1);
	patron.at<float>(0, ancho_letra - 1) = 255;
	patron.at<float>(0, ancho_letra + ancho_espacio_menor) = 255;
	patron.at<float>(0, (2 * ancho_letra) + ancho_espacio_menor - 1) = 255;
	patron.at<float>(0, (2 * ancho_letra) + (2 * ancho_espacio_menor)) = 255;
	patron.at<float>(0, (3 * ancho_letra) + (2 * ancho_espacio_menor) - 1) =
			255;
	patron.at<float>(0,
			(3 * ancho_letra) + (2 * ancho_espacio_menor) + ancho_espacio_mayor) =
			255;
	patron.at<float>(0,
			(4 * ancho_letra) + (2 * ancho_espacio_menor) + ancho_espacio_mayor
					- 1) = 255;
	patron.at<float>(0,
			(4 * ancho_letra) + (3 * ancho_espacio_menor) + ancho_espacio_mayor) =
			255;
	patron.at<float>(0,
			(5 * ancho_letra) + (3 * ancho_espacio_menor) + ancho_espacio_mayor
					- 1) = 255;
	patron.at<float>(0,
			(5 * ancho_letra) + (4 * ancho_espacio_menor) + ancho_espacio_mayor) =
			255;
	patron.at<float>(0,
			(6 * ancho_letra) + (4 * ancho_espacio_menor) + ancho_espacio_mayor
					- 1) = 255;
	int estado = 255;
	for (int i = 0; i < largo_total - 1; i++) {
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
	filter2D(ProyeccionX, convolucion, CV_32FC1, patron);
	filter2D(patron, autoconvolucion, CV_32FC1, patron);
	cv::minMaxLoc(convolucion, &minVal, &maxVal, 0);
	cv::minMaxLoc(autoconvolucion, &minValAuto, &maxValAuto, 0);
	return (maxVal / maxValAuto);
}

int Segmentador::comprobar_proyeccionY() {
	float puntaje = 0;
	Mat py = resultados.ProyeccionY_patente;
	int maxSup = 0, maxInf = 0;
	float mediaInterna = 0;
	float mediaExterna = 0;
	int maxSupI, maxInfI;
	double maxVal = 0;
	double minVal = 0;
	Point maxValp, minValp;
	cv::minMaxLoc(py(Rect(0, 0, 1, ceil(py.rows / 2))), &minVal, &maxVal,
			&minValp, &maxValp);
	maxSup = (int) maxVal;
	maxSupI = maxValp.y;
	//cout<<endl<<py<<endl;
	cv::minMaxLoc(py(Rect(0, ceil(py.rows / 2), 1, ceil(py.rows / 2))), &minVal,
			&maxVal, &minValp, &maxValp);
	maxInf = (int) maxVal;
	maxInfI = maxValp.y + ceil(py.rows / 2);
	maxSupI = 1;
	mediaInterna = mean(py(Rect(0, maxSupI + 1, 1, maxInfI - maxSupI)))[0];
	if (mediaInterna < 5) {
		resultados.codError.error = 4;
		resultados.codError.tipoPatente = 2;
		resultados.codError.tipoFalloPatente = 1;
		cout << "0-media interna baja" << endl;
		return (resultados.codError.error);
	}
	if (maxSupI > 0)
		mediaExterna = (mean(py(Rect(0, 0, 1, maxSupI - 1)))[0]
				+ mean(py(Rect(0, maxInfI + 1, 1, py.rows - maxInfI - 1)))[0])
				/ 2;
	else
		mediaExterna = (mean(py(Rect(0, 0, 1, 1)))[0]
				+ mean(py(Rect(0, maxInfI + 1, 1, py.rows - maxInfI - 1)))[0])
				/ 2;
	return (0);
}

int Segmentador::comprobar_proyeccionX() {
	float puntaje = 0;
	resultados.codError.tipoPatente = 1;
	resultados.umbralProyXpat = ceil(
			(float) 1.2 * mean(resultados.ProyeccionX_patente).val[0]);
	umbralTipoPatente = resultados.umbralProyXpat;
	int cuenta = 0;
	iInicial = ceil((float) (resultados.ProyeccionX_patente.cols * 3 / 8));
	iFinal = ceil((float) (resultados.ProyeccionX_patente.cols * 5 / 8));
	anchoMinimo = ceil((float) (resultados.ProyeccionX_patente.cols / 12));
	for (int i = iInicial; i < iFinal; i++) {
		float pi = resultados.ProyeccionX_patente.at<float>(0, i);
		float pia = resultados.ProyeccionX_patente.at<float>(0, i - 1);
		if (pi < resultados.umbralProyXpat) {
			cuenta++;
			if (cuenta > anchoMinimo) {
				resultados.codError.tipoPatente = 0;
				break;
			}
		} else
			cuenta = 0;
	}
	return (0);
}

promedio Segmentador::buscar_promedios(vector<double> valores, int rangLibertad,
		int mostrar) {
	vector<promedio> vp;
	vector<double> valoresaux = valores;
	//ordenar valores
	sort(valoresaux.begin(), valoresaux.end());
	float promaux = valoresaux[0];
	int cantidad = 1;
	//buscar promedios
	for (int i = 1; i < valoresaux.size(); i++) {
		if (abs(valoresaux[i] - promaux) < rangLibertad) {
			promaux = (promaux + valoresaux[i]) / 2;
			cantidad++;
		} else {
			vp.push_back( { cantidad, promaux });
			cantidad = 1;
			promaux = valoresaux[i];
		}
	}
	vp.push_back( { cantidad, promaux });
	int indMax;
	int CantidadMax = 0;
	if (mostrar)
		cout << "PROMEDIOS" << endl;
	for (int i = 0; i < vp.size(); i++) {
		if (vp[i].cantidad > CantidadMax) {
			CantidadMax = vp[i].cantidad;
			indMax = i;
		}
	}
	return (vp[indMax]);
}

Mat Segmentador::warpPerspective(Mat patente, int dx) {
	Mat aux;
	cv::Point2f srcQuad[4], dstQuad[4];
	cv::Mat warpMatrix;
	cv::Mat inverseMatrix;
	int dx2;
	if (dx > 0)
		dx2 = 0;
	else
		dx2 = dx;
	srcQuad[0].x = dx; //dst Top left
	srcQuad[0].y = 0;
	srcQuad[1].x = patente.cols + dx; //dst Top right
	srcQuad[1].y = 0;
	srcQuad[2].x = 0; //dst Bottom left
	srcQuad[2].y = patente.rows;
	srcQuad[3].x = patente.cols; //dst Bot right
	srcQuad[3].y = patente.rows;
	dstQuad[0].x = dx2; //dst Top left
	dstQuad[0].y = 0;
	dstQuad[1].x = patente.cols + dx2; //dst Top right
	dstQuad[1].y = 0;
	dstQuad[2].x = dx2; //dst Bottom left
	dstQuad[2].y = patente.rows;
	dstQuad[3].x = patente.cols + dx2; //dst Bot right
	dstQuad[3].y = patente.rows;
	if (contRota == 0) {
		cvtColor(resultados.imagBordesRotada0, resultados.imagBordesRotada2,
				CV_GRAY2BGR);
		line(resultados.imagBordesRotada2,
				Point(srcQuad[1].x - dx, srcQuad[1].y),
				Point(srcQuad[3].x - dx, srcQuad[3].y), Scalar(0, 0, 255), 1);
		line(resultados.imagBordesRotada2, Point(srcQuad[0].x, srcQuad[0].y),
				Point(srcQuad[2].x, srcQuad[2].y), Scalar(0, 0, 255), 1);
	}
	warpMatrix = cv::getPerspectiveTransform(srcQuad, dstQuad);
	cv::warpPerspective(patente, aux, warpMatrix, aux.size(), INTER_LINEAR,
			BORDER_CONSTANT);
	return (aux);
}

bool Segmentador::controlar_caracter(Mat caracter) {
	bool es = true;
	float relacion = (float) countNonZero(caracter)
			/ (caracter.cols * caracter.rows);
	if (relacion < 0.20 || relacion > 0.85) {
		es = false;
	}
	return (es);
}

int Segmentador::postproceso() {
	resultados.imagBinariaRotadaSegX.copyTo(resultados.imagSegmentada);
	if (resultados.Caracteres.size() > 6) {
		vector<int> espacios;
		bool empiezaNum = false;
		int cuentaLetras = 0;
		int cuentaNumeros = 0;
		for (int i = 0; i < resultados.Caracteres.size() - 1; i++) {
			espacios.push_back(
					resultados.Caracteres[i + 1].inicio.x
							- resultados.Caracteres[i].fin.x);
			if (!empiezaNum) {
				resultados.Caracteres[i].tipo = 0;//todavia no paso el espacio mayor--->es letra
				cuentaLetras++;
			} else
				cuentaNumeros++;
			if ((float) espacios[i] / resultados.Caracteres[i].fin.y * 100
					> 50) {
				empiezaNum = true;
			}
		}
		cuentaNumeros++;
		cout << "cuentaLetras: " << cuentaLetras << endl;
		cout << "cuentaNumeros: " << cuentaNumeros << endl;
		if (cuentaLetras == 4) { //eliminar el primero
			resultados.Caracteres.erase(resultados.Caracteres.begin());
		}
		if (cuentaNumeros == 4) { //eliminar el ultimo
			resultados.Caracteres.erase(resultados.Caracteres.end());
		}
		resultados.imagSegmentada = resultados.imagSegmentada * 0;
		for (int i = 0; i < resultados.Caracteres.size(); i++) {

			resultados.Caracteres[i].imagen.copyTo(
					resultados.imagSegmentada(
							Rect(resultados.Caracteres[i].inicio,
									resultados.Caracteres[i].fin)));
		}
	}
	if (resultados.codError.tipoPatente == 0
			&& resultados.Caracteres.size() == 6)
		resultados.codError.error = 0;

	if (resultados.codError.tipoPatente == 1
			&& resultados.Caracteres.size() == 7)
		resultados.codError.error = 0;
	return (resultados.codError.error);
}
