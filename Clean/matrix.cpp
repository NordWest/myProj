#include "matrix.h"

matrix::matrix(int I, int J)
{
	this->M = new matr;
	this->M->i = 0;
	this->M->j = 0;
	this->M->value = 0;
	this->M->next = NULL;
	this->n = I;
	this->m = J;
}

matrix::matrix()
{
	this->M = new matr;
	this->M->i = 0;
	this->M->j = 0;
	this->M->value = 0;
	this->M->next = NULL;
	this->n = 0;
	this->m = 0;
}

matrix::matrix(matrix* M)
{
	matr *tut;
	this->M = new matr;
	this->M->i = 0;
	this->M->j = 0;
	this->M->value = 0;
	this->M->next = NULL;
	this->n = M->n;
	this->m = M->m;
	tut = M->M;
	while(tut != NULL)
	{
		this->set(tut->i, tut->j, tut->value);
		tut = tut->next;
	}
}

matrix::matrix(ifstream *file)
{
	this->M = new matr;
	this->M->i = 0;
	this->M->j = 0;
	this->M->value = 0;
	this->M->next = NULL;



	int i, j, k;
	char *str, *strval, *sim;
	double val;

	str = new char[10000];
	strval = new char[30];
	sim = new char[1];

	i = j = this->n = this->m = 0;

	strcpy(sim, "");
	strcpy(strval, "");

	while(!file->eof())
	{

		file->getline(str, 1000);
//		istrstream sstr(str, 100);
//		cout << strlen(str) << endl;

		for(k=0; k< strlen(str); k++)
		{
			sim[0] = str[k];
//			cout << "sim |" << sim << "|" << endl;
/*
			if(strstr(sim, "\n")!=NULL)
			{
				val = atof(strval);
				if(fabs(val)>e)this->set(i, j, val);
				strcpy(sim, "");
				strcpy(strval, "");
				i++;
				if(j>this->m) this->m = j+1;
				j=0;
				cout << "val= " << val << endl;
				break;
			}
*/
			if(strstr(sim, " ")!=NULL)
			{
				val = atof(strval);
				if(fabs(val)>e)this->set(i, j, val);
				strcpy(sim, "");
				strcpy(strval, "");
				j++;
//				cout << "val= " << val << endl;
				continue;
			}
			strcat(strval, &sim[0]);
		}
		val = atof(strval);
		if(fabs(val)>e)this->set(i, j, val);
		strcpy(sim, "");
		strcpy(strval, "");
		i++;
		if(j>this->m) this->m = j+1;
		j=0;
//		cout << "val= " << strval << endl;
	}
	this->n = i;

}

int matrix::set(int i, int j, double value)
{
	matr *etot, *sled;

	etot = sled = this->M;
	while(sled != NULL)
	{
		if((sled->i == i)&&(sled->j == j)) 
		{
			if(fabs(value)>e)
			{				
				sled->value = value;
				return 5;
			}
			else
			{
				if(this->M == sled)
				{
					this->M->value = 0.0;
					return 4;

				}
				else
				{
					etot->next = sled->next;
					return 3;
				}
			}
		}
		if(((etot->i==i)&&(i==sled->i)&&(etot->j<j)&&(sled->j>j))||((etot->i<=i)&&(i<sled->i)&&(etot->j<j))||((etot->i<i)&&(i<=sled->i)&&(j<sled->j)))
		{
			if(fabs(value)>e)
			{
				etot->next = new matr;
				etot->next->i = i;
				etot->next->j = j;
				etot->next->value = value;
				etot->next->next = sled;
				return 1;
			}
			else
			{
//				etot->next = sled->next;
				return 3;
			}

		}
		etot = sled;
		sled = etot->next;
	}

	if(fabs(value)>e)
	{
		sled = new matr;
		sled->i = i;
		sled->j = j;
		sled->value = value;
		sled->next = NULL;
		etot->next = sled;
		return 2;
	}
	return 3;
}

