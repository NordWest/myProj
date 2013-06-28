//#include "stdafx.h"

#include "fbStream.h"

////////////////////	fbStream	/////////////////////////////

fbStream::fbStream()
{
	this->fn = new char[FNLEN];
	this->buf = NULL;
	this->size = -1;
	this->is_edited = 0;
}

fbStream::fbStream(char *fname)
{
	fbStream();	
	this->init(fname);
}

fbStream::~fbStream()
{
	if(fn!=NULL) delete [] this->fn;
	this->fn = NULL;
	
	if(buf!=NULL) delete(this->buf);

	this->size = -1;
	this->is_edited = -1;
}

void fbStream::get_file_size()
{
    FILE *fp = fopen( fn, "rb" );
    if ( !fp )
    {
		size = 0;
        return ;
    }
    fpos_t position;
    
    if ( fgetpos(fp, &position) )       //запоминаем текущую позицию
    {
        fclose(fp);
		size = -1;
        return ;
    }        //ошибка получения текущей позиции файла

    fseek(fp, 0, SEEK_END);          //смещаемся в конец файла
    size = ftell(fp);                 //читаем положение указателя в конце файла
    
    if ( fsetpos(fp, &position) )       //кладем откуда взяли (позицию файла)
    {
        fclose(fp);
		size = -1;
        return ;
    }        //ошибка установки позиции файла

    fclose(fp);
//    return res;
}


void fbStream::defSize()
{
	this->size = 0;
	ifstream infile( fn, std::ios::in );
    if ( !infile )
    {
		infile.open(fn, ios_base::in | ios_base::trunc);
        return;
    }

	infile.seekg(0, ios_base::end);
	this->size = infile.tellg();

	infile.close();
}

void fbStream::defSizeBuf()
{
	if(this->is_edited)
	{
		this->size = buf->length();
	}
}

int fbStream::init(char* fname)
{
	strncpy(this->fn, fname, FNLEN);
	this->get_file_size();

    ifstream infile;
    infile.open(fn, ios_base::binary );
    if (!infile.is_open())
    {
        infile.open(fn, ios_base::binary | ios_base::trunc | ios_base::out | ios_base::in);
        //ios_base::trunc);
        if (!infile.is_open()) return 1;
    }

	is_edited = 0;

	char *tstr = new char[this->size+1];
	strcpy(tstr, "\0");
	this->buf = new string();

	infile.read(tstr, this->size);
	strcpy(&tstr[size], "\0");

	this->buf->insert(0, tstr);
	buf->resize(size, '!');

	delete [] tstr;

	return 0;
}



void fbStream::Free()
{
	buf->erase();
	buf->resize(0);
	this->is_edited = 1;
}

int fbStream::Save()
{
	if(!this->is_edited) return 0;

	std::ofstream outfile( this->fn, std::ios::binary | ios_base::trunc);
	size = buf->size();
	char *tstr = new char[(int)size];
	strcpy(tstr, buf->data());
	outfile.write(tstr, size );
	outfile.close();
	this->is_edited = 0;

	return 0;
}

int fbStream::SaveAs(char *fname)
{
	strncpy(fn, fname, FNLEN);
	return(this->Save());
}
/*
int fbStream::Get(char *str, char *name, int kpos, int szstr)
{
	basic_string <char>::size_type spos;
	static const basic_string <char>::size_type npos = -1;
        char *tname = new char[256];
        int res;
        sprintf(tname, " %s ", name);
        spos = buf->find(tname);
        res = 1;
	if(spos!=npos)
	{
                buf->copy(str, szstr, spos-kpos+1);
		strcpy(&str[szstr], "\0");
                res = 0;
	}

        delete [] tname;
        return res;
}
*/
int fbStream::Get(char *str, char *name, int kpos, int szstr, int startpos)
{
    basic_string <char>::size_type spos;
    static const basic_string <char>::size_type npos = -1;
        char *tname = new char[256];
        int res;
        sprintf(tname, " %s ", name);
        spos = buf->find(tname, startpos);
        res = 1;
    if(spos!=npos)
    {
                buf->copy(str, szstr, spos-kpos+1);
        strcpy(&str[szstr], "\0");
                res = 0;
    }

        delete [] tname;
        return res;
}

int fbStream::Del(char *name, int kpos, int szstr)
{
	basic_string <char>::size_type spos;
	static const basic_string <char>::size_type npos = -1;
	spos = buf->find(name);

	char *tstr = new char[szstr];
	if(spos!=npos)
	{
		strcpy(tstr, "\0");
		buf->replace(spos-kpos, szstr, tstr);
		size -= szstr;
		buf->resize(size);
		this->is_edited = 1;

		delete [] tstr;
		return 0;
	}

	delete [] tstr;
	return 2;
}

