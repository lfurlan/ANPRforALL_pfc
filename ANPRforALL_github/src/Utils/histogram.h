#if !defined HISTOGRAM
#define HISTOGRAM
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class Histogram1D {

  private:

    int histSize[1];
    float hranges[2];
    const float* ranges[1];
    int channels[1];

  public:

    Histogram1D()
    {

        // Preparar argumentos para el histograma 1D
        histSize[0]= 256;
        hranges[0]= 0.0;
        hranges[1]= 255.0;
        ranges[0]= hranges;
        channels[0]= 0; // por defecto,canal 0
    }


    // Calcular el histograma 1D.
    cv::MatND getHistogram(const cv::Mat &image) {

        cv::MatND hist;

        // Calcular histograma
        cv::calcHist(&image,
            1,   // histograma de solo una imagen
            channels, // el canal usado
            cv::Mat(), // sbinin Mask
            hist,  // el histograma resultante
            1,   // este es un histograma de 1 dimension
            histSize, // numero de contenedores en el histograma
            ranges  // rango de valores de píxeles
        );

        return hist;
    }



    // Calcula el histograma 1D y devuelve una imagen de ella.
    cv::Mat getHistogramImage(const cv::Mat &image){

        // Calcular histograma
        cv::MatND hist= getHistogram(image);

        // Obtener los contenedores min y max
        double maxVal=0;
        double minVal=0;
        cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);

        // Imagen en la que se mostrará el histograma
        cv::Mat histImg(histSize[0], histSize[0], CV_8U,cv::Scalar(255));

        // establecer el punto más alto como 90% of nbins
        int hpt = static_cast<int>(0.9*histSize[0]);

        // dibujar lineas verticales por cada contenedor
        for( int h = 0; h < histSize[0]; h++ ) {

            float binVal = hist.at<float>(h);
            int intensity = static_cast<int>(binVal*hpt/maxVal);
            cv::line(histImg,cv::Point(h,histSize[0]),cv::Point(h,histSize[0]-intensity),cv::Scalar::all(0));
        }

        return histImg;
    }


};
 #endif