double matrix::get(int i, int j)
{
	matr *etot, *sled;

	if((this->n<=i)||(this->m<=j))
	{
		cout << "\nerror: get position is out of range\n";
		exit(EXIT_FAILURE);
	}

	etot = sled = this->M;
	while(sled != NULL)
	{
		if((sled->i == i)&&(sled->j == j)) 
		{
			return(sled->value);
		}
		if(((etot->i==i)&&(i==sled->i)&&(etot->j<j)&&(sled->j>j))||((etot->i<=i)&&(i<sled->i)&&(etot->j<j))||((etot->i<i)&&(i<=sled->i)&&(j<sled->j)))
		{
			return(0.0);
		}
		etot = sled;
		sled = etot->next;
	}


	return(0.0);
}

int matrix::add(int i, int j, double value)
{
	this->set(i, j, this->get(i, j)+value);
	return 0;
}

int matrix::ded(int i, int j, double value)
{
	this->set(i, j, this->get(i, j)-value);
	return 0;
}


char* matrix::print()
{
	int i, j;
	matr *cur;
	char *str, *val;
	int size, l;
	l = 15;
	size = (l*2*this->n)*this->m + 1;
	str = new char[size];
	val = new char[l];
	cur = this->M;

	strcpy(str, "");
	for(i = 0; i<this->n; i++)
	{
		for(j = 0; j<this->m; j++)
		{
			if(cur!=NULL)
			{
				if((i==cur->i)&&(j==cur->j))
				{
				if((fabs(cur->value)>99999.0)||(fabs(cur->value)<0.00001)) 					sprintf(val, "%7.2e", cur->value);
				else sprintf(val, "%7f", cur->value);
				//	sprintf(val, "%7.2e", cur->value);
					strncat(str, val, l);
					cur = cur->next;
				}
				else
				{
					strcat(str, "0.0000000000000");
				}
			}
			else
			{
				strcat(str, "0.0000000000000");
			}
			strcat(str, " ");
		}
		strcat(str, "\n");
	}

	return(str);
}

vector& matrix::column(int j)
{
	int i;
	vector *V;

	V = new vector(this->n);

	for(i=0; i<this->n; i++)
	{
		V->set(i, this->get(i, j));
	}

	return *V;
}

vector& matrix::row(int i)
{
	int j;
	vector *V;

	V = new vector(this->m);

	for(j=0; j<this->m; j++)
	{
		V->set(j, this->get(i, j));
	}

	return *V;
}

vector& matrix::diag()
{
	int j;
	vector *V;

	if(!this->is_square())
	{
		cout << endl << "error: matrix is not diag" << endl;
		exit(1);
	}


	V = new vector(this->m);

	for(j=0; j<this->m; j++)
	{
		V->set(j, this->get(j, j));
	}

	return *V;
}


matrix& operator+(matrix M1, matrix M2)
{
	matrix *M;
	matr *tut;

	if((M1.n==M2.n)&&(M1.m==M2.m))
	{
		M = new matrix(&M1);
	}
	else 
	{
		cout << endl << "error: can't + matrix with different dimentions" << endl;
		exit(1);
	}

	tut = M2.M;
	while(tut!=NULL)
	{
		M->add(tut->i, tut->j, tut->value);
		tut = tut->next;
	}

	return *M;
}

matrix& operator-(matrix M1, matrix M2)
{
	matrix *M;
	matr *tut;

	if((M1.n==M2.n)&&(M1.m==M2.m))
	{
		M = new matrix(&M1);
	}
	else 
	{
		cout << endl << "error: can't - matrix with different dimentions" << endl;
		exit(1);
	}

	tut = M2.M;
	while(tut!=NULL)
	{
		M->ded(tut->i, tut->j, tut->value);
		tut = tut->next;
	}

	return *M;
}

