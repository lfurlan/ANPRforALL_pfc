#include "ANPR.hpp"

ANPR::ANPR(){
	this->prep=new Preprocesador();
	this->loc;
	this->seg=new Segmentador();
	this->rec=new Reconocedor();
}

void ANPR::ejecutar(String nombre,Mat &imgEntrada,Mat &imgSalida, int mostrar, int cand){
	codEr.error=0;
	mostrarPasos=mostrar;

	imgEntrada = imread(nombre,IMREAD_COLOR);
	if(imgEntrada.empty()) {
		codEr.error=-1;
		}
	if(!codEr.error){
		codEr.error=prep->ejecutar(imgEntrada,imgSalida);
		prep->~Preprocesador();
	}
	if(!codEr.error){
		loc=new Localizador(imgSalida,imgSalida);
		loc->ejecutar();

		if(mostrarPasos==2)loc->mostrar_pasos();
		stringstream out;
		int pasada=1;

		if(!codEr.error){
			Mat patente1=loc->obtener_candidato(cand);
			if(patente1.data){
				codEr=seg->ejecutar(patente1);
				Mat patenteSeg;
				if(!codEr.error){
					if(mostrarPasos==3)seg->mostrar_pasos();
					patenteSeg=seg->sacarImagen();
					numeracion=rec->ejecutar(patenteSeg);
					if(mostrarPasos==4)rec->mostrar_pasos();
					cout<<"Tipo de patente encontrada: ";
					if(codEr.tipoPatente) cout<< "Mercosur(desde 2016)"<<endl;
					else  cout<< "Argentina(1996-2015)"<<endl;
					cout<<"Numeracion de patente: "<<numeracion<<endl;
				} else {
					cout<<"Error de segmentación: "<<codEr.error<<endl;
				}
			} else cout<<"Patente no localización"<<endl;
		}
	}
	prep->~Preprocesador();
	loc->~Localizador();
	seg->~Segmentador();
	rec->~Reconocedor();
}



