#include "mb.h"
#include "math.h"

#define M_2PI (2.*M_PI)

bool ssleg(const double* a, double *const x, const double* b, int size)
{
/*
решение системы линейных алгебраических уравнений методом Гаусса
a - массив коэффициентов перед неизвестными,
b - коэффициенты правых частей,
x - массив, в который помещаются результаты,
size - число уравнений и неизвестных.
функция возвращает true, если решение выполнено успешно, и - false в противном случае.
*/
        //создаем массивы и копируем в них коэффициенты перед неизвестными (A) и правые части (B)
        double* A = new double[size*size];
        for(int i=0; i<size*size; i++) A[i]=a[i];
        double* B = new double[size];
        for(int i=0; i<size; i++) {B[i] = b[i];x[i] = 0;}
        /////////////////////////////////////////////
        double tmp;//переменная для хранения промежуточных данных
        //прямой ход метода Гаусса: приводим матрицу коэффициентов к треугольному виду
        int el;
        for(int k=0;k<size-1;k++)
        {
                if (A[k*size+k]==0)
                {
                        el=k+1;
                        while((A[el*size+k]==0)&&(el<size)) el++;
                        if(el==size) continue;
                        else
                        {
                                for(int i=k;i<size;i++){tmp=A[k*size+i];A[k*size+i]=A[el*size+i];A[el*size+i]=tmp;}
                                tmp=B[k];B[k]=B[el];B[el]=tmp;
                        }
                }
                for(int i=k+1;i<size;i++)
                {
                        tmp = A[i*size+k]/A[k*size+k];
                        for(int j=k;j<size;j++)A[i*size+j]-=tmp*A[k*size+j];
                        B[i]-=tmp*B[k];
                }
        }
        /////////////////////////////////////////////
        //обратный ход метода Гаусса: вычисляем неизвестные
        for(int k=size-1;k>=0;k--)
        {
                if(A[k*size+k]==0){delete[]A;delete[]B;return false;}
                tmp = B[k];
                for(int i=k;i<size;i++) tmp-=A[k*size+i]*x[i];
                x[k] = tmp/A[k*size+k];
        }
        /////////////////////////////////////////////
        delete[]A;
        delete[]B;
        return true;
}

bool invgj(double *const inv, const double* a, int size)
{
        /*
        обращение матрицы методом Гаусса-Жордана
        a - указатель на массив элементов матрицы,
        inv - указатель на массив, в который должен быть записан результат
        size - размер матрцы.
        функция возвращает true, если обращение выполнено успешно, и - false в противном случае.
        */
        //создаем массивы A(исходная матрица) и INV (единичная матрица) и заполняем их соответствующими элементами
        double* A = new double[size*size];
        for(int i=0; i<size*size; i++)
                        A[i]=a[i];
        double* INV = new double[size*size];
        for(int i=0; i<size; i++)
                for(int j=0; j<size; j++)
                        if(i==j)INV[i*size+j] =1; else INV[i*size+j]=0;
        double tmp;//переменная для хранения промежуточных данных
        int el;
        //прямой ход
        for(int k = 0; k<size-1; k++)
        {
                if (A[k*size+k]==0)
                {
                        el=k+1;
                        while((A[el*size+k]==0)&&(el<size)) el++;
                        if(el==size) continue;
                        else
                        {
                                for(int i=k;i<size;i++){tmp=A[k*size+i];A[k*size+i]=A[el*size+i];A[el*size+i]=tmp;}
                                for(int i=0;i<size;i++){tmp=INV[k*size+i];INV[k*size+i]=INV[el*size+i];INV[el*size+i]=tmp;}
                        }
                }
                //преобразование, приводящее исходную матрицу к верхней треугольной матрице
                for(int i = k+1; i<size; i++)
                {
                        tmp = A[i*size+k]/A[k*size+k];
                        for(int j = k; j<size; j++)A[i*size+j]-=A[k*size+j]*tmp;
                        for(int j = 0; j<size; j++)INV[i*size+j]-=INV[k*size+j]*tmp;
                }
        }
        //
        for(int k = 0; k<size; k++)
        {
                tmp =A[k*size+k];
                if(tmp==0){delete[]A;delete[]INV;return false;}
                for(int i=0;i<size;i++){A[k*size+i]=A[k*size+i]/tmp;INV[k*size+i]=INV[k*size+i]/tmp;}
        }
        //обратный ход (преобразуем элементы матрицы INV так как если бы мы приводили верхнюю треугольную матрицу А к единичной)
        for(int k=size-1;k>=0;k--)
        {
                for(int i=0;i<k;i++)
                        for(int j=0; j<size; j++)
                                INV[i*size+j]-=INV[k*size+j]*A[i*size+k];
        }
        //////////////////////////////////////////////////////////
        //записываем результаты для вывода
        for(int i=0; i<size*size; i++) inv[i]= INV[i];
        delete[]A;
        delete[]INV;
        return true;
}

bool mvprod(double *const r, const double* m, const double* v, int w, int h)
/*
умножение матрицы  m на вектор v.
r - указатель на массив элементов результирующей матрицы,
m - указатель на массив элементов исходной матрицы
v - указатель на массив элементов вектора размером w
w,h - размеры матрцы по горизонтали и вертикали.
функция возвращает true
*/
{
        for(int i=0; i<h; i++)
        {
                r[i]=0;
                for(int j=0; j<w; j++) r[i]+=m[i*w+j]*v[j];
        }
        return true;
}

bool mmprod(double *const r, const double* ml, const double* mr, int wl, int hl, int wr, int hr)
/*
умножение матрицы  ml(размеры матрицы wl X hl) на матрицу mr (размеры матрицы wr X hr).
r - указатель на массив элементов результирующей матрицы размером wr X hl,
ml - указатель на массив элементов матрицы, стоящей слева,
mr - указатель на массив элементов матрицы, стоящей справа,
функция возвращает true при успешном выполнении операции и false в случае wl!=hr, когда умножение невозможно
*/
{
        if(wl!=hr) return false;
        ////////////////////////////
        for(int i=0; i<hl; i++)
        {
                for(int j=0; j<wr; j++)
                {
                        r[i*wr+j] = 0;
                        for(int k=0; k<hr; k++)r[i*wr+j]+=ml[i*wl+k]*mr[k*wr+j];
                }
        }
        return true;
}

void transpose(double *const r, const double* m, int w, int h)
/*
транспонирование матрицы m (размеры матрицы w X h).
r - указатель на массив элементов результирующей матрицы размером h X w,
m - указатель на массив элементов исходной матрицы,
*/
{
        for(int i=0; i<h; i++)
                for(int j=0; j<w; j++) r[j*h+i] = m[i*w+j];
}