matrix& matrix::operator+=(matrix &M1)
{
	matr *tut;

	if((this->n!=M1.n)&&(M1.m!=this->m))
	{
		cout << endl << "error: can't += matrix with different dimentions" << endl;
		exit(1);
	}

	tut = M1.M;
	while(tut!=NULL)
	{
		this->add(tut->i, tut->j, tut->value);
		tut = tut->next;
	}

	return *this;
}

matrix& matrix::operator-=(matrix &M1)
{
	matr *tut;

	if((n!=M1.n)&&(M1.m!=m))
	{
		cout << endl << "error: can't -= matrix with different dimentions" << endl;
		exit(1);
	}

	tut = M1.M;
	while(tut!=NULL)
	{
		this->ded(tut->i, tut->j, tut->value);
		tut = tut->next;
	}

	return *this;
}

matrix& operator*(matrix M1, double a)
{
	int i, j;
	matrix *M;

	M = new matrix(M1.n, M1.m);

	for(i=0; i<M1.n; i++)
	{
		for(j=0; j<M1.m; j++)
		{
			M->set(i, j, (M1.get(i, j)*a));
		}
	}

	return *M;
}

matrix& operator*(double a, matrix M1)
{
	int i, j;
	matrix *M;

	M = new matrix(M1.n, M1.m);

	for(i=0; i<M1.n; i++)
	{
		for(j=0; j<M1.m; j++)
		{
			M->set(i, j, (M1.get(i, j)*a));
		}
	}

	return *M;
}

matrix& operator*(matrix M1, matrix M2)
{
	int i, j;
	double val;
	matrix *M;

	if(M1.m==M2.n)
	{
		M = new matrix(M1.n, M2.m);
	}
	else 
	{
		cout << endl << "error: can't * matrix with different dimentions" << endl;
		exit(1);
	}

	for(i=0; i<M->n; i++)
	{
		for(j=0; j<M->m; j++)
		{
			val = Smul(&M1.row(i),&M2.column(j));
			M->set(i, j, val);
		}
	}

	return *M;
}

matrix& matrix::operator*=(matrix &M1)
{
	matrix M0(*this);
	*this = M0 *M1;
	return (*this);
}

matrix& operator/(matrix M1, double a)
{
	return(M1*(1.0/a));
}

vector& operator*(matrix M1, vector V2)
{
	int i, j;
	double val;
	vector *V;

	if(M1.m==V2.n)
	{
		V = new vector(M1.n);
	}
	else 
	{
		cout << endl << "error: can't * matrix and vector with different dimentions" << endl;
		exit(1);
	}

	for(i=0; i<M1.n; i++)
	{
		val = 0.0;
		for(j=0; j<M1.m; j++)
		{
				val += M1.get(i, j)*V2.get(j);
		}
		V->set(i, val);
	}

	return *V;
}

vector& operator*(vector V1, matrix M2)
{
	int i, j;
	double val;
	vector *V;

	if(V1.n==M2.n)
	{
		V = new vector(M2.m);
	}
	else 
	{
		cout << endl << "error: can't * matrix and vector with different dimentions" << endl;
		exit(1);
	}

	for(i=0; i<M2.m; i++)
	{
		val = 0.0;
		for(j=0; j<M2.n; j++)
		{
				val += V1.get(j)*M2.get(j, i);
		}
		V->set(i, val);
	}

	return *V;
}

matrix& matrix::operator=(matrix &source)
{
	int i, j, N, M;
	this->n = N = source.n;
	this->m = M = source.m;
	

	for(i=0; i<N; i++)
	{
		for(j=0; j<M; j++)
		{
			this->set(i, j, source.get(i, j));
		}
	}
	return *this;
}

int matrix::is_square()
{
	return(this->n==this->m);
}

int matrix::is_symm()
{
	int i, j, N;
	if(!is_square()) return 0;

	N = this->n;
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			if(this->get(i, j)!=this->get(j, i)) return 0;
		}
	}

	return 1;
}

