/* Include Directives {{{ */
#include "cxx/platform.hpp"
#include "ext/foreach.hpp"

#include <stdbool.h>
#include <stdint.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <typeinfo>

#include <sys/time.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <Cg/cgGL.h>
#include <IL/ilut.h>
#include <jpeglib.h>

#include "mt19937ar.c"

#include "menes.hpp"
/* }}} */
/* Error Handling {{{ */
#define _trace() \
    printf("_trace(): %s(%u)\n", __FILE__, __LINE__)

void error_(const char *file, unsigned line) {
    bool errored(false);

    while (ILenum error = ilGetError()) {
        errored = true;
        fprintf(stderr, "ILerror #%u: %s\n", error, iluErrorString(error));
    }

    while (GLenum error = glGetError()) {
        errored = true;
        fprintf(stderr, "GLerror #%u: %s\n", error, gluErrorString(error));
    }

    if (errored)
        printf("  at %s(%u)\n", file, line);
}

#define error_() \
    error_(__FILE__, __LINE__)
/* }}} */
/* More... #includes? {{{ */
#include "cxx/assert.hpp"

#include "alg/vector.hpp"
//#include "alg/transform.hpp"

#include "g3d/buffer.hpp"
#include "g3d/camera.hpp"
#include "g3d/engine.hpp"
#include "g3d/grid.hpp"
#include "g3d/material.hpp"
#include "g3d/object.hpp"
#include "g3d/texture.hpp"
#include "g3d/time.hpp"
#include "g3d/window.hpp"
/* }}} */
/* Source #includes {{{ */
#include "cxx/assert.cpp"
/* }}} */

_R<g3d::Texture2d> bottom_;
_R<g3d::Texture2d> depth_[8];
_R<g3d::Texture2d> environment_;
_R<g3d::Texture2d> described_;

_R<g3d::Camera> lights_[64];

bool paused_ = false;

static const ::CGprofile VertexProfile_ = CG_PROFILE_VP30;
static const ::CGprofile FragmentProfile_ = CG_PROFILE_FP30;

static const unsigned Resolution_ = 256;
//static float Waves_[Resolution_][Resolution_];

extern "C" void glutSolidTeapot2();
extern "C" void glutTexturedTorus(GLfloat r, GLfloat R, GLint nsides, GLint rings);

static _R<g3d::Program> shadowed_fcg_;

static _R<g3d::Material> shader_;
static _R<g3d::Material> position_;
static _R<g3d::Material> flat_;
static _R<g3d::Material> diffuse_;
static _R<g3d::Material> projected_;
static _R<g3d::Material> shadowed_;

static _R<g3d::Camera> camera_;
static _R<g3d::Camera> eye_;

static _R<g3d::World> world_;
static _R<g3d::World> sky_;
static _R<g3d::World> markers_;

bool mark_;

/* Matrix Handlers {{{ */
void print4x4_(GLenum name, const char *tag) {
    float value[4][4];
    glGetFloatv(name, reinterpret_cast<float *>(value));

    printf("%s:\n", tag);
    printf("        X      Y      Z      T\n");
    for (size_t i(0); i != 4; ++i) {
        printf("%c:", "XYZW"[i]);
        for (size_t j(0); j != 4; ++j)
            printf(" %6.2g", value[j][i]);
        printf("\n");
    }
}

