/*************************************************************************
Cephes Math Library Release 2.8:  June, 2000
Copyright by Stephen L. Moshier

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from C to
      pseudocode.

See subroutines comments for additional copyrights.

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

#ifndef _gammaf_h
#define _gammaf_h

#include "ap.h"
#include "ialglib.h"

/*************************************************************************
Вычисление гамма-функции.

Входные параметры:
    X   -   аргумент

Допустимые значения:
    0 < X < 171.6
    -170 < X < 0, X не целое.

Относительная погрешность вычисления:

    ОБЛАСТЬ         #ТЕСТОВ        МАКС.          СРЕДН.
    (-170, -33)       20000      2.3e-15         3.3e-16
    (-33,   33)       20000      9.4e-16         2.2e-16
    ( 33,  171.6)     20000      2.3e-15         3.2e-16

Cephes Math Library Release 2.8:  June, 2000
Original copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
Translated to AlgoPascal by Bochkanov Sergey (2005, 2006).
*************************************************************************/
double gamma(double x);


/*************************************************************************
Вычисление натурального логарифма модуля гамма-функции.

Входные параметры:
    x       -   точка, в которой вычисляем логарифм:

Выходные параметры:
    SgnGam  -   знак, который имеет гамма-функция.
                Либо +1, либо -1.

Результат:
    натуральный логарифм модуля гамма-функции

Допустимые значения:
    0 < X < 2.55e305
    -2.55e305 < X < 0, X не целое.

Погрешность аппроксимации (относительная для значений функции
больших 1, иначе - абсолютная):

domain               #trials   peak        rms
0, 3                 28000     5.4e-16     1.1e-16
2.718, 2.556e305     40000     3.5e-16     8.3e-17

Следующий тест приводит относительную погрешность, хотя для некоторых
точек ошибка может быть больше, чем указано в таблице:
-200, -4             10000     4.8e-16     1.3e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
Translated to AlgoPascal by Bochkanov Sergey (2005, 2006).
*************************************************************************/
double lngamma(double x, double& sgngam);


#endif