void matrix::trans()
{
	if(!this->is_square())
	{
		cout << "Sorry, no transponiruem tol'ko kvadratnye matrcy" << endl;
		exit(1);
	}
	matrix M(this->n, this->n);
	for(int i=0; i<this->n; i++)
	{
		for(int j=0; j<this->n; j++)
		{
			M.set(i, j, this->get(j, i));
		}
	}
	*this = M;
}

/////////////////////////////////////////////////////////////////////////////

vector::vector(int I)
{
	this->V = new vect;
	this->V->i = 0;
	this->V->next = NULL;
	this->V->value = 0.0;
	this->n = I;
}

vector::vector()
{
	this->V = new vect;
	this->V->i = 0;
	this->V->next = NULL;
	this->V->value = 0.0;
	this->n = 0;
}

vector::vector(vector* V)
{
	vect *tut;
	this->V = new vect;
	this->V->i = 0;
	this->V->next = NULL;
	this->V->value = 0.0;
	this->n = V->n;
	
	tut = V->V;
	while(tut != NULL)
	{
		this->set(tut->i, tut->value);
		tut = tut->next;
	}
}

vector::vector(ifstream *file)
{
	this->V = new vect;
	this->V->i = 0;
	this->V->next = NULL;
	this->V->value = 0.0;

	int i, k;
	char *str, *strval, *sim;
	double val;

	str = new char[1000];
	strval = new char[30];
	sim = new char[1];

	i = this->n = 0;

	strcpy(sim, "");
	strcpy(strval, "");

	file->getline(str, 1000);

//	istrstream sstr(str, 100);
	for(k=0; k<strlen(str); k++)
	{
		sim[0] = str[k];

		if(strstr(sim, " ")!=NULL)
		{
			val = atof(strval);
			if(fabs(val)>e)this->set(i, val);
			strcpy(sim, "");
			strcpy(strval, "");
			i++;
//			cout << "val= " << val << endl;
			continue;
		}

		strcat(strval, &sim[0]);
	}
	val = atof(strval);
	if(fabs(val)>e)this->set(i, val);
	strcpy(sim, "");
	strcpy(strval, "");
	i++;

	this->n = i;

}

int vector::set(int i, double value)
{
	vect *etot, *sled;

	etot = sled = this->V;
	while(sled != NULL)
	{
		if(sled->i == i) 
		{
			if(fabs(value)>e)
			{
				sled->value = value;
				return 5;
			}
			else
			{
				if(this->V==sled)
				{
					this->V->value = 0.0;
					return 4;

				}
				else
				{
					etot->next = sled->next;
					return 3;
				}
			}
		}
		if((etot->i<i)&&(sled->i>i))
		{
			if(fabs(value)>e)
			{
				etot->next = new vect;
				etot->next->i = i;
				etot->next->value = value;
				etot->next->next = sled;
				return 1;
			}
			else
			{
//				etot->next = sled->next;
				return 3;
			}

		}
		etot = sled;
		sled = etot->next;
	}
	if(fabs(value)>e)
	{
		sled = new vect;
		sled->i = i;
		sled->value = value;
		sled->next = NULL;
		etot->next = sled;
		return 2;
	}
	return 3;
}

double vector::get(int i)
{
	vect *etot, *sled;

	if(this->n<=i)
	{
		cout << "\nerror: get position is out of range\n";
		exit(EXIT_FAILURE);
	}

	etot = sled = this->V;
	while(sled != NULL)
	{
		if(sled->i == i) 
		{
			return(sled->value);
		}
		if((etot->i<i)&&(sled->i>i))
		{
			return(0.0);
		}
		etot = sled;
		sled = etot->next;
	}


	return(0.0);
}

int vector::add(int i, double value)
{
	this->set(i, this->get(i)+value);
	return 0;
}

int vector::ded(int i, double value)
{
	this->set(i, this->get(i)-value);
	return 0;
}


