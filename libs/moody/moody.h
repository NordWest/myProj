#pragma once
/**
	* Moodys main header, include this in any program that needs to use Moody
	*/

#include "capsule/Capsule.h"

#ifdef CUDALIB
#include "capsule/capsuleBase/CapsuleCUDA.h"
#endif

#ifdef OMPLIB
#include "capsule/capsuleBase/CapsuleOpenMP.h"
#endif

#ifdef TBBLIB
#include "capsule/capsuleBase/CapsuleTBB.h"
#endif

#ifdef SERIAL
#include "capsule/capsuleBase/CapsuleSerial.h"
#endif

/*
* Author      : Dr Carey Pridgeon
* Copyright   : Dr Carey Pridgeon 2010
* Licence     : Licensed under the Apache License, Version 2.0 (the "License");
*             : you may not use this file except in compliance with the License.
*             : You may obtain a copy of the License at
*             : http://www.apache.org/licenses/LICENSE-2.0
*             :
*             : Unless required by applicable law or agreed to in writing,
*             : software distributed under the License is distributed on
*             : an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*             : either express or implied. See the License for the specific
*             : language governing permissions and limitations under the License.
*/
