/** \class SiconosMatrix
*   \brief This class is an encapsulation of the Lapack++ class managing vmatrices of double.
*  \author SICONOS Development Team - copyright INRIA
*   \version 1.0
*   \date (Creation) 05/19/2004
*
 *
*
*
* SiconosMatrix is used in the platform to store matrices (mathematical object).
*
*/

#ifndef __SiconosMatrix__
#define __SiconosMatrix__


#include <iostream>
#include <lapack++.h>
#include <string>
#include <fstream>
#include <stdio.h>
//#include "SiconosVector.h"
#include "NewSiconosVector.h"
#include "SimpleVector.h"
#include "SiconosMatrixException.h"
#include "check.h"

//using namespace std;

const bool printVerbose = true;

// This constant set the maximum allowed size for displaying a Matrix on standard output (see function display())
const int MAXSIZEFORDISPLAY = 10;

class siconosVector;
class SimpleVector;

class SiconosMatrix
{
private:

  /** \var LaGenMatDouble mat;
   * \brief The Matrix Lapack ++, LaGenMatDouble, encapsulated.
   */
  LaGenMatDouble mat;

  /** \var LaVectorLongInt* ipiv;
   * \brief Pointer to a  LaVectorLongInt for the partial pivoting in PLUFactorization.
   */
  LaVectorLongInt* ipiv;

  /** \var  bool isPLUFactorized;
     * \brief  Boolean = true if the Matrix is PLU Factorized
     */
  bool isPLUFactorized;

  /** \var  bool isPLUInversed;
     * \brief  Boolean = true if the Matrix is Inversed in Place
     */
  bool isPLUInversed;

  /** \fn LaGenMatDouble getMatrix() const;
   *  \brief get the private member mat
   *  \return LaGenMatDouble
   */
  LaGenMatDouble getMatrix() const;


  /** \fn void setMatrix(LaGenMatDouble m)
   *  \brief set the private member mat with m
   *  \param the LaGenMatDouble to put in the matrix
   */
  void setMatrix(LaGenMatDouble m)
  {
    this->mat = m;
  };

  /** \fn static void verbose(string msg)
   *  \brief print on the screen a message "printVerbose" static variable is true
   *  \param string : the message to print
   */
  static void verbose(string msg);



public:


  /** \fn SiconosMatrix ()
   *  \brief contructor
   *  \return SiconosMatrix
   */
  SiconosMatrix();

  /** \fn SiconosMatrix (int row, int col)
   *  \brief contructor with the dimension
   *  \param an integer to set the number of row
   *  \param an integer to set the number of column
   *  \return SiconosMatrix
   *  \exception SiconosMatrixException
   */
  SiconosMatrix(int row, int col);

  /** \fn SiconosMatrix (const LaGenMatDouble m)
   *  \brief contructor with a LaGenMatDouble
   *  \param a LaGenMatDouble
   *  \return SiconosMatrix
   */
  SiconosMatrix(const LaGenMatDouble m);

  /** \fn SiconosMatrix (const LaVectorDouble v, int row, int col);
   *  \brief contructor with a LaVectorDouble
   *  \param a LaVectorDouble
   *  \param an integer to set the number of row
   *  \param an integer to set the number of column
   *  \return SiconosMatrix
   */
  SiconosMatrix(const LaVectorDouble v, int row, int col);

  /** \fn SiconosMatrix (string file, bool ascii)
   *  \brief contructor with an input file
   *  \param a string which contain the file path
   *  \param a boolean to indicate if the file is in ascii
   *  \return SiconosMatrix
   */
  SiconosMatrix(string file, bool ascii);

  /** \fn ~SiconosMatrix ()
   *  \brief destructor
   */
  ~SiconosMatrix();

  /** \fn int size (int)
   *  \brief get the dimension of the matrix
   *  \param the dimension to get (0 to get the number of rows or 1 to get the number of columns)
   *  \exception SiconosMatrixException
   *  \return the a dimension of the matrix
   */
  int size(int d) const;

  /** \fn operator (int row, int col)
   *  \brief get or set the element matrix[i,j]
   *  \param an integer i
   *  \param an integer j
   *  \exception SiconosMatrixException
   *  \return the element matrix[i,j]
   */
  double& operator()(int row, int col);

  /** \fn setValue(const int row, const int col, const double d)
   * \brief set the element matrix[row, col]
   * \param an integer col
   * \param an integer row
   * \param a double d : the value.
   */
  inline void setValue(const int row, const int col, const double d)
  {
    (*this)(row, col) = d;
  }

  /** \fn getValue(const int row, const int col)
   * \brief get the element matrix[row, col]
   * \param an integer col
   * \param an integer row
   * \return a double d : the value.
   */
  inline double getValue(const int row, const int col)
  {
    return (*this)(row, col);
  }