char* vector::print()
{
	int i;
	vect *cur;
	char *str, *val;
	int size, l;
	l = 15;
	size = l*2*this->n + 1;
	str = new char[size];
	val = new char[l];
	cur = this->V;

	strcpy(str, "");
	for(i = 0; i<this->n; i++)
	{
		if(cur!=NULL)
		{
			if(i==cur->i)
			{
				if((fabs(cur->value)>99999.0)||(fabs(cur->value)<0.00001)) 					sprintf(val, "%7.2e", cur->value);
				else sprintf(val, "%7f", cur->value);
				strncat(str, val, l);
				cur = cur->next;
			}
			else
			{
				strcat(str, "0.0000000000000");
			}
		}
		else
		{
			strcat(str, "0.0000000000000");
		}
		strcat(str, " ");
	}
	strcat(str, "\n");

	return(str);
}

vector& operator+(vector V1, vector V2)
{
	vector *V;
	vect *tut;

	if(V1.n==V2.n)
	{
		V = new vector(&V1);
	}
	else 
	{
		cout << endl << "error: can't add vector with different dimentions" << endl;
		exit(1);
	}

	tut = V2.V;
	while(tut!=NULL)
	{
		V->add(tut->i, tut->value);
		tut = tut->next;
	}

	return *V;
}

int operator==(vector V1, vector V2)
{
	int i, N;

	if(V1.n!=V2.n)
	{
		cout << endl << "error: can't == vector with different dimentions" << endl;
		exit(1);
	}

	N = V1.n;

	for(i=0; i<N; i++)
	{
		if(V1.get(i)!=V2.get(i)) return 0 ;
	}

	return 1;
}


vector& operator-(vector V1, vector V2)
{
	vector *V;
	vect *tut;

	if(V1.n==V2.n)
	{
		V = new vector(&V1);
	}
	else 
	{
		cout << endl << "error: can't ded matrix with different dimentions" << endl;
		exit(1);
	}

	tut = V2.V;
	while(tut!=NULL)
	{
		V->ded(tut->i, tut->value);
		tut = tut->next;
	}

	return *V;
}

vector& vector::operator+=(vector &V1)
{
	int i, N;

	N = this->n = V1.n;
	for(i=0; i<N; i++)
	{
		this->set(i, this->get(i) + V1.get(i));
	}

	return *this;
}

vector& vector::operator-=(vector &V1)
{
	int i, N;

	N = this->n = V1.n;
	for(i=0; i<N; i++)
	{
		this->set(i, this->get(i) - V1.get(i));
	}

	return *this;
}


vector& vector::operator*=(double &a)
{
	int k;

	for(k=0; k<n; k++)
	{
		set(k, get(k)*a);
	}

	return *this;
}

vector& operator*(vector V1, double a)
{
	int i;
	vector *V;
	V = new vector(V1.n);

	for(i=0; i<V1.n; i++)
	{
			V->set(i, (V1.get(i)*a));

	}

	return *V;
}

vector& operator*(double a, vector V1)
{
	int i;
	vector *V;

	V = new vector(V1.n);

	for(i=0; i<V1.n; i++)
	{
		V->set(i, (V1.get(i)*a));
	}

	return *V;
}

