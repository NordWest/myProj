#include <QtCore/QCoreApplication>
#include <mgl/mgl_eps.h>
//#include <mgl/mgl_data.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    mglGraphPS gr;
    mglData ad(20,30), bd(20,30);
    ad.Modify("0.6*sin(2*pi*x)*sin(3*pi*y) + 0.4*cos(3*pi*(x*y))");
    bd.Modify("0.6*cos(2*pi*x)*cos(3*pi*y) + 0.4*cos(3*pi*(x*y))");
    gr.Box();
    gr.Vect(ad,bd);
    gr.WritePNG("test.png");


    return 0;//a.exec();
}