  /** \fn LaGenMatDouble getLaGenMatDouble()
   *  \brief get LaGenMatDouble matrix
   *  \return a LaGenMatDouble
   */
  LaGenMatDouble getLaGenMatDouble() const;

  /** \fn LaGenMatDouble getLaGenMatDouble()
   *  \brief get LaGenMatDouble matrix
   *  \return a LaGenMatDouble
   */
  LaGenMatDouble& getLaGenMatDouble();


  /** \fn SiconosVector& getRow(int index)
   *  \brief get a row of the matrix
   *  \return a SiconosVector which represent the row
   *  \WARNING 11 Feb 2005 : possible memory problem ? (dynamical allocation without delete...)
   */
  SimpleVector getRow(const int index) const;

  /** \fn bool isSquare()
   *  \brief determines if the matrix is square
   *  \return true if the matrix is square
   */
  bool isSquare();

  /** \fn bool isInversed()
   *  \brief determines if the matrix has been inversed
   *  \return true if the matrix is inversed
   */
  inline bool isInversed() const
  {
    return this->isPLUInversed;
  }

  /** \fn bool isfactorized()
   *  \brief determines if the matrix has been factorized
   *  \return true if the matrix is factorized
   */
  inline bool isFactorised() const
  {
    return this->isPLUFactorized;
  }

  /** \fn bool addRow(int row, SiconosVector v)
   *  \brief fill values of a row in the matrix
   *  \param int row : the row which is filled
   *  \param SiconosVector v : the values which have to be copied in the row
   *  \return true if no error
   */
  bool addRow(int row, SiconosVector &v);

  /** \fn bool read(string fileName, string mode = BINARY)
   *  \brief read the matrix in a file
   *  \param string fileName : the file to read
   *  \param string mode : ASCII or BINARY (binary is the default mode)
   *  \exception SiconosMatrixException
   *  \return true if no error
   */
  bool read(string fileName, string mode = "binary");

  /** \fn bool write(string fileName, string mode = BINARY)
   *  \brief write the matrix in a file
   *  \param string fileName : the file to read
   *  \param string mode : ASCII or BINARY (binary is the default mode
   *  \exception SiconosMatrixException
   *  \return true if no error
   */
  bool write(string fileName, string mode = "binary");

  /** \fn double* getArray()
   *  \brief return the adress of the array of double values of the matrix
   *  \return double* : the pointer on the double array
   */
  inline double* getArray()
  {
    return this->mat.addr();
  }


  /** \fn SiconosMatrix linearSolve(SiconosMatrix & B)
   *  \brief compute the vector x in  Ax=B
   *  \param SiconosMatrix & B
   *  \return SiconosMatrix X
   */
  SiconosMatrix linearSolve(SiconosMatrix & B);


  /** \fn  PLUFactorizationInPlace(void);
   *  \brief Compute the LU factorization with Partial pivoting.  The result is returned in this (InPlace)
   */
  void PLUFactorizationInPlace(void);

  /** \fn SiconosMatrix PLUFactorization(void);
   *  \brief Compute the LU factorization with Partial pivoting.
   *  \return The factorized Matrix PLU
   */
  SiconosMatrix PLUFactorization(void);


  /** \fn  SiconosMatrix  PLUInverse(void);
   *  \brief  use the LU factorization with Partial pivoting of the matrix this (or the matrix this itself if it is triangular)
   *       to compute the inverse matrix.
   * \return The Inverse Matrix
   */
  SiconosMatrix  PLUInverse(void);

  /** \fn  SiconosMatrix  PLUInverseInPlace(void);
  *  \brief  use the LU factorization with Partial pivoting of the matrix this (or the matrix this itself if it is triangular)
  *       to compute the inverse matrix.  The result is returned in this (InPlace)
  */
  void  PLUInverseInPlace(void);

  /** \fn SiconosMatrix  PLUForwardBackward(SiconosMatrix &B);
   *  \brief use the LU factorization with Partial pivoting of the matrix this (or the matrix this itself if it is triangular)
   *  to solve A x = B by forward or backward substitution;
   *  \param the RHS matrix b
   *   \return The result matrix x
   */
  SiconosMatrix  PLUForwardBackward(SiconosMatrix &B);


  /** \fn SiconosMatrix  PLUForwardBackward(SiconosMatrix &B);
   *  \brief use the LU factorization with Partial pivoting of the matrix this (or the matrix this itself if it is triangular)
   *  to solve A x = B by forward or backward substitution;
   *  \param the RHS matrix b  which contains the result x
   */
  void  SiconosMatrix::PLUForwardBackwardInPlace(SiconosMatrix &B) ;

  /** \fn SiconosVector  PLUForwardBackward(SiconosVector &B);
   *  \brief use the LU factorization with Partial pivoting of the matrix this (or the matrix this itself if it is triangular)
   *  to solve A x = B by forward or backward substitution;
   *  \param the RHS vector b
   *   \return The result vector x
   */
  SimpleVector  PLUForwardBackward(SiconosVector &B);

