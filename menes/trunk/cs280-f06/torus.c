#include <GL/glut.h>
#define _USE_MATH_DEFINES 1
#include <math.h>


void glutTexturedTorus ( GLfloat r, GLfloat R, GLint nsides, GLint rings )
{
  int i, j;
  GLfloat theta, phi, theta1;
  GLfloat cosTheta, sinTheta;
  GLfloat cosTheta1, sinTheta1;
  GLfloat ringDelta, sideDelta;
  GLfloat tx, tx1, ty;
  GLfloat txDelta, tyDelta;

  txDelta = 1.0 / rings;
  tyDelta = 1.0 / nsides;
  ringDelta = 2.0 * M_PI / rings;
  sideDelta = 2.0 * M_PI / nsides;

  tx = 0.0;
  theta = 0.0;
  cosTheta = 1.0;
  sinTheta = 0.0;
  for (i = rings - 1; i >= 0; i--) {
    tx1 = tx + txDelta;
    theta1 = theta + ringDelta;
    cosTheta1 = cos(theta1);
    sinTheta1 = sin(theta1);
    glBegin(GL_QUAD_STRIP);
    phi = 0.0;
    ty = 0.0;
    for (j = nsides; j >= 0; j--) {
      GLfloat cosPhi, sinPhi, dist;

      phi += sideDelta;
      cosPhi = cos(phi);
      sinPhi = sin(phi);
      dist = R + r * cosPhi;

      glTexCoord2f( tx1, ty );
      glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
      glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);

      glTexCoord2f( tx, ty );
      glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
      glVertex3f(cosTheta * dist, -sinTheta * dist,  r * sinPhi);

      ty += tyDelta;
    }
    glEnd();
    tx = tx1;
    theta = theta1;
    cosTheta = cosTheta1;
    sinTheta = sinTheta1;
  }
}

