// #include <GL/glew.h>
#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"
#include "game.hpp"
#include <iostream>
#include <math.h>
// #include <GL/gl.h>
#include <GL/glu.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#define UWELLCOLOR (QVector4D(0.0f, 0.0f, 1.0f, 1.0f))

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mVertexArrayObject(this)
#else
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif
{
	game = new Game(10, 20);
	game->reset();
	shiftPressed = false;
	mode = FACE;
	mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(updateGame()));
	// fps = 30
    mTimer->start(1000/30);
	counter = 0; 
	// speed indicates the numbers of frames of time the game needs to make the block go down by
	// one step.
	speed = 30;
	setFocusPolicy(Qt::StrongFocus);
	persistenceSpeed = 0;
	persistenceAxis = QVector3D(0, 0, 0);

	// drawing U-well geometry.
	QMatrix4x4 tempMatrix;
	tempMatrix.setToIdentity();

	tempMatrix.translate(-6, 9, 0);

	addWell(tempMatrix, UWELLCOLOR);

	for (int i = 0; i < 20; i++) {
		tempMatrix.translate(0, -1, 0);
		addWell(tempMatrix, UWELLCOLOR);
	}

	for (int i = 0; i < 11; i++) {
		tempMatrix.translate(1, 0, 0);
		addWell(tempMatrix, UWELLCOLOR);
	}

	for (int i = 0; i < 20; i++) {
		tempMatrix.translate(0, 1, 0);
		addWell(tempMatrix, UWELLCOLOR);
	}

	drawCurrentGameState();
}

Viewer::~Viewer() {
	delete game;
}

void Viewer::newGame() {
	if (game != NULL) {
		delete game;
	}
	game = new Game(10, 20);
}

void Viewer::resetGame() {
	game->reset();
}

void Viewer::wireframeMode() {
	mode = WIRE_FRAME;
}

void Viewer::faceMode() {
	mode = FACE;
}

void Viewer::multicolouredMode() {
	mode = MULTICOLOURED;
}

void Viewer::slowSpeed() {
	speed = 30;
}

void Viewer::mediumSpeed() {
	speed = 20;
}

void Viewer::fastSpeed() {
	speed = 10;
}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 600);
}

// Add a cube in the view with modelMatrix and color attribute
void Viewer::addWell(QMatrix4x4 modelMatrix, QVector4D color) {
	mWellModelMatrices.push_back(modelMatrix);
	mWellColors.push_back(color);
}

void Viewer::addBlock(int x, int y, QVector4D color) {
	QMatrix4x4 modelMatrix;
	modelMatrix.setToIdentity();
	modelMatrix.translate(x-5, y-10, 0);
	mBlockModelMatrices.push_back(modelMatrix);
	mBlockColors.push_back(color);
}

void Viewer::drawCurrentGameState() {
	mBlockModelMatrices.clear();
	mBlockColors.clear();
	// Row
	for (int r = 0; r < game->getHeight() + 4; r++) {
		// Column
		for (int c = 0; c < game->getWidth(); c++) {
			switch ( game->get(r, c)) {
				case 0:
					addBlock(c, r, QVector4D(1.0f, 0.0f, 0.0f, 1.0f));
					break;
				case 1:
					addBlock(c, r, QVector4D(0.0f, 1.0f, 0.0f, 1.0f));
					break;
				case 2:
					addBlock(c, r, QVector4D(0.0f, 1.0f, 1.0f, 1.0f));
					break;
				case 3:
					addBlock(c, r, QVector4D(0.0f, 1.0f, 0.5f, 1.0f));
					break;
				case 4:
					addBlock(c, r, QVector4D(0.0f, 0.5f, 0.5f, 1.0f));
					break;
				case 5:
					addBlock(c, r, QVector4D(1.0f, 1.0f, 0.0f, 1.0f));
					break;
				case 6:
					addBlock(c, r, QVector4D(1.0f, 0.0f, 1.0f, 1.0f));
					break;
				case 7:
					addBlock(c, r, QVector4D(0.5f, 0.0f, 0.5f, 1.0f));
					break;
				case -1:
					break;
			}
		}	
	}	
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
		1.0f, 1.0f, 1.0f,

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
	// glDepthFunc(GL_LESS);

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
	for (int i = 0; i < mWellModelMatrices.size(); i++) {	
		mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mWellModelMatrices[i]);
		mProgram.setUniformValue(mColorLocation, mWellColors[i]);
		if (mode == WIRE_FRAME) {
			glDrawArrays(GL_LINE_STRIP, 0, 12 * 3);
		} else if (mode == FACE) {		
			glDrawArrays(GL_TRIANGLES, 0, 12 * 3); 
		}
	}

	for (int i = 0; i < mBlockModelMatrices.size(); i++) {
		mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mBlockModelMatrices[i]);
		mProgram.setUniformValue(mColorLocation, mBlockColors[i]);
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
	std::cerr << "speed = " << persistenceSpeed;

	if (pressedMouseButton == Qt::LeftButton) {
		persistenceAxis = QVector3D(1, 0, 0);
	} else if (pressedMouseButton == Qt::MidButton) {
		persistenceAxis = QVector3D(0, 1, 0);
	} else if (pressedMouseButton == Qt::RightButton) {
		persistenceAxis = QVector3D(0, 0, 1);
	}
	pressedMouseButton = Qt::NoButton;
}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
	persistenceSpeed = event->x() - prePos;

	if (shiftPressed) {
		mTransformMatrix.scale( event->x() > prePos ? 1.05:0.95);
	} else if (pressedMouseButton == Qt::LeftButton) {
		mTransformMatrix.rotate(persistenceSpeed, QVector3D(1, 0, 0));
	} else if (pressedMouseButton == Qt::MidButton) {
		mTransformMatrix.rotate(persistenceSpeed, QVector3D(0, 1, 0));
	} else if (pressedMouseButton == Qt::RightButton) {
		mTransformMatrix.rotate(persistenceSpeed, QVector3D(0, 0, 1));
	}
	prePos = event->x();
	update();
}

void Viewer::keyPressEvent ( QKeyEvent * event ) {
	if ( event->key() == Qt::Key_Shift ) {
		std::cerr << "shift key pressed." << std::endl;
		shiftPressed = true;
	} else if (event->key() == Qt::Key_Left) {
		std::cerr << "left key pressed." << std::endl;
		if (game != NULL) {
			game->moveLeft();
		}
	} else if (event->key() == Qt::Key_Right) {
		std::cerr << "right key pressed." << std::endl;
		if (game != NULL) {
			game->moveRight();
		}
	} else if (event->key() == Qt::Key_Up) {
		std::cerr << "up key pressed." << std::endl;
		if (game != NULL) {
			game->rotateCCW();
		}
	} else if (event->key() == Qt::Key_Down) {
		std::cerr << "down key pressed." << std::endl;
		if (game != NULL) {
			game->rotateCW();
		}
	} else if (event->key() == Qt::Key_Space) {
		std::cerr << "space key pressed." << std::endl;
		if (game != NULL) {
			game->drop();
		}
	} else if (event->key() == Qt::Key_0) {
		game->tick();
	} else {
		QWidget::keyPressEvent(event);
	}
	drawCurrentGameState();
	update();
}

void Viewer::updateGame() {
	if ( counter < speed ) {
		counter++;
	} else {
		game->tick();
		counter = 0;
	}
	if (abs(persistenceSpeed) >= 2 && pressedMouseButton == Qt::NoButton) {
		mTransformMatrix.rotate(persistenceSpeed, persistenceAxis);
	}
	drawCurrentGameState();
	update();
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

