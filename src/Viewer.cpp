// #include <GL/glew.h>
#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"
#include <iostream>
#include <math.h>
// #include <GL/gl.h>
#include <GL/glu.h>


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mVertexArrayObject(this)
#else
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif
{
	shiftPressed = false;
	mode = WIRE_FRAME;
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
    mTimer->start(1000/30);

	// drawing U-well geometry.
	QMatrix4x4 tempMatrix;
	tempMatrix.setToIdentity();

	tempMatrix.translate(-6, 9, 0);

	addCube(tempMatrix, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));

	for (int i = 0; i < 20; i++) {
		tempMatrix.translate(0, -1, 0);
		addCube(tempMatrix, QVector4D(1.0f, 0.0f, 1.0f, 1.0f));
	}

	for (int i = 0; i < 11; i++) {
		tempMatrix.translate(1, 0, 0);
		addCube(tempMatrix, QVector4D(0.0f, 1.0f, 0.0f, 1.0f));
	}

	for (int i = 0; i < 20; i++) {
		tempMatrix.translate(0, 1, 0);
		addCube(tempMatrix, QVector4D(0.0f, 1.0f, 1.0f, 1.0f));
	}
	
}

Viewer::~Viewer() {

}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 600);
}

// Add a cube in the view with modelMatrix and color attribute
void Viewer::addCube(QMatrix4x4 modelMatrix, QVector4D color) {
	mModelMatrices.push_back(modelMatrix);
	mCubeColors.push_back(color);
}

void Viewer::defineCubeGeometry() {
	float cubeData[] = {
		// Faces of the cube
		// X   Y    Z
		// Back face
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Top face
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,

		// Right face
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,

		// Left face
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		// Bottom face
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,

		// Front face
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	
	};

    mVertexBufferObject.allocate(cubeData, 6 * 6 * 3 * sizeof(float));

}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.7, 0.7, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
        std::cerr << "Cannot load vertex shader." << std::endl;
        return;
    }

    if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
        std::cerr << "Cannot load fragment shader." << std::endl;
        return;
    }

    if ( !mProgram.link() ) {
        std::cerr << "Cannot link shaders." << std::endl;
        return;
    }


#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
#else

    /*
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of
     * QOpenGLBuffer.
     */
    uint vao;

    typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);

    _glGenVertexArrays glGenVertexArrays;
    _glBindVertexArray glBindVertexArray;

    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QGLBuffer::StaticDraw);
#endif

    if (!mVertexBufferObject.bind()) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    // mVertexBufferObject.allocate(triangleData, 3 * 3 * sizeof(float));
	defineCubeGeometry();

    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    // mPerspMatrixLocation = mProgram.uniformLocation("cameraMatrix");
    mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
	mColorLocation = mProgram.uniformLocation("vertColor");
}

void Viewer::paintGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mVertexArrayObject.bind();
#endif
	for (int i = 0; i < mModelMatrices.size(); i++) {	
		mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mModelMatrices[i]);
		mProgram.setUniformValue(mColorLocation, mCubeColors[i]);
		if (mode == WIRE_FRAME) {
			glDrawArrays(GL_LINE_STRIP, 0, 12 * 3);
		} else if (mode == FACE) {		
			glDrawArrays(GL_TRIANGLES, 0, 12 * 3); 
		}
	}
}

void Viewer::resizeGL(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    mPerspMatrix.setToIdentity();
    mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " pressed\n";
	pressedMouseButton = event->button();
	prePos = event->x();
	setFocus();
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " released\n";
	pressedMouseButton = Qt::NoButton;
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;

	if (shiftPressed) {
		mTransformMatrix.scale( event->x() > prePos ? 1.05:0.95);
	} else if (pressedMouseButton == Qt::LeftButton) {
		mTransformMatrix.rotate(event->x() - prePos, QVector3D(1, 0, 0));
	} else if (pressedMouseButton == Qt::MidButton) {
		mTransformMatrix.rotate(event->x() - prePos, QVector3D(0, 1, 0));
	} else if (pressedMouseButton == Qt::RightButton) {
		mTransformMatrix.rotate(event->x() - prePos, QVector3D(0, 0, 1));
	}
	prePos = event->x();
	update();
}

void Viewer::keyPressEvent ( QKeyEvent * event ) {
	if ( event->key() == Qt::Key_Shift ) {
		std::cerr << "shift key pressed." << std::endl;
		shiftPressed = true;
	}
}

void Viewer::keyReleaseEvent ( QKeyEvent * event ) {
	if (event->key() == Qt::Key_Shift) {
		std::cerr << "shift key released." << std::endl;
		shiftPressed = false;
	}
}


QMatrix4x4 Viewer::getCameraMatrix() {
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
    mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float x, float y, float z) {
    mTransformMatrix.rotate(x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
    mTransformMatrix.scale(x, y, z);
}

