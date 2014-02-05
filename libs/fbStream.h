#ifndef FBSTREAM_H
#define FBSTREAM_H

#include "stdlib.h"
#include "math.h"
//#include "conio.h"
#include <string>
#include <iostream>
#include <fstream>



#ifndef COMF_H
#include "comfunc.h"
#endif


class fbStream;				//Базовый класс потокового буферезированного файла
class fbStreamStr;			//Расширенный класс с поддержкой строк данных с ключевыми записями
class fbStreamLine;			//Класс, расширяющий предыдущий на поддержку построчного представления

//#define STRLENBUFF 1024

using namespace std;


class fbStream
{
public:
	char *fn;								//имя файла

	string *buf;							//буффер, содержащий весь файл
//	fstream *fS;

	size_t size;							//длинна буфера
	int is_edited;							//ключ измененности буфера

	fbStream();
	fbStream(char *fname);
	~fbStream();
	int init(char* fname);					//инициализация

    int Get(char *str, char *name, int kpos, int szstr, int startpos = 0);	//получить строку str длинной szstr, содержащую ключевую подстроку name в позиции kpos
	int Del(char *name, int kpos, int szstr);				//удалить строку длинной szstr, содержащую ключевую подстроку name в позиции kpos
	int Insert(char *str, int pos);							//вставить строку str в позицию pos
	int Push(char *str);									//добавить строку str в конец файла
	void Free();											//отчистить буфер
	int Save();												//сохранить буфер в файл fn
	int SaveAs(char *fname);								//сменить имя файла fn на fname и сохранить

private:
	void get_file_size();									//правильно определить размер файла
	void defSize();											//неправильно определить размер файла
	void defSizeBuf();										//определить размер буфера
};

class fbStreamStr : public fbStream
{
public:
//	int GetRecName(char *name);								//получить строку по имени

	int sizestr, keypos, is_istr;							//размер строк, позиция ключевой подстроки, ключ "инициализация работы строк"
	char *str;												//рабочая строка

	fbStreamStr();
	fbStreamStr(char *fn);
	~fbStreamStr();
	
	void init_str(int sizes, int kpos);						//инициализация работы строк

	int GetStr(char *name);									//получить строку, содержащую подстроку name
	int DelStr(char *name);									//удалить строку, содержащую подстроку name
	int PushStr(char *str);									//вставить строку в конец файла
};

class fbStreamLine : public fbStreamStr
{
public:
	int begpos, is_iline, nline;							//позиция начала строк

	fbStreamLine();
	fbStreamLine(char *fn);
	~fbStreamLine();

	void init_line(int bpos);								//инициализация работы по начальному положению в файле
	void init_line(char *bstr);								//инициализация работы по последней строке перед линейными строками

	int GetLine(int linepos);								//получить строковую запись с номером linepos [0-nline]
	int DelLine(int linepos);								//удалить строковую запись с номером linepos [0-nline]
	int AddLine(char *astr, int linepos);					//добывить строковую запись в позицию linepos [0-nline]	
	int PushLine(char *astr);								//добывить строковую запись в позицию linepos=nline
};

#endif
