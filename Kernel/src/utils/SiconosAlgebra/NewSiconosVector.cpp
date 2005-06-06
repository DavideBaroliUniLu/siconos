#include "NewSiconosVector.h"
using namespace std;

SiconosVector::SiconosVector()
{}

SiconosVector::~SiconosVector()
{}

SiconosVector& SiconosVector::operator = (const SiconosVector& v)
{
  //IN(" SiconosVector::operator =\n");
  cout << " SiconosVector::operator =\n";

  if (this == &v)
  {
    //OUT(" SiconosVector::operator =\n");
    return *this;
  }
  else if (this->size() == v.size())
  {
    cout << "WARNING : operator = used with different types of vectors." << endl;
    for (int i = 0; i < this->size() ; i++)
      (*this)(i) = v(i);
    return *this;
  }
  else
    SiconosVectorException::selfThrow(" SiconosVector::operator = --- case not unforseen");
  //OUT(" SiconosVector::operator =\n");
}




void SiconosVector::zero()
{
  const int size = this->size();
  for (int i = 0; i < size; i++)
    (*this)(i) = 0.0;
}


string SiconosVector::toString()
{
  char element[100];
  int i = 0, end = 0;
  string vectorContent = "";
  while (i < this->size())
  {
    strcpy(element, "");
    sprintf(element, N_DOUBLE_PRECISION, (*this)(i));
    if (i > 0)
    {
      vectorContent += " ";
      vectorContent += element;
    }
    else vectorContent = element;
    i++;
  }
  return vectorContent;
}