bool mdiagprod(double *const r, const double* m, const double* d, int w, int h)
/*
умножение матрицы  m на диагональную матрицу d.
r - указатель на массив элементов результирующей матрицы,
m - указатель на массив элементов исходной матрицы
d - указатель на одномерный массив диагональных элементов матрицы d,
w,h - размеры матрцы m по горизонтали и вертикали.
функция возвращает true
*/
{
        for(int i=0; i<h; i++)
        {
                for(int j=0; j<w; j++) r[i*w+j] = m[i*w+j]*d[j];
        }
        return true;
}


void slsm(int w, int h, double *const z, const double* c, const double* L,  const double* W)
{
/*
Метод наименьших квадратов. ? ешаем систему вида L = с*z
Эта функция позволяет быстро вычислить вектор неизвестных (z), чтобы сэкономить время и ресурсы.
Ковариационная матрица, ошибка единицы веса и ошибки неизвестных не вычисляются,
L - указатель на массив элементов, представляющий вектор данных.
W - указатель на массив элементов, представляющий весовую диагональную матрицу.
Для экономии времени и ресурсов диагональная матрица хранится как одномерный массив диагональных элементов.
По умолчанию этот указатель равен 0 (то есть если веса не нужны)
w,h - размеры матрцы плана (с) по горизонтали и вертикали. Одновременно w - число неизвестных, h - число условных уравнений
z - вектор оценок параметров модели (указатель на одномерный массив, в который выводится результат)
*/
        double* ww = new double[w*w];//матрица "весов" неизвестных
        double* hh = new double[w];//вектор сt*L, ct - результат транспонирования матрицы с
        ////////////////////вычисления ведутся как описано в книге В.С. Губанов Обобщенный метод наименьших квадратов...
        double* ct = new double[w*h];
        transpose(ct, c, w, h);
        if(W!=0)//весовая диагональная матрица задана
        {
            ///norming
            double* WE = new double[h];
            double sw=0;for(int i=0;i<h;i++)sw+=W[i];
            sw=sw/(h-w);for(int i=0;i<h;i++)WE[i]=W[i]/sw;
            double* ctw = new double[w*h];
            mdiagprod(ctw, ct, WE, h, w);
            mmprod(ww, ctw, c, h, w, w, h);
            mvprod(hh, ctw, L, h, w);
            delete[]ctw;
            delete[] WE;
        }
        else // весовая диагональная матрица не задана
        {
                mmprod(ww, ct, c, h, w, w, h);
                mvprod(hh, ct, L, h, w);
        }
        ssleg(ww, z, hh, w);//решаем систему уравнений методом Гаусса
        //for(int i=0;i<w;i++)z[i]=x[i];//выводим результат
        delete[] ww;
        delete[] hh;
        delete[] ct;
}

void hclsm(int w, int h, double *const z, const double* c, const double* L, double& uwe, double *const D, const double* W,
           int q, const double* E, const double* t)
{
/*
Метод наименьших квадратов c "жесткими" условиями.
? ешаем систему вида L = с*z + r, r - это вектор невязок с нулевым матожиданием
"жесткие условия" вводятся как модель вида E*z = t (строгое равенство). q - размер матрицы Е по вертикали.
L - указатель на массив элементов, представляющий вектор данных.
W - указатель на массив элементов, представляющий весовую диагональную матрицу.
Для экономии времени и ресурсов диагональная матрица хранится как одномерный массив диагональных элементов.
По умолчанию этот указатель равен 0 (то есть если веса не нужны)
w,h - размеры матрцы плана (с) m по горизонтали и вертикали. Одновременно w - число неизвестных, h - число условных уравнений
uwe - указатель на переменную, в которую выводится значение ошибки единицы веса в квадрате
z - вектор оценок параметров модели (указатель на одномерный массив, в который выводится результат)
D - указатель на массив элементов ковариационной матрицы
*/
double* WE = new double[h];
double sw=0;for(int i=0;i<h;i++)sw+=W[i];
sw=sw/(h-w);for(int i=0;i<h;i++)WE[i]=W[i]/sw;
//
double* ww = new double[w*w];//матрица "весов" неизвестных
double* hh = new double[w];//вектор сt*L, ct - результат транспонирования матрицы 'с'
double* dd = new double[w*w];
double* ct = new double[w*h];
transpose(ct, c, w, h);//c'
double* ctw = new double[w*h];
mdiagprod(ctw, ct, WE, h, w);
mmprod(ww, ctw, c, h, w, w, h);//c'Wc
mvprod(hh, ctw, L, h, w);//c'WL
delete[] ctw;
int asize = (w+q)*(w+q);
double* A = new double[asize];
double* B = new double[w+q];
double* zk = new double[w+q];
for(int i=0;i<asize;i++) A[i] = 0;
for(int i=0;i<w;i++)
    for(int j=0;j<w;j++)
        A[i*(w+q)+j]=ww[i*w+j];
for(int i=0;i<q;i++)
    for(int j=0;j<w;j++)
    {
        A[j*(w+q)+i+w]=E[i*w+j];
        A[(i+w)*(w+q)+j]=E[i*w+j];
    }
for(int i=0;i<w;i++) B[i] = hh[i];
for(int i=0;i<q;i++) B[i+w] = t[i];
ssleg(A, zk, B, w+q);//решаем систему уравнений методом Гаусса
for(int i=0;i<w;i++) z[i] = zk[i];
///////////
//////////////////////////////////////вычисляем ошибку единицы веса
uwe=0;
double estL;
for(int i=0;i<h;i++)
{
        estL=0;
        for(int j=0;j<w;j++)estL+=c[i*w+j]*z[j];
        uwe+=WE[i]*(L[i]-estL)*(L[i]-estL);
}
uwe = uwe/(h-w);
////////////////////////вычисляем ковариационную матрицу
invgj(dd, ww, w);
for(int i=0;i<w;i++)
{
        //z[i]=x[i];
        for(int j=0;j<w;j++)
                D[i*w+j]=dd[i*w+j]*uwe;
}
////////////////////////////////////////
////////////
delete[] ww;
delete[] hh;
delete[] ct;
delete[] dd;
delete[] A;
delete[] B;
delete[] zk;
delete[] WE;
}


