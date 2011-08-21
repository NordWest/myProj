/*************************************************************************
Copyright (c) 2007, Sergey Bochkanov (ALGLIB project).

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/

#ifndef _spline2d_h
#define _spline2d_h

#include "ap.h"

#include "spline3.h"


/*************************************************************************
Построение билинейного интерполирующего сплайна

Входные параметры:
    X   -   массив абсцисс прямоугольной сетки на которой строится сплайн.
            Нумерация элементов: [0..N-1]
    Y   -   массив ординат прямоугольной сетки на которой строится сплайн.
            Нумерация элементов: [0..M-1]
    F   -   матрица значений функции в узлах сетки.
            Нумерация элементов: [0..M-1, 0..N-1]
    M   -   размер сетки по оси Y, M>=2
    N   -   размер сетки по оси X, N>=2
    
Выходные параметры:
    C   -   таблица  коэффициентов  для   использования   в   подпрограмме
            SplineInterpolation2D.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void buildbilinearspline(ap::real_1d_array x,
     ap::real_1d_array y,
     ap::real_2d_array f,
     int m,
     int n,
     ap::real_1d_array& c);


/*************************************************************************
Построение бикубического интерполирующего сплайна

Входные параметры:
    X   -   массив абсцисс прямоугольной сетки на которой строится сплайн.
            Нумерация элементов: [0..N-1]
    Y   -   массив ординат прямоугольной сетки на которой строится сплайн.
            Нумерация элементов: [0..M-1]
    F   -   матрица значений функции в узлах сетки.
            Нумерация элементов: [0..M-1, 0..N-1]
    M   -   размер сетки по оси Y
    N   -   размер сетки по оси X

Выходные параметры:
    C   -   таблица  коэффициентов  для   использования   в   подпрограмме
            SplineInterpolation2D.

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void buildbicubicspline(ap::real_1d_array x,
     ap::real_1d_array y,
     ap::real_2d_array f,
     int m,
     int n,
     ap::real_1d_array& c);


/*************************************************************************
Интерполяция двухмерным (билинейным или бикубическим) сплайном

Входные параметры:
    C   -   таблица коэффициентов, рассчитанная  подпрограммой  построения
            сплайна.
    X, Y-   точка, в которой вычисляется значение сплайна

Результат:
    Значение интерполирующего сплайна в точке (X,Y)

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
double splineinterpolation2d(const ap::real_1d_array& c, double x, double y);


/*************************************************************************
Интерполяция двухмерным (билинейным или бикубическим) сплайном.
Также осуществлятся вычисление градиента и перекрестной производной.

Входные параметры:
    C   -   таблица коэффициентов, рассчитанная  подпрограммой  построения
            сплайна.
    X, Y-   точка, в которой вычисляется значение сплайна

Результат:
    F   -   значение интерполирующего сплайна
    FX  -   dF/dX
    FY  -   dF/dY
    FXY -   d2F/dXdY

  -- ALGLIB PROJECT --
     Copyright 05.07.2007 by Bochkanov Sergey
*************************************************************************/
void splinedifferentiation2d(const ap::real_1d_array& c,
     double x,
     double y,
     double& f,
     double& fx,
     double& fy,
     double& fxy);


