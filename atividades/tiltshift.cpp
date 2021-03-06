#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

// GLOBAL VARIABLES

int blurValue = 7 ;
int blur_slider = 0;
int blur_slider_max = 10;

int MAX = 100;
double ponderacao = 6;
int ponderacao_slider = 0;

int top_slider = 0;
int posicao_vertical = 0;

int tamanho_faixa = 20;
int altura_slider = 0;

Mat img1, img2, alpha, beta1;
int height;

char TrackbarName[50];

// FUNCTIONS

void refresh(){
    Mat srcImg, blurImg, tiltshiftImg;
    img1.convertTo(srcImg, CV_32FC3);
    img2.convertTo(blurImg, CV_32FC3);
    multiply(srcImg, alpha, srcImg);
    multiply(blurImg, beta1, blurImg);
    add(srcImg, blurImg, tiltshiftImg);
    tiltshiftImg.convertTo(tiltshiftImg, CV_8UC3);
    imshow("tiltshift", tiltshiftImg);
    imwrite("tiltshift.jpg", tiltshiftImg);
}

void blur(int blurValue){
    Mat aux, mask;
    float media[] = {1,1,1,
                     1,1,1,
                     1,1,1};
    // Creates the Mask matrix
    mask = Mat(3, 3, CV_32F, media); 
    scaleAdd(mask, 1/9.0, Mat::zeros(3,3,CV_32F), mask);
    // "img1" is used otherwise the blur will always increase
    img1.convertTo(aux, CV_32F); 
    for (int i = 0; i < blurValue; i++) {
        filter2D(aux, aux, aux.depth(), mask, Point(1, 1), 0);
    }
    aux=abs(aux);
    aux.convertTo(img2, CV_8UC3);
}

void tiltShiftCalc() {
    int l1 = -tamanho_faixa/2;
    int l2 = -l1;

    alpha = Mat::zeros(img1.rows, img1.cols, CV_32F);
    beta1 = Mat::zeros(img1.rows, img1.cols, CV_32F);
    int i, j;
    for (i = 0; i < alpha.rows; i++) {
        int x = i - (posicao_vertical + tamanho_faixa/2);
        float alphaValue = 0.5f * (tanh((x - l1)/ponderacao) 
                                    - tanh((x - l2)/ponderacao));
        for (j = 0; j < alpha.cols; j++) {
            alpha.at<float>(i, j) = alphaValue;
            beta1.at<float>(i, j) = 1 - alphaValue;
        }
    }
    Mat auxA[] = {alpha, alpha, alpha};
    Mat auxB[] = {beta1, beta1, beta1};
    merge(auxA, 3, alpha);
    merge(auxB, 3, beta1);
    // imshow("alpha", alpha);
    // imshow("beta", beta1);
    refresh();
}

// TRACKBARS

void on_trackbar_blur(int, void*){
    blurValue = blur_slider;
    // Blurs image
    blur(blurValue);
    // Updates window
    refresh();
}

void on_trackbar_ponderacao(int, void*){
    ponderacao = (double) ponderacao_slider;
    if (ponderacao < 1) {
        ponderacao = 1;
    }
    tiltShiftCalc();
}

void on_trackbar_posicaoVertical(int, void*){
    posicao_vertical = top_slider*height/MAX;
    tiltShiftCalc();
}

void on_trackbar_altura_regiao(int, void*) {
    tamanho_faixa = altura_slider*height/MAX;
    if (tamanho_faixa == 0) {
        tamanho_faixa = 1;
    }

    if (tamanho_faixa > height) {
        tamanho_faixa = height;
    }
    tiltShiftCalc();
}

// MAIN

int main(int argvc, char** argv){
    img1 = imread(argv[1], IMREAD_COLOR);
    height = img1.size().height;
    img2 = img1.clone();
    img2.convertTo(img2, CV_8UC3);

    blur(7);

    namedWindow("tiltshift", 1);

    // sprintf( TrackbarName, "Blur Value");
    // createTrackbar( TrackbarName, "Blur",
    //                 &blur_slider,
    //                 blur_slider_max,
    //                 on_trackbar_blur);
    // on_trackbar_blur(blur_slider, 0);
    sprintf( TrackbarName, "Posição Vertical");
    createTrackbar( TrackbarName, "tiltshift",
                    &top_slider,
                    MAX,
                    on_trackbar_posicaoVertical );
    sprintf( TrackbarName, "Altura da região");
    createTrackbar( TrackbarName, "tiltshift",
                    &altura_slider,
                    MAX,
                    on_trackbar_altura_regiao);
    sprintf( TrackbarName, "Ponderação");
    createTrackbar( TrackbarName, "tiltshift",
                    &ponderacao_slider,
                    MAX,
                    on_trackbar_ponderacao);
    tiltShiftCalc();
    waitKey(0);

    return 0;
}