void lsm(int w, int h, double *const z, const double* c, const double* L, double& uwe, double *const D, const double* W)
{
/*
Метод наименьших квадратов c выводом ковариационной матрицы. ? ешаем систему вида L = с*z + r, r - это вектор невязок с нулевым матожиданием
L - указатель на массив элементов, представляющий вектор данных.
W - указатель на массив элементов, представляющий весовую диагональную матрицу.
Для экономии времени и ресурсов диагональная матрица хранится как одномерный массив диагональных элементов.
По умолчанию этот указатель равен 0 (то есть если веса не нужны)
w,h - размеры матрцы плана (с) m по горизонтали и вертикали. Одновременно w - число неизвестных, h - число условных уравнений
uwe - указатель на переменную, в которую выводится значение ошибки единицы веса в квадрате
z - вектор оценок параметров модели (указатель на одномерный массив, в который выводится результат)
D - указатель на массив элементов ковариационной матрицы
*/
        double* ww = new double[w*w];//матрица "весов" неизвестных
        double* hh = new double[w];//вектор сt*L, ct - результат транспонирования матрицы 'с'
        double* dd = new double[w*w];
        double* ct = new double[w*h];
        double* WE = new double[h];
        //double* x = new double[w];
        ////////////////////вычисления ведутся как описано в книге В.С. Губанов 'Обобщенный метод наименьших квадратов...'
        transpose(ct, c, w, h);
        if(W!=0)//весовая диагональная матрица задана
        {
            ///norming
            double sw=0;for(int i=0;i<h;i++)sw+=W[i];
            sw=sw/(h-w);for(int i=0;i<h;i++)WE[i]=W[i]/sw;
            double* ctw = new double[w*h];
            mdiagprod(ctw, ct, WE, h, w);
            mmprod(ww, ctw, c, h, w, w, h);
            mvprod(hh, ctw, L, h, w);
            delete[] ctw;
        }
        else // весовая диагональная матрица не задана
        {
                mmprod(ww, ct, c, h, w, w, h);
                mvprod(hh, ct, L, h, w);
        }
        ssleg(ww, z, hh, w);//решаем систему уравнений методом Гаусса
        //////////////////////////////////////вычисляем ошибку единицы веса
        uwe=0;
        double estL;
        for(int i=0;i<h;i++)
        {
                estL=0;
                for(int j=0;j<w;j++)estL+=c[i*w+j]*z[j];
                if(W==0)uwe+=(L[i]-estL)*(L[i]-estL);else uwe+=WE[i]*(L[i]-estL)*(L[i]-estL);
        }
        uwe = uwe/(h-w);
        ////////////////////////вычисляем ковариационную матрицу
        invgj(dd, ww, w);
        for(int i=0;i<w;i++)
        {
                //z[i]=x[i];
                for(int j=0;j<w;j++)
                        D[i*w+j]=dd[i*w+j]*uwe;
        }
        ////////////////////////////////////////
        delete[] ww;
        delete[] hh;
        delete[] ct;
        delete[] dd;
        delete[] WE;
        //delete[] x;
}

void iLSM(int w, int h, double maxdiv, int *const exclind, double *const z, const double* c, const double* L, double& uwe, double *const D, double fl, int& rn, const double* W)
{
/*
Метод наименьших квадратов c выводом ковариационной матрицы. ? ешаем систему вида L = с*z
L - указатель на массив элементов, представляющий вектор данных.
W - указатель на массив элементов, представляющий весовую диагональную матрицу.
Для экономии времени и ресурсов диагональная матрица хранится как одномерный массив диагональных элементов.
По умолчанию этот указатель равен 0 (то есть если веса не нужны)
w,h - размеры матрцы плана (с) m по горизонтали и вертикали. Одновременно w - число неизвестных, h - число условных уравнений
uwe - указатель на переменную, в которую выводится значение ошибки единицы веса в квадрате
z - вектор оценок параметров модели (указатель на одномерный массив, в который выводится результат)
D - указатель на массив элементов ковариационной матрицы
maxdiv - максимальное значение невязки условыных уравнений, уравнения с невязкой, большей maxdiv исключаются из решений
exclind - одномерный массив, содержит номер уравнения, если оно использовалось и -1, если оно исключено
fl - флаг, маркирующий способ отбраковки:
если fl>0, то на первой итерации вычисляется ошибка единицы веса и перевычисляется maxdiv = fl*'ошибку единицы веса'.
Это позволяет применять критерии типа "3-сигма"
если fl<=0, то для отбраковки используется maxdiv, заданный при вызове функции
rn - по завершении содержит число уравнений, которое реально использовалось.
*/
        double *iZ = new double[w];//вектор оценок неизвестных
        double *iC;//временный буфер для матрицы плана текущей итерации
        double *iL;//временный буфер для вектора данных
        double *iW;//временный буфер для весов
        int *indxs = new int[h];//массив индексов уравнений 0...h-1
        for(int i=0;i<h;i++)indxs[i]=i;//нумеруем уравнения
        int hs, k, nit=0;
        bool notbr;// переменная для управления условием выхода из цикла do...while
        double estL;// промежуточная переменная для расчета оценки текущего компонента вектора данных на основе строки матрицы плана и текущего вектора оценок неизвестных iZ
        double errL;
        hs = h;
        do
        {
                nit++;//число итераций
                //создаем буферы
                iC = new double[hs*w];
                iL = new double[hs];
                iW = new double[hs];
                k=0;
                for(int i=0;i<h;i++)
                        if(indxs[i]!=-1)//если уравнение с данным номером не исключено, присвоить необходимые значения всем элементам массивов
                        {
                                for(int j=0;j<w;j++){iC[k*w+j]=c[i*w+j];}
                                iL[k]=L[i];
                                if(W!=0)iW[k]=W[i];else iW[k]=1;
                                k++;
                        }
                notbr=false;//устанавливаем false, чтобы выйти из цикла do...while
                ////////////////////////////////
                if(hs>w+1)//если число оставшихся уравнений на единицу больше числа неизвестных, выходим из цикла do...while
                {
                        slsm(w, hs, iZ, iC, iL, iW);//решение системы условных уравнений методом наименьших квадратов
                        if(fl>0)
                        {
                                errL=0;
                                for(int i=0;i<hs;i++)
                                {
                                        estL=0;
                                        for(int j=0;j<w;j++)estL+=iC[i*w+j]*iZ[j];
                                        errL+=(iL[i]-estL)*(iL[i]-estL);
                                }
                                maxdiv=fl*sqrt(errL/(hs-w));
                        }
                        hs=0;
                        for(int i=0;i<h;i++)// обходим все уравнения с номерами отличными от -1, считаем невязки...
                        {
                                if(indxs[i]!=-1)
                                {
                                        estL=0;
                                        for(int j=0;j<w;j++)estL+=c[i*w+j]*iZ[j];
                                        if(fabs(L[i]-estL)>maxdiv){indxs[i]=-1;notbr=true;}//если текущая невязка больше максимально возможной, запрещаем выход из цикла do...while(устанавливаем notbr=true), присваиваем этому уравнению номер -1 (исключаем его из решения)
                                        else hs++;
                                }
                        }
                        if(notbr)//если будет новый цикл, освобождаем память.
                        {
                                delete[] iL;
                                delete[] iC;
                                delete[] iW;
                        }
                }
        }
        while(notbr);
        delete iZ;
        double iUWE;//промежуточное значение ошибки единицы веса
        double zz[w];//промежуточные результаты решений
        double* iD = new double[w*w];//промежуточная ковариационная матрица
        lsm(w, hs, zz, iC, iL, iUWE, iD, iW);
        uwe = iUWE;
        for(int i=0;i<h;i++)exclind[i]=indxs[i];
        for(int i=0;i<w;i++)z[i]=zz[i];
        for(int i=0;i<w*w;i++)D[i]=iD[i];
        rn = hs;
        delete iL;
        delete iC;
        delete iW;
        delete indxs;
}

