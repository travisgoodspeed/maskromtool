#include "romsecond.h"
#include "ui_romsecond.h"

#include "maskromtool.h"

RomSecond::RomSecond(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomSecond)
{
    ui->setupUi(this);
    view=ui->graphicsView;
}

RomSecond::~RomSecond(){
    delete ui;
}


void RomSecond::keyPressEvent(QKeyEvent *event){
    if(mrt){
        mrt->keyPressEvent(event);
    }
}


void RomSecond::enableOpenGL(unsigned int antialiasing){
    //Enabled OpenGL.  Maybe better performance, but it's currently broken.
    QOpenGLWidget *gl = new QOpenGLWidget();
    if(antialiasing>0){
        QSurfaceFormat format;
        format.setSamples(antialiasing);
        gl->setFormat(format);
    }
    gl->setMouseTracking(true); //Required to track all mouse clicks.
    view->setViewport(gl);
}