vector& vector::operator=(vector &source)
{
	int i, N;
	this->n = N = source.n;

	for(i=0; i<N; i++)
	{
		set(i, source.get(i));
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////

double det(matrix M)
{
	if((M.n==1)&&(M.m==1)) return M.get(0, 0);

	int i, j;
	double val;

	j = 0;

	val = 0.0;
	for(i=0; i< M.n; i++)
	{
//		if(fabs(M.get(i, j))>e) val += M.get(i, j)*pow(-1.0, i+j+2)*det((M, i, j));
		val += M.get(i, j)*pow(-1.0, i+j+2)*det(Minor(&M, i, j));
	}

	return val;
}

matrix& Minor(matrix *M, int i, int j)
{
	if((M->n==1)&&(M->m==1))
	{
		return *M;
	}
	matrix *MN;
	MN = new matrix(M->n-1, M->m-1);
	int p, q, k, l;

	for(p=0, k=0; p<M->n; p++)
	{
		if(p==i) continue;
		for(q=0, l=0; q<M->m; q++)
		{
			if(q==j) continue;
			MN->set(k, l, M->get(p, q));
			l++;
		}
		k++;
	}

	return *MN;
}

double Smul(vector *V1, vector *V2)
{
	if(V2->n!=V1->n)
	{
		cout << endl << "error: can't ded matrix with different dimentions" << endl;
		exit(1);
	}

	int i;
	double val;

	val = 0.0;
	for(i=0; i<V1->n; i++)
	{
		val += V1->get(i)*V2->get(i);
	}
	return(val);
}

double kron(int i, int j)
{
	if(i==j) return(1.0);
	return(0.0);
}

/*
void matrix::inverse()
{
	if(!this->is_square())
	{
		cout << endl << "error: inverse is not permited" << endl;
		exit(1);
	}

	int i, j, k, N;
	N = this->n;
	matrix A(N, N), V(N, N), Ai(N, N);
	vector d(N), b(N), n(N), x(N);
	char *pm;

	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			A.set(i, j, this->get(i, j));
		}
	}

	for(k=0; k<N; k++)
	{
		for(i=0; i<N; i++)
		{
			b.set(i, kron(i, k));
		}

		x = Lsys(&A, &b);

		for(i=0; i<N; i++)
        {
        	Ai.set(k, i, x.get(i));
		}
	}

//	return Ai;

	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			this->set(i, j, Ai.get(i, j));
		}
	}

	pm = A.print();
	cout << "A:" << endl << pm << endl;
	pm = Ai.print();
	cout << "Ai:" << endl << pm << endl;
	getch();

	A *= Ai;

	pm = A.print();
	cout << "A:" << endl << pm << endl;
	pm = Ai.print();
	cout << "Al:" << endl << pm << endl;
	getch();
}
*/
void OrtR(matrix *M1, matrix *M2, vector *V1, vector *V2)
{

	int i, j, k, N;
	N = M2->n;
	double gij, sums;
	vector sum(N);

	for(i=0; i<N; i++)
	{
		for(k=0; k<N; k++)
		{
			sum.set(k, 0.0);
		}
		sums = 0.0;
		for(j=0;j<i; j++)
		{
			gij = Smul(&M2->row(i), &M1->row(j))/Smul(&M1->row(j), &M1->row(j));
			sum += gij*M1->row(j);
			sums += gij*V1->get(j);
		}
		for(k=0; k<N; k++)
		{
			M1->set(i, k, M2->get(i, k) - sum.get(k));
		}
		V1->set(i, V2->get(i) - sums);
	}
}

vector& LsysOrt(matrix *A, vector *b)
{
	int i, N;
	double res;
	N = b->n;
	vector d(N), *x, n(N), ax(N);
	matrix V(N, N);

	x = new vector(N);

	OrtR(&V, A, &d, b);

	for(i = 0; i<N; i++)
	{
		n.set(i, sqrt(Smul(&V.row(i), &V.row(i))));
	}

	for(i=0; i<N; i++)
    {
	   	res = 0.0;
/*
		for(j=0; j<N; j++)
        {
	    	res += (V.get(j, i)*d.get(j))/(n.get(j)*n.get(j));
		}
*/
		res = Smul(&V.column(i), &d)/Smul(&n, &n);
        x->set(i, res);
	}

	return *x;
}


void matrix::inverse()
{
	if(!this->is_square())
	{
		cout << endl << "error: metod Lsys1 ne goditsya, systema ne sovmestna" << endl;
		exit(1);
	}

	int i, j, k, N, M;
	double g, val;
	N = this->n;
	M = N*2;
	matrix V(N, M), E(N, N);

	for(i=0; i<N; i++)
	{
		for(j=0; j<M; j++)
		{
			if(j<N) V.set(i, j, this->get(i, j));
			else V.set(i, j, kron(i, j-N));
		}
	}

	for(k=0; k<N-1; k++)
	{
		g = 0.0;
		for(i=k+1; i<N; i++)
		{
			g = V.get(i, k)/V.get(k, k);
			val = 0.0;
			for(j=k; j<M; j++)
			{
				val = V.get(i, j) - V.get(k, j)*g;
				V.set(i, j, val);
			}
		}
	}

	for(k=N-1; k>0; k--)
	{
		g = 0.0;
		for(i=k-1; i>=0; i--)
		{
			g = V.get(i, k)/V.get(k, k);
			val = 0.0;
			for(j=k; j<M; j++)
			{
				val = V.get(i, j) - V.get(k, j)*g;
				V.set(i, j, val);
			}
		}
	}

	for(i=0; i<N; i++)
	{
		g = 1.0/V.get(i, i);
		val = 0.0;
		for(j=0; j<M; j++)
		{
			val = V.get(i, j)*g;
			V.set(i, j, val);
			if(j>=N) this->set(i, j-N, val);
		}
	}
}

vector& Lsys(matrix *A, vector *b)
{
	if(!A->is_square())
	{
		cout << endl << "error: metod Lsys ne goditsya, systema ne sovmestna" << endl;
		exit(1);
	}

	int N;
	matrix Al;//, At;
	vector *x;
//	char *pm;

	N = A->n;
	x = new vector(N);

	Al = *A;
	Al.inverse();

//	At = *A;

//	At*=Al;

//	pm = At.print();
//	cout << "E:" << endl << pm << endl;

	*x = Al*(*b);

	return *x;
}

double SNgrade(matrix M)
{
	if(!M.is_square())
	{
		cout << endl << "error: metod grade ne goditsya" << endl;
		exit(1);
	}
	int i, k, N;
	N = M.n;
	double lk, lk1, iks0;

	vector *l, x0(N), x1(N);
	matrix An;

	An = M;

	l = new vector(2);

	lk1 = 1.0;
	for(k=0; k<N; k++)
	{
		x0.set(k, 1.0);
	}
	i=0;
	do
	{
		lk = lk1;
		x1 = An*x0;
		for(k=0; k<N; k++)
		{
			iks0 = x1.get(0);
			x0.set(k, x1.get(k)/iks0);
	    }
		lk1 = x1.get(0);
		i++;
	}while(fabs(lk-lk1)>e);

	cout << " " << i << " iteracyi" << endl;
	if((!Pruve(M, x1.get(0)))&&(!Pruve(M, -x1.get(0))))
	{
		cout << endl << "error: SN rezul'tat ne dal" << endl;
	}
	
	return x1.get(0);
}

int Pruve(matrix M, double l)
{
	int i, j, N;
	N = M.n;
	matrix A(N, N), E(N, N);
//	char *pm;

	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			A.set(i, j, M.get(i, j) - kron(i, j)*l);
		}
	}
//	pm = A.print();
//	cout << pm << endl;
	return(fabs(det(A))<e);
}