void iterLSM(int w, int h, double *const z, const double* c, const double* L, double& uwe, double *const D, double fl, int& rn, const double* W)
{
    bool noLargeRes=true;
    int indexes[h];for(int i=0;i<h;i++)indexes[i]=0;
    rn=h;
    //
    double *iC;
    double *iL;
    double *iW;
    //
    int it;
    int nOfMaxRes;
    double res,maxRes;
    do
    {
        iC=new double[rn*w];
        iL=new double[rn];
        iW=new double[rn];
        it=0;
        for(int i=0;i<h;i++)
            if(indexes[i]==0)
            {
                for(int j=0;j<w;j++)iC[it*w+j]=c[i*w+j];
                iL[it]=L[i];iW[it]=W[i];
                it++;
            }
        lsm(w,rn,z,iC,iL,uwe,D,iW);
        delete[] iC; delete[] iL;delete[] iW;
        maxRes=0;
        for(int i=0;i<h;i++)
            if(indexes[i]==0)
            {
                res=L[i];for(int j=0;j<w;j++)res-=z[j]*c[i*w+j];
                if(fabs(res)>maxRes){maxRes=fabs(res);nOfMaxRes=i;}
            }
        if(maxRes>fl*sqrt(uwe)){indexes[nOfMaxRes]=1;noLargeRes=false;rn--;}else noLargeRes=true;
        if(rn<w+2)noLargeRes=true;
    }while(!noLargeRes);
}


void mlsm(int w, int h, double *const z, const double* c, const double* L, double& uwe, double *const D, const double* W)
{
    int indx[h];for(int i=0;i<h;i++)indx[i]=1;
    int N;
    double *mC,*mL,*mW;
    double *sC,*sL,*sW;
    double *UWE;
    int *en;
    double uwef,maxUWE,minUWE;
    int maxi,mini;
    int q;
    for(int m=h;m>w+2;m--)//do
    {
        N=0;for(int i=0;i<h;i++)if(indx[i]==1)N++;
        //if(N<w+2) break;
        sC = new double[N*w];sL = new double[N];sW = new double[N];en = new int[N];
        q=0;
        for(int i=0;i<h;i++)if(indx[i]==1){sL[q]=L[i];sW[q]=W[i];en[q]=i;for(int j=0;j<w;j++){sC[q*w+j]=c[i*w+j];}q++;}
        lsm(w,N,z,sC,sL,uwef,D,sW);
        UWE=new double[N];
        for(int k=0;k<N;k++)
        {
            mC = new double[(N-1)*w];mL = new double[N-1];mW = new double[N-1];
            q=0;
            for(int i=0;i<N;i++)if(i!=k){mL[q]=sL[i];mW[q]=sW[i];for(int j=0;j<w;j++){mC[q*w+j]=sC[i*w+j];}q++;}
            lsm(w,N-1,z,mC,mL,uwe,D,mW);
            delete[] mC;delete[] mL;delete[] mW;
            UWE[k]=uwe;
        }
        delete[] sC;delete[] sL;delete[] sW;
        maxi=0;mini=0;for(int i=0;i<N;i++){if(UWE[i]<UWE[mini])mini=i;if(UWE[i]>UWE[maxi])maxi=i;}
        maxUWE=UWE[maxi];minUWE=UWE[mini];
        delete[] UWE;
        if((maxUWE-minUWE)/uwef<0.1) {delete[] en;break;}else {indx[en[mini]]=0;delete[] en;}
        //if((maxUWE/minUWE)>4) {delete[] en;break;}else {indx[en[mini]]=0;delete[] en;}
    }
    //while((maxUWE-minUWE)/uwef>0.1);
    /////////////////////////////
    N=0;for(int i=0;i<h;i++)if(indx[i]==1)N++;
    sC = new double[N*w];sL = new double[N];sW = new double[N];en = new int[N];
    q=0;
    for(int i=0;i<h;i++)if(indx[i]==1){sL[q]=L[i];sW[q]=W[i];en[q]=i;for(int j=0;j<w;j++){sC[q*w+j]=c[i*w+j];}q++;}
    lsm(w,N,z,sC,sL,uwe,D,sW);
    delete[] sC;delete[] sL;delete[] sW;
}

void centerofmass(double *const cm, const double* fd, int rho)
{
  double sumX=0, sumY=0, sumJ=0, J;
  for(int y=0;y<2*rho;y++)
    for(int x=0;x<2*rho;x++)
      {
        J = fd[y*2*rho+x];
        if(J!=-10E-9)
        {
          sumX+=x*J;
          sumY+=y*J;
          sumJ+=J;
        }
      }
  if(sumJ!=0){cm[0] = sumX/sumJ;cm[1] = sumY/sumJ;}else{cm[0] = rho;cm[1] = rho;}
  if(isnan(cm[0]))cm[0] = rho;
  if(isnan(cm[1]))cm[1] = rho;
}

void getFWHM(double *const fwhm, const double* fd, int rho, double blevel)
{
  double J,hm=0;
  int xm = 0, ym = 0;
  for(int y=0;y<2*rho;y++)
    for(int x=0;x<2*rho;x++)
      {J = fd[y*2*rho+x];if((J!=-10E-9)&&(J>hm)){hm = J;xm=x,ym=y;}}
  fwhm[2] = hm-blevel;
  fwhm[0] = xm;
  fwhm[1] = ym;
  hm = blevel+(hm-blevel)/2;
  int fw = 0;
  for(int y=0;y<2*rho;y++)
    for(int x=0;x<2*rho;x++)
      {J = fd[y*2*rho+x];if((J!=-10E-9)&&(J>hm))fw++;}
  fwhm[3] = 2*sqrt(fw/acos(-1));
}

