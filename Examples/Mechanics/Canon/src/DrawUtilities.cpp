/* Siconos-Example version 3.0.0, Copyright INRIA 2005-2008.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth FLOOR, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 *
 */

#ifdef WithQGLViewer
#include "DrawUtilities.h"
#include <QGLViewer/qglviewer.h>

using namespace std;

void DrawUtilities::drawSphere(double radius, double x, double y, double z, double c)
{
  glPushMatrix();
  glTranslatef(x, y, z);
  glBegin(GL_QUADS);
  GLUquadricObj *Sphere = gluNewQuadric();
  //  gluQuadricCallback(Sphere, which, errorFunc);

  GLint slices = 100; // number of slices around z-axis
  GLint stacks = 100; // number of stacks along z-axis

  glColor3f(c, 2 * c, 0.5 * c);
  gluSphere(Sphere, radius, slices, stacks);
  glEnd();

  glPopMatrix();
}

void DrawUtilities::drawHorizontalPlane(double ground)
{
  glEnable(GL_BLEND);
  double alpha = 0.1;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.5, 0.7, 0.9, alpha);

  glBegin(GL_QUADS);
  // Ground

  glNormal3f(0.0f, -1.0f, ground);// Normal Pointing Down
  glVertex3f(-1.0, -1.0, ground);
  glVertex3f(-1.0, 1.0, ground);
  glVertex3f(1.0, 1.0, ground);
  glVertex3f(1.0, -1.0, ground);
  glEnd();
  glDisable(GL_BLEND);
}

void DrawUtilities::drawXVerticalPlane(double val)
{
  glEnable(GL_BLEND);
  double alpha = 0.1;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.5, 0.7, 0.9, alpha);

  glBegin(GL_QUADS);
  // Ground

  glNormal3f(1.0f, 0.0f, 0);
  glVertex3f(val, -1,  -1);
  glVertex3f(val, -1, 1);
  glVertex3f(val, 1, 1);
  glVertex3f(val, 1, -1);
  glEnd();
  glDisable(GL_BLEND);
}

void DrawUtilities::drawYVerticalPlane(double val)
{
  glEnable(GL_BLEND);
  double alpha = 0.1;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.5, 0.7, 0.9, alpha);

  glBegin(GL_QUADS);

  glNormal3f(0.0f, 1.0f, 0);
  glVertex3f(-1, val,  -1);
  glVertex3f(-1, val,  1);
  glVertex3f(1, val, 1);
  glVertex3f(1, val, -1);

  glEnd();
  glDisable(GL_BLEND);
}

#endif

