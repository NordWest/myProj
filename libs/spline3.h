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

#ifndef _spline3_h
#define _spline3_h

#include "ap.h"

/*************************************************************************
Построение таблицы коэффициентов кусочно-линейного сплайна

Входные параметры:
    X           -   абсциссы, массив с нумерацией элементов [0..N-1]
    Y           -   значения функции,
                    массив с нумерацией элементов [0..N-1]
    N           -   число точек, N>=2

Выходные параметры:
    C           -   таблица коэффициентов сплайна для использования в
                    подпрограмме SplineInterpolation

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildlinearspline(ap::real_1d_array x,
     ap::real_1d_array y,
     int n,
     ap::real_1d_array& c);


/*************************************************************************
Построение таблицы коэффициентов кубического сплайна

Входные параметры:
    X           -   абсциссы, массив с нумерацией элементов [0..N-1]
    Y           -   значения функции,
                    массив с нумерацией элементов [0..N-1]
    N           -   число точек, N>=2
    BoundLType  -   тип граничного условия (левая граница)
    BoundL      -   значение первой (или второй, в зависимости от
                    BoundType) производной сплайна на левой границе
    BoundRType  -   тип граничного условия (правая граница)
    BoundR      -   значение первой (или второй, в зависимости от
                    BoundType) производной сплайна на правой границе

Выходные параметры:
    C           -   таблица коэффициентов сплайна для использования в
                    подпрограмме SplineInterpolation

Параметры BoundLType/BoundRType задают тип граничного условия и  принимают
следующие значения:
    * 0, что соответствует граничному условию типа "сплайн, завершающийся
      параболой" (при этом значения BoundL/BoundR игнорируются).
    * 1, что соответствует граничному условию для первой производной
    * 2, что соответствует граничному условию для второй производной

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildcubicspline(ap::real_1d_array x,
     ap::real_1d_array y,
     int n,
     int boundltype,
     double boundl,
     int boundrtype,
     double boundr,
     ap::real_1d_array& c);


/*************************************************************************
Построение таблицы коэффициентов сплайна Эрмита

Входные параметры:
    X           -   абсциссы, массив с нумерацией элементов [0..N-1]
    Y           -   значения функции,
                    массив с нумерацией элементов [0..N-1]
    D           -   значения производной,
                    массив с нумерацией элементов [0..N-1]
    N           -   число точек, N>=2

Выходные параметры:
    C           -   таблица коэффициентов сплайна для использования в
                    подпрограмме SplineInterpolation

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildhermitespline(ap::real_1d_array x,
     ap::real_1d_array y,
     ap::real_1d_array d,
     int n,
     ap::real_1d_array& c);


/*************************************************************************
Построение таблицы коэффициентов сплайна Акимы

Входные параметры:
    X           -   абсциссы, массив с нумерацией элементов [0..N-1]
    Y           -   значения функции,
                    массив с нумерацией элементов [0..N-1]
    N           -   число точек, N>=5

Выходные параметры:
    C           -   таблица коэффициентов сплайна для использования в
                    подпрограмме SplineInterpolation

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildakimaspline(ap::real_1d_array x,
     ap::real_1d_array y,
     int n,
     ap::real_1d_array& c);


/*************************************************************************
Вычисление интерполирующего сплайна

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна.
    X   -   точка, в которой вычисляется значение сплайна

Результат:
    значение сплайна в точке X

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
double splineinterpolation(const ap::real_1d_array& c, double x);


/*************************************************************************
Дифференцирование сплайна

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна.
    X   -   точка, в которой вычисляется значение сплайна

Выходные параметры:
    S   -   значение сплайна в точке X
    DS  -   первая производная в точке X
    D2S -   вторая производная в точке X

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinedifferentiation(const ap::real_1d_array& c,
     double x,
     double& s,
     double& ds,
     double& d2s);


/*************************************************************************
Копирование

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна.

Выходные параметры:
    CC  -   копия сплайна
    

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinecopy(const ap::real_1d_array& c, ap::real_1d_array& cc);


/*************************************************************************
Распаковка сплайна

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна.

Выходные параметры:
    N   -   число точек, на основе которых был построен сплайн
    Tbl -   таблица коэффициентов сплайна. Массив с нумерацией элементов
            [0..N-2, 0..5].
            Для I = 0..N-2:
                Tbl[I,0] = X[i]
                Tbl[I,1] = X[i+1]
                Tbl[I,2] = C0
                Tbl[I,3] = C1
                Tbl[I,4] = C2
                Tbl[I,5] = C3
            Сплайн имеет вид:
                t = x-x[i]
                S(x) = C0 + C1*t + C2*t^2 + C3*t^3

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void splineunpack(const ap::real_1d_array& c, int& n, ap::real_2d_array& tbl);


/*************************************************************************
Линейная замена аргумента сплайна

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна S(x).
    A, B-   коэффициенты преобразования x = A*t + B

Выходные параметры:
    C   -   преобразованный сплайн

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinelintransx(ap::real_1d_array& c, double a, double b);


/*************************************************************************
Линейное преобразование сплайна

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна S(x).
    A, B-   коэффициенты преобразования S2(x) = A*S(x) + B

Выходные параметры:
    C   -   преобразованный сплайн

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinelintransy(ap::real_1d_array& c, double a, double b);


/*************************************************************************
Вычисление определенного интеграла от сплайна

Входные параметры:
    C   -   массив коэффициентов, вычисленный подпрограммой для
            построения сплайна.
    X   -   точка, в которой вычисляется интеграл

Результат:
    значение интеграла на отрезке [A, X], где A - левая граница интервала,
    на котором построен сплайн.

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
double splineintegration(const ap::real_1d_array& c, double x);


/*************************************************************************
Obsolete subroutine, left for backward compatibility.
*************************************************************************/
void spline3buildtable(int n,
     const int& diffn,
     ap::real_1d_array x,
     ap::real_1d_array y,
     const double& boundl,
     const double& boundr,
     ap::real_2d_array& ctbl);


/*************************************************************************
Obsolete subroutine, left for backward compatibility.
*************************************************************************/
double spline3interpolate(int n, const ap::real_2d_array& c, const double& x);


#endif