double getpsf(const double* p, double x, double y, int model)
//функция выдает значение PSF для вектора параметров p и пиксельных координат x,y.
// model - модель PSF (0 - профиль Лоренца, 1 - профиль Моффата, 2 - профиль Гаусса со степенью).
{
 double res;
 double r = p[4]*(x-p[0])*(x-p[0])+p[5]*(y-p[1])*(y-p[1])+p[6]*(x-p[0])*(y-p[1]);
 switch(model)
  {
    case 0:{res=p[2]+p[3]/(1+pow(r,p[7]));break;}
    case 1:{res=p[2]+p[3]/pow(1+r,p[7]);break;}
    case 2:{res=p[2]+p[3]*exp(-pow(r,p[7]));break;}
  }
 return res;
}

double getpdpsf(const double* p, double x, double y, int model, int nv)
/*функция выдает значение частной производной PSF для вектора параметров p и пиксельных координат x,y.
model - модель PSF (0 - профиль Лоренца, 1 - профиль Моффата, 2 - профиль Гаусса со степенью).
nv - номер переменной для которой вычисляется значение часной производной.
p[0] = Xo - пиксельные координаты фотоцентра изображения звездообразного объекта
p[1] = Yo
p[2] = Jo - уровень фона. Не вычисляется в данной функции, вычисляется вне функции и передается во входном массиве p
p[3]= Jmax - уровень максимума psf
p[4] = A, p[5] = B, p[6] = C - параметры, описывающие ширину профиля и его ориентировку и эллептичность
p[7] = delta - степень psf
*/
{
  double res=0;
  double ksi = x-p[0];
  double eta = y-p[1];
  double u = p[4]*ksi*ksi+p[5]*eta*eta+p[6]*ksi*eta;
  double u1,u2;
  switch(model)
    {
      case 0:{//lorentz profile partial derivatives
              u1 = p[3]/pow(1+pow(u,p[7]),2);
              u2 = p[7]*pow(u,p[7]-1);
              switch(nv)
              {
                case 0:{res = u1*u2*(2*p[4]*ksi+p[6]*eta);break;}
                case 1:{res = u1*u2*(2*p[5]*eta+p[6]*ksi);break;}
                case 2:{res = 1;break;}
                case 3:{res = 1/(1+pow(u,p[7]));break;}
                case 4:{res = -u1*u2*ksi*ksi;break;}
                case 5:{res = -u1*u2*eta*eta;break;}
                case 6:{res = -u1*u2*ksi*eta;break;}
                case 7:{res = -u1*pow(u,p[7])*log(u);break;}
              }
              break;
             }
      case 1:{//moffat profile partial derivatives
              u1 = u+1;
              u2 = p[7]*p[3]*pow(u1,-(p[7]+1));
              switch(nv)
              {
                case 0:{res = u2*(2*p[4]*ksi+p[6]*eta);break;}
                case 1:{res = u2*(2*p[5]*eta+p[6]*ksi);break;}
                case 2:{res = 1;break;}
                case 3:{res = 1/pow(u1,p[7]);break;}
                case 4:{res = -u2*ksi*ksi;break;}
                case 5:{res = -u2*eta*eta;break;}
                case 6:{res = -u2*ksi*eta;break;}
                case 7:{res = -p[3]*pow(u1,-p[7])*log(u1);break;}
              }
              break;
             }
      case 2:{//gaussian profile partial derivatives
              u1 = p[3]*exp(-pow(u,p[7]));
              u2 = -p[7]*pow(u,p[7]-1);
              switch(nv)
              {
                case 0:{res = -u1*u2*(2*p[4]*ksi+p[6]*eta);break;}
                case 1:{res = -u1*u2*(2*p[5]*eta+p[6]*ksi);break;}
                case 2:{res = 1;break;}
                case 3:{res = exp(-pow(u,p[7]));break;}
                case 4:{res = u2*u1*ksi*ksi;break;}
                case 5:{res = u2*u1*eta*eta;break;}
                case 6:{res = u2*u1*ksi*eta;break;}
                case 7:{res = -u1*pow(u,p[7])*log(u);break;}
              }
              break;
             }
    }
  if(isnan(res)) res = 0;
  if(isinf(res)) res = 0;
  return res;
}

