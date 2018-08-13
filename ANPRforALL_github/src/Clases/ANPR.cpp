#include "ANPR.hpp"






ANPR::ANPR(){
	this->prep=new Preprocesador();
	this->loc;
	this->seg=new Segmentador();
	this->rec=new Reconocedor();
	estadio=0;
}

vector<inicio_fin> ANPR::ejecutar(String nombre,Mat &imgEntrada,Mat &imgSalida, int mostrar, int cand){

	estadio=0;
	Mat imgSalida2;
	int error=0;
	codigoError codEr;
	clock_t t1, t2,t3,t4,t5,t6,t7;
	vector<inicio_fin> candidatos;


	imgEntrada = imread(nombre,IMREAD_COLOR);
	if(imgEntrada.empty()) {
		error=-1;
		//return(error);
		}
	t1=clock();
	if(!error){
		error=prep->ejecutar(imgEntrada,imgSalida);
		prep->~Preprocesador();
	}
	t2=clock();



	if(!error){

		loc=new Localizador(imgSalida,imgSalida);

		candidatos=loc->ejecutar();

		if(mostrar==2)loc->mostrar_pasos();
		stringstream out;
		int pasada=1;

		/*-------TOP-HAT------------*/
		if(mostrar==3){
			Mat patente1=loc->obtener_candidato(cand);
			if(patente1.data){

				codEr=seg->ejecutar(patente1);
				Mat patenteSeg;
				if(codEr.error==0){
					patenteSeg=seg->sacarImagen();
					rec->ejecutar(patenteSeg);
					seg->mostrar_pasos();
				} else {
					cout<<"Error "<<codEr.error<<endl;
				}
			} else cout<<"imagen de candidato TOP-HAT sin datos"<<endl;
		}
//		cout<<"------------TOP-HAT--------------"<<endl;
//		Mat patente1=loc->obtener_candidato();
//		if(patente1.data){
//
//			codEr=seg->ejecutar(patente1);
//			Mat patenteSeg;
//			if(codEr.error==0){
//				patenteSeg=seg->sacarImagen();
//				rec->ejecutar(patenteSeg);
//				goto fin;
//			} else {
//				cout<<"Primera pasada sin resultados, error "<<codEr.error<<endl;
//				seg->mostrar_pasos();
//			}
//		} else cout<<"imagen de candidato TOP-HAT sin datos"<<endl;
//

		/*-------BLACK-HAT-----------*/
//		cout<<"------------BLACK-HAT--------------"<<endl;
//		Mat patente2=loc->obtener_candidato();
//		if(patente2.data){
//			codEr=seg->ejecutar(patente2);
//			Mat patenteSeg;
//			if(codEr.error==0){
//				patenteSeg=seg->sacarImagen();
//				rec->ejecutar(patenteSeg);
//			} else {
//				cout<<"Segunda pasada sin resultados, error "<<codEr.error<<endl;
//				seg->mostrar_pasos();
//			}
//		} else cout<<"imagen de candidato BLACK-HAT sin datos"<<endl;

		loc->~Localizador();
	}

	fin:
	//if(!error)
	//res=seg->ejecutar(res);
//	printf("tiempo preprocesamiento: %f\n", (float)(t2 - t1) / 1000);
//	printf("tiempo localizacion: %f\n", (float)(t3 - t2) / 1000);
//	printf("tiempo comprobar patente: %f\n", (float)(t4 - t3) / 1000);
//	printf("que patente es: %f\n", (float)(t5 - t4) / 1000);
//	printf("limpiar caracteres: %f\n", (float)(t6 - t5) / 1000);
//	printf("reconocer caracteres: %f\n", (float)(t7 - t6) / 1000);
//	printf("tiempo total: %f\n", (float)(t7 - t1) / CLK_TCK);




	//return(error);
	return(candidatos);
}