void Vpq(matrix *V, matrix *M)
{
        int i, j, p, q;
        double C, S;

        MaxNDMod(*M, &p, &q);
	
        C = Mcos(*M, p, q);
        S = Msin(*M, p, q);

        for(i=0; i<M->n; i++)
        {
                for(j=0; j<M->m; j++)
                {
                        if((i==p)&&(j==p)) V->set(i, j, C);
                        else if((i==q)&&(j==p)) V->set(i, j, S);
                        else if((i==p)&&(j==q)) V->set(i, j, -S);
                        else if((i==q)&&(j==q)) V->set(i, j, C);
                        else V->set(i, j, kron(i, j)); 
                }
        }

/*
		char *pmatr;
		pmatr = V->print();
		cout << "Vpq" << endl << pmatr << endl;
		pmatr = M->print();
		cout << "M" << endl << pmatr << endl;
		cout << C*C + S+S << endl;
		getch();
*/
}

double Mcos(matrix M, int i, int j)
{
        double d, res;

        d = sqrt(pow((M.get(i, i) - M.get(j, j)), 2.0) + 4.0*M.get(i, j)*M.get(i, j));
        res = sqrt(0.5*(1.0 + fabs(M.get(i, i) - M.get(j, j))/d));
        return(res);
}

double Msin(matrix M, int i, int j)
{
        double d, res;

        d = sqrt(pow((M.get(i, i) - M.get(j, j)), 2.0) + 4.0*M.get(i, j)*M.get(i, j));
        res = Sign(M.get(i, j)*(M.get(i, i) - M.get(j, j)))*sqrt(0.5*(1.0 - fabs(M.get(i, i) - M.get(j, j))/d));
        return(res);
}