double lorenzian(double *const p, double x, double y){double u=p[4]*(x-p[0])*(x-p[0])+p[5]*(y-p[1])*(y-p[1])+p[6]*(x-p[0])*(y-p[1]);return p[2]+p[3]/(1+pow(u,p[7]));}
double Jlorenzian(double *const p, double x, double y,int n)
{
    double ksi = x-p[0];
    double eta = y-p[1];
    double u = p[4]*ksi*ksi+p[5]*eta*eta+p[6]*ksi*eta;
    double u1 = -p[3]/((1+pow(u,p[7]))*(1+pow(u,p[7])));
    double u2 = p[7]*pow(u,p[7]-1);
    if(n==0) return -u1*u2*(2*p[4]*ksi+p[6]*eta);
    if(n==1) return -u1*u2*(2*p[5]*eta+p[6]*ksi);
    if(n==2) return 1;
    if(n==3) return 1/(1+pow(u,p[7]));
    if(n==4) return u1*u2*ksi*ksi;
    if(n==5) return u1*u2*eta*eta;
    if(n==6) return u1*u2*ksi*eta;
    if(n==7) return u1*pow(u,p[7])*log(u);
}
bool lorenzianFit(double *const p, const double* fd, int rho, int nit, double minstep)
{
    //initial conditions
    double cm[2];
    centerofmass(cm,fd,rho);
    if((cm[0]<0)||(cm[0]>2.0*rho)||(cm[1]<0)||(cm[1]>2.0*rho)) return false;
    p[0]=cm[0];p[1]=cm[1];
    int k=0;
    double d,J;
    p[2]=0;
    for(int y=0;y<2*rho;y++)
        for(int x=0;x<2*rho;x++)
        {
            d = sqrt((x-rho)*(x-rho)+(y-rho)*(y-rho));
            J = fd[x+y*2*rho];
            if((J!=-10E-9)&&(d>rho-2)){p[2]+=J;k++;}
        }
    p[2] = p[2]/k;
    p[3] = fd[(int)p[0]+(int)p[1]*2*rho] - p[2];
    k=0;
    for(int y=0;y<2*rho;y++)
        for(int x=0;x<2*rho;x++)
        {
            J = fd[x+y*2*rho];
            if(J>p[2]+p[3]/2)k++;
        }
    p[4] = M_PI/k;
    p[5] = p[4];p[6]=0;
    //
    double uwe,itUWE,w;
    double Jac[8];
    double sP[8];
    double dP[8];
    double Hess[64];
    //double eHess[64];
    double iHess[64];
    double lambda = 0;
    long double r;
    double step;
    int it = 0;
    int err;
    //uwe
    k=0;uwe=0;
    for(int y=0;y<2*rho;y++)
        for(int x=0;x<2*rho;x++)
          {J = fd[x+y*2*rho];if(J!=-10E-9){r=J-lorenzian(p,x,y);uwe+=r*r;k++;}}
    //LMA solution
    p[8]=uwe;
    p[19] = k;
    do
    {
        //Jacobian
        for(int i=0;i<8;i++)
        {
            Jac[i]=0;
            for(int y=0;y<2*rho;y++)
                for(int x=0;x<2*rho;x++)
                  {J = fd[x+y*2*rho];if(J!=-10E-9){w=1;Jac[i]+=w*(lorenzian(p,x,y)-J)*Jlorenzian(p,x,y,i);}}
        }
        //Hessian
        for(int i=0;i<8;i++)
            for(int j=0;j<8;j++)
            {
                Hess[j+i*8] = 0;
                for(int y=0;y<2*rho;y++)
                    for(int x=0;x<2*rho;x++)
                      {J = fd[x+y*2*rho];if(J!=-10E-9){w=1;Hess[j+i*8]+=w*Jlorenzian(p,x,y,i)*Jlorenzian(p,x,y,j);}}
            }
        k=0;
        do
        {
            //regularization of the Hessian
            for(int i=0;i<8;i++) Hess[i+i*8]+=lambda*Hess[i+i*8];
            for(int i=0;i<8;i++) sP[i]=p[i];
            //corrections
            ssleg(Hess,dP,Jac,8);
            for(int i=0;i<8;i++) sP[i]-=dP[i];
            err=0;for(int i=0;i<8;i++) if(isnan(sP[i])||isinf(sP[i])){err=1;break;}
            if(err==0) if((sP[4]<0)||(sP[5]<0)||(sP[7]<0.5)||(sP[7]>4)) err=1;
            //itUWE
            if(err==0)
            {
                itUWE=0;
                for(int y=0;y<2*rho;y++)
                    for(int x=0;x<2*rho;x++)
                      {J = fd[x+y*2*rho];if(J!=-10E-9){r=J-lorenzian(sP,x,y);itUWE+=r*r;}}
            }else itUWE = uwe;
            if(isnan(itUWE)) itUWE=uwe;
            k++;
            it++;
            lambda+=0.01*k;
            if(it>nit){p[20]=it-1;return false;}
        }
        while(itUWE>=uwe);
        for(int i=0;i<8;i++) p[i]=sP[i];
        step = (uwe-itUWE)/itUWE;
        uwe = itUWE;
        lambda=0;
    }while(step>minstep);
    p[20]=it-1;
    //error budget
    uwe = uwe/(p[19]-8);
    p[8]=sqrt(uwe);
    invgj(iHess,Hess,8);
    for(int i=0;i<8;i++) p[9+i]=sqrt(uwe*iHess[i+i*8]);
    //fluxes
    p[17]=0;
    p[18]=0;
    for(int y=0;y<2*rho;y++)
        for(int x=0;x<2*rho;x++)
          {J = fd[x+y*2*rho];if(J!=-10E-9){p[17]+=lorenzian(p,x,y)-p[2];p[18]+=J-p[2];}}
    if(p[17]<0)p[17]=0;
    if(p[18]<0)p[18]=0;
    p[17]=-2.5*log10(p[17]/p[19]);
    p[18]=-2.5*log10(p[18]/p[19]);
    return true;
}


bool psffit(double *const p, const double* fd, int rho, int model, int nit, double kappa, double lambda, double regk, int& it, int ipfl)
/*
функция реализует вычисления по алгоритму Левенберга-Марквардта для определения параметров psf
p - вектор параметров (длина массива должна быть равна 21)
p[0] = Xo - пиксельные координаты фотоцентра изображения звездообразного объекта
p[1] = Yo
p[2] = Jo - уровень фона. Не вычисляется в данной функции, вычисляется вне функции и передается во входном массиве p
p[3]= Jmax - уровень максимума psf
p[4] = A, p[5] = B, p[6] = C - параметры, описывающие ширину профиля и его ориентировку и эллиптичность
p[7] = delta - степень psf

fd - массив отсчетов в пределах апертуры.
model - модель PSF (0 - профиль Лоренца, 1 - профиль Моффата, 2 - профиль Гаусса со степенью)
rho - радиус апертуры
nit - максимально-возможное число успешных итераций (опыт подсказывает, что лучше ограничиться значением в пределах 20 -50)
kappa - критерий завершения вычислений, это отношение длины вектора поправок к корню из дисперсии последних шести векторов поправок. Это своеобразный критерий сходимости, возможно не самый лучший.? азумные значения уместно брать в пределах от 0.01 до 0.1. То есть i-тая поправка лежит глубоко в пределах дисперсии последних поправок.

lambda - регуляризующий коэффициент. Обычно на входе составляет 0.001 - 0.01. Показывает какая доля от каждого диагонального элемента Гессиана должна к нему прибавляться.
regk - коэффициент для управления значением lambda. При успешной итерации lambda уменьшается в соответствие с regk в степени числа неуспешных итераций (успешная итерация - итерация, при которой уменьшается сумма квадратов невязок).
В случае неуспешной итерации lambda умножается на regk.

it - можно передать 0-ое значение. После выхода из функции it будет содержать число успешных итераций

ipfl - флаг задающий способ вычисления начальных значений Xo,Yo. Если ipfl = 0 - по максимальному пикселю (для ПЗС - это лучше), ipfl = 1 - по центру масс (предполагается, что это лучше подходит для сканированных изображений фотопластинок)

На выходе в массиве p содержатся: элементы 0 - 7 - значения параметров профиля как описано выше.

p[8] = ошибка единицы веса для последней итерации
элементы 9 - 16 - оценки СКО соответствующих параметров 0-7
p[17] - значение десятичного логарифма интеграла psf над фоном в пределах апертуры
p[18] - значение десятичного логарифма суммы отсчетов на пикселях в пределах апертуры
Последние два значения характеризуют поток излучения от измеряемого объекта
p[19] - число пикселей в пределах апертуры
p[20] - общее число итераций (и успешных и неуспешных)
*/
{
  return lorenzianFit(p,fd,rho,nit,kappa);
}