/*************************************************************************
Распаковка сплайна

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения билинейного или бикубического сплайна.

Выходные параметры:
    M   -   размер сетки по оси Y
    N   -   размер сетки по оси X
    Tbl -   таблица коэффициентов сплайна. Массив с  нумерацией  элементов
            [0..(N-1)*(M-1)-1, 0..19]. Каждая строка матрицы соответствует
            одной ячейке сетки. Сетка обходится сначала слева направо (т.е.
            в порядке возрастания X), затем снизу вверх  (т.е.  в  порядке
            возрастания Y).
            Формат матрицы:
            For I = 0...M-2, J=0..N-2:
                K =  I*(N-1)+J
                Tbl[K,0] = X[j]
                Tbl[K,1] = X[j+1]
                Tbl[K,2] = Y[i]
                Tbl[K,3] = Y[i+1]
                Tbl[K,4] = C00
                Tbl[K,5] = C01
                Tbl[K,6] = C02
                Tbl[K,7] = C03
                Tbl[K,8] = C10
                Tbl[K,9] = C11
                ...
                Tbl[K,19] = C33
            На каждой из ячеек сетки сплайн имеет вид:
                t = x-x[j]
                u = y-y[i]
                S(x) = SUM(c[i,j]*(x^i)*(y^j), i=0..3, j=0..3)

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void splineunpack2d(const ap::real_1d_array& c,
     int& m,
     int& n,
     ap::real_2d_array& tbl);


/*************************************************************************
Линейная замена аргумента двухмерного сплайна

Входные параметры:
    C       -   массив коэффициентов, вычисленный подпрограммой для
                построения сплайна S(x).
    AX, BX  -   коэффициенты преобразования x = A*t + B
    AY, BY  -   коэффициенты преобразования y = A*u + B

Выходные параметры:
    C   -   преобразованный сплайн

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransxy(ap::real_1d_array& c,
     double ax,
     double bx,
     double ay,
     double by);


/*************************************************************************
Линейное преобразование двухмерного сплайна

Входные параметры:
    C       -   массив коэффициентов, вычисленный подпрограммой для
                построения сплайна S(x).
    A, B    -   коэффициенты преобразования S2(x,y) = A*S(x,y) + B

Выходные параметры:
    C   -   преобразованный сплайн

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dlintransf(ap::real_1d_array& c, double a, double b);


/*************************************************************************
Копирование двухмерного сплайна.

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна.

Выходные параметры:
    CC  -   копия сплайна

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void spline2dcopy(const ap::real_1d_array& c, ap::real_1d_array& cc);


/*************************************************************************
Ресэмплирование бикубическим сплайном.

    Процедура получает значения  функции  на  сетке  OldWidth*OldHeight  и
путем интерполяции бикубическим  сплайном  вычисляет  значения  функции  в
узлах Декартовой сетки размером NewWidth*NewHeight. Новая сетка может быть
как более, так и менее плотная, чем старая.

Входные параметры:
    A           - массив значений функции на старой сетке.
                  Нумерация элементов [0..OldHeight-1,
                  0..OldWidth-1]
    OldHeight   - старый размер сетки, OldHeight>1
    OldWidth    - старый размер сетки, OldWidth>1
    NewHeight   - новый размер сетки, NewHeight>1
    NewWidth    - новый размер сетки, NewWidth>1

Выходные параметры:
    B           - массив значений функции на новой сетке.
                  Нумерация элементов [0..NewHeight-1,
                  0..NewWidth-1]

  -- ALGLIB routine --
     15 May, 2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void bicubicresamplecartesian(const ap::real_2d_array& a,
     int oldheight,
     int oldwidth,
     ap::real_2d_array& b,
     int newheight,
     int newwidth);


/*************************************************************************
Ресэмплирование билинейным сплайном.

    Процедура получает значения  функции  на  сетке  OldWidth*OldHeight  и
путем интерполяции билинейным  сплайном  вычисляет  значения   функции   в
узлах Декартовой сетки размером NewWidth*NewHeight. Новая сетка может быть
как более, так и менее плотная, чем старая.

Входные параметры:
    A           - массив значений функции на старой сетке.
                  Нумерация элементов [0..OldHeight-1,
                  0..OldWidth-1]
    OldHeight   - старый размер сетки, OldHeight>1
    OldWidth    - старый размер сетки, OldWidth>1
    NewHeight   - новый размер сетки, NewHeight>1
    NewWidth    - новый размер сетки, NewWidth>1

Выходные параметры:
    B           - массив значений функции на новой сетке.
                  Нумерация элементов [0..NewHeight-1,
                  0..NewWidth-1]

  -- ALGLIB routine --
     09.07.2007
     Copyright by Bochkanov Sergey
*************************************************************************/
void bilinearresamplecartesian(const ap::real_2d_array& a,
     int oldheight,
     int oldwidth,
     ap::real_2d_array& b,
     int newheight,
     int newwidth);


/*************************************************************************
Obsolete subroutine for backwards compatibility
*************************************************************************/
void bicubicresample(int oldwidth,
     int oldheight,
     int newwidth,
     int newheight,
     const ap::real_2d_array& a,
     ap::real_2d_array& b);


/*************************************************************************
Obsolete subroutine for backwards compatibility
*************************************************************************/
void bilinearresample(int oldwidth,
     int oldheight,
     int newwidth,
     int newheight,
     const ap::real_2d_array& a,
     ap::real_2d_array& b);


#endif
