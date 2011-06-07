/* Siconos-sample version 3.1.0, Copyright INRIA 2005-2009.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 b* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 *
 */

#include "BodiesViewer.hpp"

#include "GLDebugDrawer.h"
#include "GL_ShapeDrawer.h"



class BulletViewer : public BodiesViewer
{
  GLDebugDrawer _debugDrawer;
  GL_ShapeDrawer _shapeDrawer;

  std::vector<btVector3> _colors;

  void init();
  void animate();
  void draw();

  void addColor(const btVector3& c)
  {
    _colors.push_back(c);
  };
  void drawQGLShape(const QGLShape&);
  QString helpString() const;
  virtual void mousePressEvent(QMouseEvent *);
  virtual void mouseMoveEvent(QMouseEvent *);
  virtual void mouseReleaseEvent(QMouseEvent *);
  virtual void keyPressEvent(QKeyEvent *);
};