bool ringfit(double *const p, const double* fd, int rho2)
{
  int num = 0;
  double J;
  for(int y=0;y<2*rho2;y++)
    for(int x=0;x<2*rho2;x++)
      {J=fd[y*2*rho2+x];if(J!=-10E-9){num++;}}
  double *C = new double[num*6];
  double *L = new double[num];
  int k=0;
  for(int y=0;y<2*rho2;y++)
    for(int x=0;x<2*rho2;x++)
      {
        J=fd[y*2*rho2+x];
        if(J!=-10E-9)
          {
            L[k]=J;
            C[k*6 + 0] =x*x;
            C[k*6 + 1] =y*y;
            C[k*6 + 2] =x*y;
            C[k*6 + 3] =x;
            C[k*6 + 4] =y;
            C[k*6 + 5] =1;
            k++;
          }
      }
  slsm(6, num, p, C, L);
  delete[] C;
  delete[] L;
  for(int i=0;i<6;i++) if(isnan(p[i])||isinf(p[i]))return false;
  return true;
}

void subtractSurface(const double* p, double *const fd, double bp, int rho)
{
  double J;
  for(int y=0;y<2*rho;y++)
    for(int x=0;x<2*rho;x++)
      {J=fd[y*2*rho+x];if(J!=-10E-9){fd[y*2*rho+x] = J-(p[0]*x*x+p[1]*y*y+p[2]*x*y+p[3]*x+p[4]*y+p[5])+bp;}}
}

double meanRingValue(const double* fd, int rho2)
{
  int num = 0;
  double J,meanJ=0;
  for(int y=0;y<2*rho2;y++)
    for(int x=0;x<2*rho2;x++)
      {J=fd[y*2*rho2+x];if(J!=-10E-9){meanJ+=J;num++;}}
  meanJ = meanJ/num;
  return meanJ;
}

/* Cubic equation solution. Real coefficients case.

   int Cubic(double *x,double a,double b,double c);
   Parameters:
   x - solution array (size 3). On output:
       3 real roots -> then x is filled with them;
       1 real + 2 complex -> x[0] is real, x[1] is real part of
                             complex roots, x[2] - non-negative
                             imaginary part.
   a, b, c - coefficients, as described
   Returns: 3 - 3 real roots;
            1 - 1 real root + 2 complex;
            2 - 1 real root + complex roots imaginary part is zero
                (i.e. 2 real roots).
*/



int Cubic(double *x,double a,double b,double c)
{
  double q,r,r2,q3;
  q=(a*a-3.*b)/9.; r=(a*(2.*a*a-9.*b)+27.*c)/54.;
  r2=r*r; q3=q*q*q;
  if(r2<q3) {
    double t=acos(r/sqrt(q3));
    a/=3.; q=-2.*sqrt(q);
    x[0]=q*cos(t/3.)-a;
    x[1]=q*cos((t+M_2PI)/3.)-a;
    x[2]=q*cos((t-M_2PI)/3.)-a;
    return(3);
  }
  else {
    double aa,bb;
    if(r<=0.) r=-r;
    aa=-pow(r+sqrt(r2-q3),1./3.);
    if(aa!=0.) bb=q/aa;
    else bb=0.;
    a/=3.; q=aa+bb; r=aa-bb;
    x[0]=q-a;
    x[1]=(-0.5)*q-a;
    x[2]=(sqrt(3.)*0.5)*fabs(r);
    if(x[2]==0.) return(2);
    return(1);
  }
}

void angletr(double X0, double Y0, double X1, double Y1,double X2, double Y2, double& angle, double& ratio)
{
    double r1 = sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0));
    double r2 = sqrt((X2-X0)*(X2-X0)+(Y2-Y0)*(Y2-Y0));
    double V1x = (X1-X0)/r1;
    double V1y = (Y1-Y0)/r1;
    double V2x = (X2-X0)/r2;
    double V2y = (Y2-Y0)/r2;
    angle = acos(V1x*V2x+V1y*V2y)*180/M_PI;
    if(r1<r2) ratio = r2/r1; else ratio = -r1/r2;
}

bool matchstars(int *tangNum,const  double *ksi,const  double *eta,
                int *pixNum,const  double *X,const  double *Y,
                int Nstang, int Nspix, double err, double eAng, double eRatio,
                int matchN)