int fbStream::Insert(char *str, int pos)
{
	int szstr = sizeof(str);
//	char *tstr = new char[szstr];
	size += szstr;
	buf->insert(pos, str);
	buf->resize(size);

	this->is_edited = 1;
	return 0;
}

int fbStream::Push(char *str)
{
	int szstr = sizeof(str);
//	char *tstr = new char[szstr];
	size += szstr;
	buf->append(str);
	buf->resize(size);

	this->is_edited = 1;
	return 0;
}

////////	fbStreamStr		/////////////////////////////////

fbStreamStr::fbStreamStr() : fbStream()
{
	this->str = NULL;
	this->keypos = -1;
	this->sizestr = -1;
	is_istr = 0;
}

fbStreamStr::fbStreamStr(char *fname) : fbStream(fname)
{
	this->str = NULL;
	this->keypos = -1;
	this->sizestr = -1;
	is_istr = 0;
}

fbStreamStr::~fbStreamStr()
{
//	~fbStream();
	if(str!=NULL) delete [] str;
	this->keypos = -1;
	this->sizestr = -1;
	is_istr = 0;
}

void fbStreamStr::init_str(int sizes, int kpos)
{
	this->sizestr = sizes;
	this->keypos = kpos;
	this->is_istr = 1;
//	if(str!=NULL) delete [] str;
	this->str = new char[sizestr];
}

int fbStreamStr::GetStr(char *name)
{
	if(!is_istr) return 1;
	return(this->Get(str, name, keypos, sizestr));
}

int fbStreamStr::DelStr(char *name)
{
	if(!is_istr) return 1;
	return(this->Del(name, keypos, sizestr));
}

int fbStreamStr::PushStr(char *str)
{
	if(!is_istr) return 1;
	char *tstr = new char[sizestr];
	if(this->Get(tstr, str, 0, sizestr)) return 2;
	return(this->Push(str));
}

////////	fbStreamLine		/////////////////////////////////

fbStreamLine::fbStreamLine() : fbStreamStr()
{
	this->begpos = -1;
	this->is_iline = 0;
	this->nline = -1;
}

fbStreamLine::fbStreamLine(char *fname) : fbStreamStr(fname)
{
	this->begpos = -1;
	this->is_iline = 0;
	this->nline = -1;
}

fbStreamLine::~fbStreamLine()
{
	this->begpos = -1;
	this->is_iline = 0;
	this->nline = -1;
}

void fbStreamLine::init_line(int bpos)
{
	this->begpos = bpos;
	this->is_iline = 1;
	this->nline = (size-begpos)/sizestr;
}

void fbStreamLine::init_line(char *bstr)
{
	basic_string <char>::size_type spos;
	static const basic_string <char>::size_type npos = -1;
	spos = buf->find(bstr);
	if(spos!=npos)
	{
		begpos = spos + strlen(bstr) + 1;
		this->is_iline = 1;
		this->nline = (size-begpos)/sizestr;
	}
}

int fbStreamLine::GetLine(int linepos)
{
	if(!is_istr||!is_iline) return 1;
	strncpy(str, "\0", 1);
	if(buf->copy(str, sizestr, begpos + sizestr*linepos)!=sizestr) return 1;
	strncpy(&str[sizestr], "\0", 1);

	return 0;
}

int fbStreamLine::DelLine(int linepos)
{
	if(!is_istr||!is_iline) return 1;
	char *tstr = new char[1];
	strncpy(tstr, "\0", 1);
	buf->replace(begpos + sizestr*linepos, sizestr, tstr);
	size -= sizestr;
	buf->resize(size);
	nline--;
	this->is_edited = 1;

	delete [] tstr;
	return 0;
}

int fbStreamLine::AddLine(char *astr, int linepos)
{
	if(!is_istr||!is_iline) return 1;
	strncpy(&astr[sizestr], "\0", 1);
	buf->insert(begpos + sizestr*linepos, astr, sizestr);
	size += sizestr;
	buf->resize(size);
	nline++;
	this->is_edited = 1;

	return 0;
}

int fbStreamLine::PushLine(char *astr)
{
	if(!is_istr||!is_iline) return 1;
	strncpy(&astr[sizestr], "\0", 1);
	this->PushStr(astr);
	nline++;
	this->is_edited = 1;

	return 0;
}
