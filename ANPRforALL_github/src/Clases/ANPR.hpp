
#ifndef ANPR_H_
#define ANPR_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "estructuras.hpp"
#include "Preprocesador.hpp"
#include "Localizador.hpp"
#include "Segmentador.hpp"
#include "Reconocedor.hpp"

using namespace cv;
using namespace std;


class ANPR {

private:
	Preprocesador* prep;
	Localizador* loc;
	Segmentador *seg;
	Reconocedor *rec;
	string numeracion;
	codigoError codEr;
	int mostrarPasos;

public:

	ANPR();

	~ANPR();

	void ejecutar(String nombre,Mat &imgEntrada,Mat &imagenSalida, int mostrar, int cand);
};

#endif /* ANPR_H_ */