/*
  процедура отождествления звезд звездного поля (ПЗС-кадра) с данными астрометрического каталога
  int *tangNum - массив, содержащий номера звезд с тангенциальными координатами.
  Формируется до входа в процедуру. Эти номера по сути - произвольные идентификаторы
  const  double *ksi,const  double *eta, - массивы соответствующих тангенциальных координат
  int *pixNum,const  - массив, содержащий номера звезд с измеренными
  (пиксельными) координатами.
  double *X,const  double *Y, - массивы измеренных (пиксельных) координат
  Формируется до входа в процедуру.
  int Nstang, - число элементов в массивах int *tangNum,const  double *ksi,const  double *eta,
  int Nspix, - число элементов в массивах int *pixNum,const  double *X,const  double *Y
  double err, - точность привязки (в пикселях) - с какой точностью должны совпадать координаты звезд из двух массивов
  double eAng, - величина разности углов, начиная с которой они трактуются как равные
  double eRatio, - величина разности соотношений сторон, начиная с которой они считаются равными
  int matchN - минимальное число звезд, необходимое для отождествления
*/
{
    int nextstar;
    int *triads=new int[Nspix*(Nspix-1)*(Nspix-2)*3];
    double *Spix=new double[Nspix*(Nspix-1)*(Nspix-2)*2];
    double angle, ratio;
    int tcount=0;
    for(int i=0;i<Nspix-2;i++)
        for(int j=i+1;j<Nspix-1;j++)
            for(int k=j+1;k<Nspix;k++)
            {
                //if(tcount==Nspix*(Nspix-1)*(Nspix-2)) return true;
                angletr(X[i],Y[i],X[j],Y[j],X[k],Y[k],angle,ratio);
                Spix[tcount*2+0] = angle;Spix[tcount*2+1] = fabs(ratio);
                triads[tcount*3+0] = i;
                if(ratio>0){triads[tcount*3+1] = j;triads[tcount*3+2] = k;}
                else{triads[tcount*3+1] = k;triads[tcount*3+2] = j;}
                tcount++;
            }
    //
    //return true;
    int *Triads = new int [Nstang*(Nstang-1)*(Nstang-2)*3];
    double *Stang = new double [Nstang*(Nstang-1)*(Nstang-2)*2];
    //Triads[Nstang*(Nstang-1)*(Nstang-2)-1][2] = 0;
    //ratio = eta[Nstang-1];
    //return true;
    int Tcount=0;ratio=0;angle=0;
    for(int i=0;i<Nstang-2;i++)
        for(int j=i+1;j<Nstang-1;j++)
            for(int k=j+1;k<Nstang;k++)
            {
                angletr(ksi[i],eta[i],ksi[j],eta[j],ksi[k],eta[k],angle,ratio);
                Stang[Tcount*2+0] = angle;Stang[Tcount*2+1] = fabs(ratio);
                Triads[Tcount*3+0] = i;
                if(ratio>0){Triads[Tcount*3+1] = j;Triads[Tcount*3+2] = k;}
                else{Triads[Tcount*3+1] = k;Triads[Tcount*3+2] = j;}
                Tcount++;
            }
    //return true;
    double Xp[3];double Yp[3];double M[9];double Ax[3];double Ay[3];int Ns;double xs,ys;
    /////////////
    double *xp;double *yp;double *Cp;double *W;int *exclindx;int *exclindy;int rnx,rny;double uwex,uwey;
    double zx[3]; double zy[3];double Dx[9];double Dy[9];
    ////////////
    //double er;s
    int ns;
    if(Tcount>tcount) ns = tcount*3; else ns = Tcount*3;
    int npix[ns];
    int ntang[ns];
    int ms = 0;
    for(int i=0;i<Tcount;i++)
       for(int j=0;j<tcount;j++)
         {
            if((fabs(Stang[i*2+0]-Spix[j*2+0])<eAng)&&(fabs(Stang[i*2+1]-Spix[j*2+1])<eRatio))
            {
                for(int k=0;k<3;k++)
                {
                    Xp[k]=X[triads[j*3+k]];
                    Yp[k]=Y[triads[j*3+k]];
                    M[k*3+0]=ksi[Triads[i*3+k]];
                    M[k*3+1]=eta[Triads[i*3+k]];
                    M[k*3+2]=1;
                }
                ssleg(M,Ax,Xp,3);ssleg(M,Ay,Yp,3);
                Ns=0;
                for(int m=0;m<Nstang;m++)
                {
                    xs = Ax[0]*ksi[m]+Ax[1]*eta[m]+Ax[2];
                    ys = Ay[0]*ksi[m]+Ay[1]*eta[m]+Ay[2];
                    for(int n=0;n<Nspix;n++) if(sqrt((xs-X[n])*(xs-X[n])+(ys-Y[n])*(ys-Y[n]))<err*err){Ns++;break;}
                    if(Ns>matchN)break;
                }
                if(Ns>matchN)
                {
                    for(int k=0;k<3;k++)
                    {
                        nextstar=0;
                        for(int m=0;m<ms;m++)if((npix[m]==triads[j*3+k])||(ntang[m]==Triads[i*3+k])){nextstar=1;break;}
                        if(nextstar!=1)
                        {
                            npix[ms] = triads[j*3+k];ntang[ms]=Triads[i*3+k];ms++;
                        }
                    }
                }
                 if(ms>matchN)
                 {
                    xp = new double[ms];
                    yp = new double[ms];
                    Cp = new double[ms*3];
                    W = new double[ms];
                    exclindx = new int[ms];
                    exclindy = new int[ms];
                    for(int n=0;n<ms;n++)
                    {
                        xp[n] = X[npix[n]];yp[n] = Y[npix[n]];
                        Cp[n*3+0]=ksi[ntang[n]];Cp[n*3+1]=eta[ntang[n]];Cp[n*3+2]=1;
                        W[n]=1;
                    }
                    iLSM(3,ms,1,exclindx,zx,Cp,xp,uwex,Dx,2,rnx,W);
                    iLSM(3,ms,1,exclindy,zy,Cp,yp,uwey,Dy,2,rny,W);
                    delete[] xp;delete[] yp;delete[] Cp;delete[] W;delete[] exclindx;delete[] exclindy;
                    if(sqrt(uwex+uwey)<err)
                    {
                        //return true;
                        //*
                        for(int n=0;n<Nspix;n++)
                        {
                            pixNum[n] = n;tangNum[n]=-1;
                            for(int m=0;m<Nstang;m++)
                            {
                                xs = zx[0]*ksi[m]+zx[1]*eta[m]+zx[2];
                                ys = zy[0]*ksi[m]+zy[1]*eta[m]+zy[2];
                                if(sqrt((xs-X[n])*(xs-X[n])+(ys-Y[n])*(ys-Y[n]))<err)tangNum[n]=m;
                            }
                        }//*/
                        //for(int n=0;n<Nspix;n++){pixNum[n]=-1;tangNum[n]=-1;}
                        //for(int n=0;n<ms;n++){pixNum[n]=npix[n];tangNum[n]=ntang[n];}
                        delete[] triads;delete[] Triads;delete[] Spix;delete[] Stang;
                        return true;
                    }
                    else{for(int k=0;k<ms-1;k++){npix[k]=npix[k+1];ntang[k]=ntang[k+1];}ms--;}
                 }
            }
         }
    if(ms>2)
    {
        for(int k=0;k<3;k++)
                {
                    Xp[k]=X[npix[k]];
                    Yp[k]=Y[npix[k]];
                    M[k*3+0]=ksi[ntang[k]];
                    M[k*3+1]=eta[ntang[k]];
                    M[k*3+2]=1;
                }
        ssleg(M,Ax,Xp,3);ssleg(M,Ay,Yp,3);
        for(int n=0;n<Nspix;n++)
        {
            pixNum[n] = n;tangNum[n]=-1;
            for(int m=0;m<Nstang;m++)
            {
                xs = Ax[0]*ksi[m]+Ax[1]*eta[m]+Ax[2];
                ys = Ay[0]*ksi[m]+Ay[1]*eta[m]+Ay[2];
                if(sqrt((xs-X[n])*(xs-X[n])+(ys-Y[n])*(ys-Y[n]))<err)tangNum[n]=m;
            }
        }
        delete[] triads;delete[] Triads;delete[] Spix;delete[] Stang;
        return true;
    }
    delete[] triads;delete[] Triads;delete[] Spix;delete[] Stang;
    return false;
}