#define print4x4_(name) \
    print4x4_(name, #name)
/* }}} */

unsigned pass_;

void BindShader(const g3d::Node *_this) {
    shader_->Bind();

    ::glPushMatrix();

    ::glLoadIdentity();
    _this->FullTransform();
    shader_->GetVertexProgram()->MaybeSetStateMatrixParameter("m", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

    _forall (unsigned, i, 0, 4) {
        const _R<g3d::Camera> &light(lights_[4 * pass_ + i]);

        char name[4];
        name[3] = '\0';

        name[2] = '0' + i;

        name[0] = 'l';
        name[1] = 'v';

        ::glLoadIdentity();
        camera_->FullInvert();
        light->FullTransform();
        shader_->GetVertexProgram()->MaybeSetStateMatrixParameter(name + 0, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

        name[1] = 't';

        ::glLoadIdentity();
        ::glTranslatef(0.5f, 0.5f, 0.5f);
        ::glScalef(0.5f, 0.5f, 0.5f);
        light->Project();
        light->FullInvert();
        _this->FullTransform();
        shader_->GetVertexProgram()->MaybeSetStateMatrixParameter(name + 1, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    }

    ::glLoadIdentity();
    camera_->FullInvert();
    shader_->GetVertexProgram()->MaybeSetStateMatrixParameter("v", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

    ::glPopMatrix();
}

class Teapot :
    public g3d::Node
{
  private:
    virtual void Display_() const {
        glColor3f(1, 0, 0);
        ::BindShader(this);
        //glutSolidTeapot2();
        glutTexturedTorus(0.7, 1.8, 16, 32);
        shader_->Unbind();
    }
};

class Torus :
    public g3d::Node
{
  private:
    virtual void Display_() const {
        glColor3f(0, 1, 0);
        ::BindShader(this);
        glutTexturedTorus(0.7, 1.8, 16, 32);
        shader_->Unbind();
    }
};

static _R<g3d::Engine> engine_;

class StupidGrid :
    public g3d::Grid
{
  protected:
    virtual void Display_() const {
        g3d::Face face(engine_->GetFace());
        engine_->SetFace(g3d::Both);
        glColor3f(0, 0, 1);
        ::BindShader(this);
        g3d::Grid::Display_();
        shader_->Unbind();
        engine_->SetFace(face);
    }

  public:
    StupidGrid(float radius, unsigned resolution) :
        g3d::Grid(radius, resolution)
    {
    }

    virtual ~StupidGrid() {
    }
};

/* Hasty Sphere {{{ */
class Sphere :
    public g3d::Node
{
  private:
    float radius_;
    unsigned resolution_;

    _R<g3d::Buffer> vertices_;
    _R<g3d::Buffer> indices_;
    _R<g3d::Buffer> coords_;

  protected:
    virtual void Display_() const {
        ::glColor3f(1, 0, 1);
        ::BindShader(this);

        ::glEnableClientState(GL_VERTEX_ARRAY);
        vertices_->Bind();
        ::glVertexPointer(3, GL_FLOAT, 0, NULL);

        ::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        coords_->Bind();
        ::glTexCoordPointer(2, GL_FLOAT, 0, NULL);

        indices_->Bind();
        ::glDrawElements(GL_TRIANGLES, alg::VERTICES_PER_TRIANGLE * alg::TRIANGLES_PER_SQUARE * (GetResolution() - 1) * (GetResolution() * 2 - 1), GL_UNSIGNED_SHORT, NULL);

        shader_->Unbind();
    }

  public:
    Sphere(float radius, unsigned resolution) :
        radius_(radius),
        resolution_(resolution)
    {
        indices_ = new g3d::Buffer(GL_ELEMENT_ARRAY_BUFFER);
        indices_->Copy(sizeof(uint16_t) * alg::VERTICES_PER_TRIANGLE * alg::TRIANGLES_PER_SQUARE * (GetResolution() - 1) * (GetResolution() * 2 - 1), NULL, GL_STATIC_DRAW);

        _map (indices_, uint16_t, GL_WRITE_ONLY) {
            size_t offset(0);

            for (unsigned y(0); y != GetResolution() - 1; ++y)
            for (unsigned x(0); x != GetResolution() * 2 - 1; ++x) {
                _value(offset++) = (y + 0) * (GetResolution() * 2) + (x + 0);
                _value(offset++) = (y + 1) * (GetResolution() * 2) + (x + 0);
                _value(offset++) = (y + 1) * (GetResolution() * 2) + (x + 1);
                _value(offset++) = (y + 1) * (GetResolution() * 2) + (x + 1);
                _value(offset++) = (y + 0) * (GetResolution() * 2) + (x + 1);
                _value(offset++) = (y + 0) * (GetResolution() * 2) + (x + 0);
            }
        }

        vertices_ = new g3d::Buffer(GL_ARRAY_BUFFER);
        vertices_->Copy(sizeof(alg::Vector3f) * GetResolution() * GetResolution() * 2, NULL, GL_STATIC_DRAW);

        _map (vertices_, alg::Point3f, GL_WRITE_ONLY)
            for (unsigned y(0); y != GetResolution(); ++y)
            for (unsigned x(0); x != GetResolution() * 2; ++x) {
                float u(x / static_cast<float>(GetResolution() * 2 - 1));
                float v(y / static_cast<float>(GetResolution() - 1));

                _value(y * (GetResolution() * 2) + x).Set(
                    GetRadius() * sin(u * M_PI * 2) * sin(v * M_PI),
                    GetRadius() * cos(v * M_PI),
                    GetRadius() * cos(u * M_PI * 2) * sin(v * M_PI)
                );
            }

        coords_ = new g3d::Buffer(GL_ARRAY_BUFFER);
        coords_->Copy(sizeof(alg::Vector2f) * GetResolution() * GetResolution() * 2, NULL, GL_STATIC_DRAW);

        _map (coords_, alg::Point2f, GL_WRITE_ONLY)
            for (unsigned y(0); y != GetResolution(); ++y)
            for (unsigned x(0); x != GetResolution() * 2; ++x)
                _value(y * (GetResolution() * 2) + x).Set(
                    x / static_cast<float>(GetResolution() * 2 - 1),
                    y / static_cast<float>(GetResolution() - 1)
                );
    }

    _finline float GetDelta() const {
        return 2 * radius_ / resolution_;
    }

    _property(float, Radius, {
        return radius_;
    }, {
        _assert(false);
        radius_ = _value;
    })

    _property(unsigned, Resolution, {
        return resolution_;
    }, {
        _assert(false);
        resolution_ = _value;
    })
};
/* }}} */

static _R<g3d::Window> window_;
static _R<g3d::Context> context_;

static _R<g3d::Node> control_;
static _R<g3d::Grid> ground_;
static _R<Teapot> teapot_;
static _R<Torus> torus_;

static _R<Sphere> sphere_;

unsigned mode_ = 1;

/* Display Function {{{ */
void OnDisplay_() {
    static double average_(0.0);
    static unsigned frames_(0);

    unsigned width(window_->GetWidth());
    unsigned height(window_->GetHeight());

    double begin(api::GetWallTimerSeconds());

    ::glEnable(GL_POLYGON_OFFSET_FILL);
    ::glPolygonOffset(1.1, 4.0);
    engine_->SetFace(g3d::Back);

    engine_->SetWorld(world_);

    ::glDepthFunc(GL_LESS);

    _forall (unsigned, i, 0, 8) {
        pass_ = i;

        ::glClear(GL_DEPTH_BUFFER_BIT);

        shader_ = position_;

        static const unsigned power(2);
        _forall (unsigned, x, 0, power)
        _forall (unsigned, y, 0, power) {
            ::glViewport(x * width / power, y * height / power, width / power, height / power);
            lights_[i * power * power + x * power + y]->Bind();
            engine_->Display();
        }

        depth_[i]->Bind();

        ::glReadBuffer(GL_BACK);
        ::glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
    }

    engine_->SetFace(g3d::Front);
    ::glDisable(GL_POLYGON_OFFSET_FILL);

    ::glViewport(0, 0, width, height);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ::glDepthFunc(GL_LEQUAL);

    engine_->SetWorld(world_);

    switch (mode_) {
        case 1:
            camera_->Bind();
            shader_ = diffuse_;
            engine_->Display();
        break;

        case 2:
            camera_->Bind();
            shader_ = projected_;
            engine_->Display();
        break;

        case 3:
            camera_->Bind();
            shader_ = position_;
            engine_->Display();

            ::glEnable(GL_BLEND);
                ::glBlendFunc(GL_ONE, GL_ONE);

                shader_ = shadowed_;
                camera_->Bind();

                _forall (unsigned, i, 0, 8) {
                    pass_ = i;
                    shadowed_fcg_->SetParameter("depth", depth_[i]);
                    engine_->Display();
                }

            ::glDisable(GL_BLEND);
        break;

        case 4:
            lights_[0]->Bind();
            shader_ = diffuse_;
            engine_->Display();
        break;
    }

    if (mark_) {
        engine_->SetWorld(markers_);
        camera_->Bind();
        shader_ = flat_;
        engine_->Display();
    }

    engine_->SetWorld(sky_);

    engine_->SetFace(g3d::Back);
        camera_->Bind();
        shader_ = projected_;
        engine_->Display();
    engine_->SetFace(g3d::Front);

    ::glutSwapBuffers();
    error_();

    double time(api::GetWallTimerSeconds() - begin);
    average_ = average_ * 9 / 10 + time * 1 / 10;

    if (++frames_ % 100 == 0)
        printf("averaged adjusted frame rate: %f\n", 1 / average_);
}
/* }}} */
/* Idle Function {{{ */
void OnIdle_() {
    if (engine_->Update())
        ::glutPostRedisplay();
}
/* }}} */
/* Keyboard Events {{{ */
bool keys_[256];

void OnKeyDown_(unsigned char key, int x, int y) {
    static unsigned light(0);

    keys_[key] = true;

    switch (key) {
        case '1':
        case '2':
        case '3':
        case '4':
            mode_ = key - '0';
        break;

        case 'c':
            control_ = camera_;
        break;

        case 'e':
            control_ = eye_;
        break;

        case 'p':
            control_ = teapot_;
        break;

        case 't':
            control_ = torus_;
        break;

        case 'g':
            control_ = ground_;
        break;

        case 'm':
            mark_ = !mark_;
        break;

        case '[':
            light = (light + sizeof(lights_) / sizeof(lights_[0]) - 1) % (sizeof(lights_) / sizeof(lights_[0]));
            camera_ = eye_;
        break;

        case ']':
            printf("light #%u\n", light);
            camera_ = lights_[light];
            light = (light + 1) % (sizeof(lights_) / sizeof(lights_[0]));
        break;

        case '6':
        case '7':
        case '8':
        case '9':
            camera_ = lights_[key - '6' + 5 * 4];
        break;

        case ' ':
            printf("rain is now %s\n", (paused_ = !paused_) ? "paused" : "active");
        break;

        case 'w':
            engine_->SetWireframe(!engine_->GetWireframe());
        break;

        case 'q':
            exit(1);
        break;
    }

    glutPostRedisplay();
}

void OnKeyUp_(unsigned char key, int x, int y) {
    keys_[key] = false;
}
/* }}} */
/* Mouse Events {{{ */
enum Buttons_ {
    LeftButton_   = (1 << 0),
    MiddleButton_ = (1 << 1),
    RightButton_  = (1 << 2)
} buttons_;

int lastx_, lasty_;

void OnMouseClick_(int button, int up, int x, int y) {
    buttons_ = static_cast<Buttons_>((buttons_ & ~(1 << button)) | ((~up & 0x1) << button));

    lastx_ = x;
    lasty_ = y;
}

void OnMouseMove_(int x, int y) {
    int dx(x - lastx_);
    int dy(y - lasty_);

    lastx_ = x;
    lasty_ = y;

    switch (static_cast<unsigned>(buttons_)) {
        case RightButton_:
            if (!control_.IsEmpty()) {
                control_->SetPitch(control_->GetPitch() + dy);
                control_->SetYaw(control_->GetYaw() + dx);
                ::glutPostRedisplay();
            }
        break;

        case LeftButton_ | RightButton_:
            if (!control_.IsEmpty()) {
                control_->SetPostposition(control_->GetPostposition() + alg::Offset3f(0, 0, dy));
                ::glutPostRedisplay();
            }
        break;

        case LeftButton_:
        break;
    }
}
/* }}} */
/* Error Handling {{{ */
void OnError_() {
    CGerror error(cgGetError());
    _assert(error != 0);
    fprintf(stderr, "%s\n", cgGetErrorString(error));
    const char *listing(cgGetLastListing(*context_));
    if (listing != NULL)
        fprintf(stderr, "%s", listing);
    exit(1);
}
/* }}} */

void Split(Imf::Rgba *data, float *luminance, size_t width, size_t height, size_t sx, size_t sy, size_t ex, size_t ey, float total, unsigned level, unsigned offset);

void SplitHorizontal(Imf::Rgba *data, float *luminance, size_t width, size_t height, size_t sx, size_t sy, size_t ex, size_t ey, float total, unsigned level, unsigned offset) {
    float light(0.0f), target(total / 2);
    size_t y(sy);

  loop:
    _forall (size_t, x, sx, ex)
        light += luminance[x + y * width];

    if (y - sy < 3 || light < target) {
        ++y;
        goto loop;
    }

    _assert(y < ey);

    _forall (size_t, i, 0, 2)
    _forall (size_t, x, sx, ex) {
        data[2 * x + i + (2 * y - 1) * width * 2] = Imf::Rgba(1, 0, 1, 1);
        data[2 * x + i + 2 * y * width * 2] = Imf::Rgba(1, 0, 1, 1);
        data[2 * x + i + (2 * y + 1) * width * 2] = Imf::Rgba(1, 0, 1, 1);
    }

    level >>= 1;
    Split(data, luminance, width, height, sx, sy, ex, y, light, level, offset);
    Split(data, luminance, width, height, sx, y, ex, ey, total - light, level, offset + level);
}

void SplitVertical(Imf::Rgba *data, float *luminance, size_t width, size_t height, size_t sx, size_t sy, size_t ex, size_t ey, float total, unsigned level, unsigned offset) {
    float light(0.0f), target(total / 2);
    size_t x(sx);

  loop:
    _forall (size_t, y, sy, ey)
        light += luminance[x + y * width];

    if (x - sx < 3 || light < target) {
        ++x;
        goto loop;
    }

    _assert(x < ex);

    _forall (size_t, i, 0, 2)
    _forall (size_t, y, sy, ey) {
        data[(2 * x - 1) + (2 * y + i) * width * 2] = Imf::Rgba(1, 0, 1, 1);
        data[2 * x + (2 * y + i) * width * 2] = Imf::Rgba(1, 0, 1, 1);
        data[(2 * x + 1) + (2 * y + i) * width * 2] = Imf::Rgba(1, 0, 1, 1);
    }

    level >>= 1;
    Split(data, luminance, width, height, sx, sy, x, ey, light, level, offset);
    Split(data, luminance, width, height, x, sy, ex, ey, total - light, level, offset + level);
}

void Split(Imf::Rgba *data, float *luminance, size_t width, size_t height, size_t sx, size_t sy, size_t ex, size_t ey, float total, unsigned level, unsigned offset) {
    if (level == 1) {
        float x((sx + ex) / 2.0f);
        float y((sy + ey) / 2.0f);

        float yaw((x / width - 0.5f) * -360.f);
        float pitch((y / height - 0.5f) * 180.0f + 180.0f);

        _R<g3d::Camera> &light(lights_[offset]);
        light = new g3d::Camera();
        light->SetAspectRatio(static_cast<float>(window_->GetWidth()) / static_cast<float>(window_->GetHeight()));
        light->SetFieldOfView(40);
        light->SetYaw(yaw);
        light->SetPitch(pitch);
        light->SetPostposition(alg::Point3f(0, 0, 30));
        light->SetSystem(world_);

        //if (offset == 32) {
            _R<Sphere> marker(new Sphere(1.0f, 5));
            marker->SetYaw(yaw);
            marker->SetPitch(pitch);
            marker->SetPostposition(alg::Point3f(0, 0, 30));
            marker->SetSystem(markers_);
        //}

        unsigned xi(unsigned(x) * 2), yi(unsigned(y) * 2);

        /*if (offset / 4 == 5) {
            _forall (unsigned, x, sx, ex)
            _forall (unsigned, y, sy, ey)
                data[2 * x + 2 * y * width * 2] = Imf::Rgba(0, 1, 1);
        }*/

        _forall (unsigned, i, 0, 2) {
            int q(i * 2 - 1);

            data[(xi + 0) + (yi + 2*q) * width * 2] =
            data[(xi + 2*q) + (yi + 0) * width * 2] =
            data[(xi + q) + (yi + 0) * width * 2] =
            data[(xi + 0) + (yi + q) * width * 2] =
            data[(xi + q) + (yi + q) * width * 2] =
            data[(xi + q) + (yi - q) * width * 2] =
                Imf::Rgba(1, 1, 0, 1);
        }

        printf("light #%u: [%u,%u, %u,%u] (%f,%f) @(%f, %f)\n", offset, sx, sy, ex, ey, x, y, yaw, pitch);
    } else if (ex - sx < ey - sy)
        SplitHorizontal(data, luminance, width, height, sx, sy, ex, ey, total, level, offset);
    else
        SplitVertical(data, luminance, width, height, sx, sy, ex, ey, total, level, offset);
}

static _finline float luminance_(const Imf::Rgba &pixel) {
    return 0.2125 * pixel.r + 0.7154 * pixel.g + 0.0721 * pixel.b;
}

/* C++ EntryPoint {{{ */
int main(int argc, char *argv[]) {
    /* Window Initialization {{{ */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);

    window_ = new g3d::Window("CS280 Project");
    context_ = new g3d::Context();
    engine_ = new g3d::Engine(window_);
    /* }}} */
    /* Event Registration {{{ */
    glutDisplayFunc(&OnDisplay_);
    //glutIdleFunc(&OnIdle_);
    glutKeyboardFunc(&OnKeyDown_);
    glutKeyboardUpFunc(&OnKeyUp_);
    glutMouseFunc(&OnMouseClick_);
    glutMotionFunc(&OnMouseMove_);
    /* }}} */
    /* DevIL Configuration {{{ */
    ilInit();
    ilEnable(IL_CONV_PAL);
    ilutEnable(ILUT_OPENGL_CONV);
    ilutEnable(ILUT_GL_AUTODETECT_TEXTURE_TARGET);
    ilutRenderer(ILUT_OPENGL);
    /* }}} */

    cgSetErrorCallback(&OnError_);

    /* Cube Mapping {{{
    environment_ = new g3d::TextureCube();

    environment_->Load(
        "cubemap/Terrain_posx_0001.jpg",
        "cubemap/Terrain_negx_0001.jpg",
        "cubemap/Terrain_posy_0001.jpg",
        "cubemap/Terrain_negy_0001.jpg",
        "cubemap/Terrain_posz_0001.jpg",
        "cubemap/Terrain_negz_0001.jpg"
    );

    environment_->SetWrap(GL_CLAMP_TO_EDGE);

    ILuint images[6];
    ilGenImages(6, images);

    for (unsigned i(0); i != 6; ++i) {
        ilBindImage(i);
        error_();

        static const ILint Cube_[] = {
            IL_CUBEMAP_POSITIVEX,
            IL_CUBEMAP_NEGATIVEX,
            IL_CUBEMAP_POSITIVEY,
            IL_CUBEMAP_NEGATIVEY,
            IL_CUBEMAP_POSITIVEZ,
            IL_CUBEMAP_NEGATIVEZ
        };

        char texture[] = "cubemap/Terrain_...._0001.jpg";
        texture[19] = "xyz"[i / 2];
        memcpy(texture + 16, i % 2 == 0 ? "pos" : "neg", 3);
        printf("%s\n", texture);

        ilLoadImage(texture);
        error_();

        ilSetInteger(IL_IMAGE_CUBEFLAGS, Cube_[i]);
        error_();
        ilutGLTexImage(0);
        error_();
    }}} */

    world_ = new g3d::World();
    sky_ = new g3d::World();
    markers_ = new g3d::World();

    _R<g3d::Image2d> probe(new g3d::Image2d(argv[1]));
    environment_ = new g3d::Texture2d(probe);
    environment_->SetWrap(GL_REPEAT);

    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    float divisor(24);

    {
        Imf::Rgba *data(reinterpret_cast<Imf::Rgba *>(probe->GetData()));

        size_t width(probe->GetWidth());
        size_t height(probe->GetHeight());

        float *luminance(new float[width / 2 * height / 2]);

        float total(0.0f);

        _forall (size_t, y, 0, height / 2) {
            float bias(sin(2 * y * M_PI / height));
            _forall (size_t, x, 0, width / 2) {
                float value((
                    luminance_(data[(2 * x + 0) + (2 * y + 0) * width]) +
                    luminance_(data[(2 * x + 0) + (2 * y + 1) * width]) +
                    luminance_(data[(2 * x + 1) + (2 * y + 0) * width]) +
                    luminance_(data[(2 * x + 1) + (2 * y + 1) * width])
                ) * bias);

                total += value;
                luminance[x + y * width / 2] = value;
            }
        }

        gettimeofday(&end, NULL);
        printf("%lu.%06lu (%f)\n", end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec, total / width / height);

        Split(data, luminance, width / 2, height / 2, 0, 0, width / 2, height / 2, total, 64, 0);

        delete[] luminance;
    }

    gettimeofday(&end, NULL);
    printf("%lu.%06lu\n", end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);

    /*_forall (unsigned, i, 0, 64) {
        lights_[i]->SetYaw(0);
        lights_[i]->SetPitch(90);
        lights_[i]->SetRoll(0);
        lights_[i]->SetPostposition(alg::Point3f(0, 0, 30));
    }*/

    described_ = new g3d::Texture2d(probe);
    described_->SetWrap(GL_REPEAT);

    /*bottom_ = new g3d::Texture2d();
    bottom_->Load("bottom.jpg");
    bottom_->SetWrap(GL_REPEAT);*/

    _forall (unsigned, i, 0, 8) {
        depth_[i] = new g3d::Texture2d();
        depth_[i]->Copy(GL_DEPTH_COMPONENT24, window_->GetWidth(), window_->GetHeight(), GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        depth_[i]->SetWrap(GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }

    position_ = new g3d::Material();
    position_->SetVertexProgram(new g3d::Program(context_, VertexProfile_, "position.vcg"));
    position_->SetFragmentProgram(new g3d::Program(context_, FragmentProfile_, "position.fcg"));

    diffuse_ = new g3d::Material();
    _R<g3d::Program> diffuse_vcg(new g3d::Program(context_, VertexProfile_, "diffuse.vcg"));
    diffuse_vcg->SetParameter("light", alg::Point3f(0, 100, -50));
    diffuse_->SetVertexProgram(diffuse_vcg);
    diffuse_->SetFragmentProgram(new g3d::Program(context_, FragmentProfile_, "diffuse.fcg"));

    flat_ = new g3d::Material();
    flat_->SetVertexProgram(new g3d::Program(context_, VertexProfile_, "flat.vcg"));
    flat_->SetFragmentProgram(new g3d::Program(context_, FragmentProfile_, "flat.fcg"));

    projected_ = new g3d::Material();
    projected_->SetVertexProgram(new g3d::Program(context_, VertexProfile_, "projected.vcg"));
    _R<g3d::Program> projected_fcg(new g3d::Program(context_, FragmentProfile_, "projected.fcg"));
    projected_fcg->SetParameter("bottom", described_);
    projected_fcg->SetParameter("width", probe->GetWidth());
    projected_fcg->SetParameter("height", probe->GetHeight());
    projected_->SetFragmentProgram(projected_fcg);

    shadowed_ = new g3d::Material();
    shadowed_->SetVertexProgram(new g3d::Program(context_, VertexProfile_, "shadowed.vcg"));
    shadowed_fcg_ = new g3d::Program(context_, FragmentProfile_, "shadowed.fcg");
    shadowed_fcg_->SetParameter("divisor", divisor);
    shadowed_->SetFragmentProgram(shadowed_fcg_);

    eye_ = new g3d::Camera();
    eye_->SetAspectRatio(static_cast<float>(window_->GetWidth()) / static_cast<float>(window_->GetHeight()));
    eye_->SetSystem(world_);
    eye_->SetPostposition(alg::Point3f(0, 0, 100));

    ground_ = new StupidGrid(30.0f, 100);
    ground_->SetSystem(world_);
    ground_->SetPreposition(alg::Point3f(0, -5, 0));
    ground_->SetPitch(90);

    torus_ = new Torus();
    torus_->SetSystem(world_);
    torus_->SetPreposition(alg::Point3f(5, 0, 0));

    teapot_ = new Teapot();
    teapot_->SetSystem(world_);
    teapot_->SetPreposition(alg::Point3f(-5, 0, 0));

    sphere_ = new Sphere(100.0f, 20);
    sphere_->SetSystem(sky_);
    sphere_->SetPreposition(alg::Point3f(0, 20, 0));

    control_ = eye_;
    camera_ = eye_;

    glutMainLoop();
    return 0;
}
/* }}} */