double Sign(double x)
{
        if(fabs(x)>e)return(x/fabs(x));
		return(0.0);
}

void MaxNDMod(matrix M, int* i, int* j)
{
	int it, jt;
	double max;
	*i = 0;
	*j = 1;
	max = fabs(M.get(0, 1));

	for(it=0; it<M.n; it++)
	{
		for(jt=0; jt<M.m; jt++)
		{
			if((it!=jt)&&(max<fabs(M.get(it, jt))))
			{
				*i = it;
				*j = jt;
				max = fabs(M.get(it, jt));
			}
		}
	}

}


vector& SNyakobi(matrix M)
{
//	clrscr();

	if(!M.is_symm())
	{
		cout << endl << "error: metod yakobi ne goditsya" << endl;
		exit(1);
	}
	int i, N, key, k;
	N = M.n;

	matrix C(N, N), CN(N, N), V(N, N), Vi(N, N);
	vector *l, l0, l1;
	l = new vector(N);

//	char *pmatr, *pvect;

	C = M;
	key = 1;
	l1 = C.diag();
/*
	pmatr = C.print();
	cout << "C000:" << endl << pmatr << endl;
*/
	k=0;
	do
	{
		l0 = l1;

		Vpq(&V, &C);
		Vi = V;
		Vi.trans();
//		C*=V;
		C = Vi*C*V;
//		C = Vi*C;
		l1 = C.diag();

//		C = Vi*V;
		for(i=0; i<N; i++)
		{
			if(fabs(l0.get(i)-l1.get(i))<e) key = 0;
			else 
			{
				key=1;
				break;
			}
		}

/*
		pmatr = C.print();
		cout << "C" << endl << pmatr << endl;
		pmatr = V.print();
		cout << "V" << endl << pmatr << endl << "key= " << key << endl;
//		pmatr = Vi.print();
//		cout << "Vi" << endl << pmatr << endl << "key= " << key << endl;
		getch();
*/
		
		k++;

	}while(key);

/*
	pmatr = CN.print();
	cout << "CN" << endl << pmatr << endl;
*/
	cout << " " << k << " iteracyi" << endl;

	for(i=0; i<N; i++)
	{
		l->set(i, l1.get(i));
		if(!Pruve(M, l->get(i)))
		{
			cout << endl << "error: SN rezul'tat ne dal" << endl;
		}
	}

	return *l;
}



vector& Vmul(vector *V1, vector *V2)
{
	if((V1->n!=3)||(V2->n!=3))
	{
		cout << endl << "error: metod yakobi ne goditsya" << endl;
		exit(1);
	}
	int i, j, N;
	N = V1->n;
	vector *V;
	matrix M(3, N);
	double val;
	V = new vector(N);
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			if(i==0) val = 1.0;
			if(i==1) val = V1->get(j);
			if(i==2) val = V2->get(j);
			M.set(i, j, val);
		}
	}
	for(i=0; i<N; i++)
	{
		V->set(i, det(Minor(&M, 0, i)));
	}

	return *V;

}

double VectCos(vector *V1, vector *V2)
{
	return(Smul(V1, V2)/(sqrt(Smul(V1, V1))*sqrt(Smul(V2, V2))));
}