  /** \fn SiconosVector  PLUForwardBackward(SiconosVector &B);
   *  \brief use the LU factorization with Partial pivoting of the matrix this (or the matrix this itself if it is triangular)
   *  to solve A x = B by forward or backward substitution;
   *  \param the RHS vector b which contains the result x
   */
  void   PLUForwardBackwardInPlace(SiconosVector &B);

  /** \fn SiconosMatrix multTranspose(SiconosMatrix B)
   *  \brief compute A*Bt
   *  \param SiconosMatrix B
   *  \return SiconosMatrix : the result of the multiplication
   */
  SiconosMatrix multTranspose(SiconosMatrix &B);

  /** \fn void blockMatrixCopy( SiconosMatrix &blockMat, int xPos, int yPos)
   *  \brief copy the blockmatrix "blockMat" in the matrix "mat" at the position (xPos, yPos)
   *      blockMatrixCopy([1], [0 0 0 0], 0, 2) => mat = [0 0 1 0]
   *  \param SiconosMatrix& : the block matrix to copy in the current matrix
   *  \param int : the line position to start the copy of the blockmatrix
   *  \param int : the column position to start the copy of the blockmatrix
   */
  void blockMatrixCopy(SiconosMatrix &blockMat, int xPos, int yPos);

  /** \fn affectation operator
   *  \param SiconosMatrix : the matrix to be copied
   */
  SiconosMatrix& operator = (const SiconosMatrix& m);

  /** \fn comparison operator ==
   *  \brief compare the value of each element
   *  \return true if all the element are equal
   */
  friend bool operator == (const SiconosMatrix& m1, const SiconosMatrix& m2);

  /** \fn comparison operator !=
   *  \brief compare the value of each element
   *  \return true if all the elements are not equals one to one, or if the sizes of the matrices are different
   */
  friend bool operator != (const SiconosMatrix& m1, const SiconosMatrix& m2);

  // Calculation operators
  /** \fn operator * (const SiconosMatrix& m1, const SiconosMatrix& m2);
   *  \brief multiplication of two matrices
   *  \return a SiconosMatrix
   *  \exception SiconosMatrixException, if the sizes are incompatible
   */
  friend SiconosMatrix operator * (const SiconosMatrix& m1, const SiconosMatrix& m2);

  /** \fn operator * (const SiconosMatrix& m1, const double& d);
   *  \brief multiplication of a matrix by a double
   *  \return a SiconosMatrix
   */
  friend SiconosMatrix operator * (const SiconosMatrix& m, const double& d);

  /** \fn operator * (const SiconosMatrix& m1, const double& d);
   *  \brief multiplication of a matrix by a double
   *  \return a SiconosMatrix
   */
  friend SiconosMatrix operator * (const double d, const SiconosMatrix& m)
  {
    return (m * d);
  }

  /** \fn operator + (const SiconosMatrix& m1, const SiconosMatrix& m2);
   *  \brief Addition of two matrices
   *  \return a SiconosMatrix
   *  \exception SiconosMatrixException, if the sizes are incompatible
   */
  friend SiconosMatrix operator + (const SiconosMatrix& m1, const SiconosMatrix& m2);

  /** \fn operator - (const SiconosMatrix& m1, const SiconosMatrix& m2);
   *  \brief subtraction of two matrices
   *  \return a SiconosMatrix
   *  \exception SiconosMatrixException, if the sizes are incompatible
   */
  friend SiconosMatrix operator - (const SiconosMatrix& m1, const SiconosMatrix& m2);

  /** \fn operator / (const SiconosMatrix& m1, const double d);
  *  \brief division of the matrix by a double
  *  \return a SiconosMatrix
  *  \exception SiconosMatrixException, if the double d = 0
  */
  friend SiconosMatrix operator / (const SiconosMatrix& m, const double d);

  /** \fn operator ^ (const SiconosMatrix& m1, const int pow);
   *  \brief compute the power of the matrix (!)
   *  \return a SiconosMatrix
   *  \exception SiconosMatrixException, if the power < 0
   */
  friend SiconosMatrix operator ^ (const SiconosMatrix& m, const int pow);

  // Io-stream operators
  friend istream& operator >> (istream& i, SiconosMatrix& m);
  friend ostream& operator << (ostream& o, SiconosMatrix& m);

  /** \fn void zero();
   *  \brief sets all the values of the matrix to 0.0
   */
  void zero();

  /** \fn void display();
   *  \brief display data on standard output
   */
  void display() const;

  /** \fn SiconosMatrix BlockMatrixAssemble(vector<SiconosMatrix*>);
   *  \brief build a matrix from n matrices
   *  \return a SiconosMatrix
   */
  friend SiconosMatrix BlockMatrixAssemble(vector<SiconosMatrix*>);

};

#endif // __SiconosMatrix__
