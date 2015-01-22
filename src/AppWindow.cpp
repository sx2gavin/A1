#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
    setWindowTitle("488 Tetrominoes on the Wall");

    QGLFormat glFormat;
    glFormat.setVersion(3,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    QVBoxLayout *layout = new QVBoxLayout;
    // m_menubar = new QMenuBar;
    m_viewer = new Viewer(glFormat, this);
    layout->addWidget(m_viewer);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(layout);
    m_viewer->show();

    createMenu();
    createActions();
}

void AppWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        QCoreApplication::instance()->quit();
    } else if (event->key() == Qt::Key_T) {
        std::cerr << "Hello!" << std::endl;
    } else if (event->key() == Qt::Key_N) {
		newGame();
	} else if (event->key() == Qt::Key_R) {
		resetGame();
	} else if (event->key() == Qt::Key_W) {
		wireframeMode();
	} else if (event->key() == Qt::Key_F) {
		faceMode();
	} else if (event->key() == Qt::Key_M) {
		multicolouredMode();
	} else if (event->key() == Qt::Key_1) {
		slowSpeed();
	} else if (event->key() == Qt::Key_2) {
		mediumSpeed();
	} else if (event->key() == Qt::Key_3) {
		fastSpeed();
	} else {
        QWidget::keyPressEvent(event);
    }
}

void AppWindow::createActions() {
	// Creates a New game action in the Application menu
	QAction* newGameAct = new QAction(tr("&New game"), this);
	newGameAct->setStatusTip(tr("Start a new game"));	
	newGameAct->setShortcut(Qt::Key_N);
	connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));
	m_menu_app->addAction(newGameAct);

	QAction* resetGameAct = new QAction(tr("&Reset"), this);
	resetGameAct->setStatusTip(tr("Reset current game"));
	resetGameAct->setShortcut(Qt::Key_R);
	connect(resetGameAct, SIGNAL(triggered()), this, SLOT(resetGame()));
	m_menu_app->addAction(resetGameAct);

    // Creates a new action for quiting and pushes it onto the menu actions vector 
    QAction* quitAct = new QAction(tr("&Quit"), this);
    // We set the accelerator keys
    // Alternatively, you could use: setShortcuts(Qt::CTRL + Qt::Key_P); 
    quitAct->setShortcuts(QKeySequence::Quit);

    // Set the tip
    quitAct->setStatusTip(tr("Exits the file"));

    // Connect the action with the signal and slot designated
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
	m_menu_app->addAction(quitAct);

	// wireframe
	QAction* wireframeAct = new QAction(tr("&Wire-frame"), this);
	wireframeAct->setStatusTip(tr("Switch to wire-frame mode"));
	wireframeAct->setShortcut(Qt::Key_W);
	connect(wireframeAct, SIGNAL(triggered()), this, SLOT(wireframeMode()));
	m_menu_draw->addAction(wireframeAct);
	
	// face	
	QAction* faceAct = new QAction(tr("&Face"), this);
	faceAct->setStatusTip(tr("Switch to face mode"));
	faceAct->setShortcut(Qt::Key_F);

	connect(faceAct, SIGNAL(triggered()), this, SLOT(faceMode()));
	m_menu_draw->addAction(faceAct);

	// multicoloured
	QAction* multicolouredAct = new QAction(tr("&Multicoloured"), this);
	multicolouredAct->setStatusTip(tr("Switch to face mode"));
	multicolouredAct->setShortcut(Qt::Key_M);
	connect(multicolouredAct, SIGNAL(triggered()), this, SLOT(multicolouredMode()));
	m_menu_draw->addAction(multicolouredAct);
	
	// slow speed	
	QAction* slowSpeedAct = new QAction(tr("&Slow"), this);
	slowSpeedAct->setStatusTip(tr("Switch to slow speed mode"));
	slowSpeedAct->setShortcut(Qt::Key_1);
	connect(slowSpeedAct, SIGNAL(triggered()), this, SLOT(slowSpeed()));
	m_menu_speed->addAction(slowSpeedAct);
	
	// medium speed
	QAction* mediumSpeedAct = new QAction(tr("&Medium"), this);
	mediumSpeedAct->setStatusTip(tr("Switch to medium speed mode"));
	mediumSpeedAct->setShortcut(Qt::Key_2);
	connect(mediumSpeedAct, SIGNAL(triggered()), this, SLOT(mediumSpeed()));
	m_menu_speed->addAction(mediumSpeedAct);

	// fast speed
	QAction* fastSpeedAct = new QAction(tr("&Fast"), this);
	fastSpeedAct->setStatusTip(tr("Switch to fast speed mode"));
	fastSpeedAct->setShortcut(Qt::Key_3);
	connect(fastSpeedAct, SIGNAL(triggered()), this, SLOT(fastSpeed()));
	m_menu_speed->addAction(fastSpeedAct);
}

void AppWindow::createMenu() {
    m_menu_app = menuBar()->addMenu(tr("&Application"));
	m_menu_draw = menuBar()->addMenu(tr("&Draw Mode"));
	m_menu_speed = menuBar()->addMenu(tr("&Speed"));
}

void AppWindow::newGame(){
	m_viewer->newGame();	
}

void AppWindow::resetGame() {
	m_viewer->resetGame();
}

void AppWindow::wireframeMode() {
	m_viewer->wireframeMode();
}

void AppWindow::faceMode() {
	m_viewer->faceMode();
}

void AppWindow::multicolouredMode() {
	m_viewer->multicolouredMode();
}

void AppWindow::slowSpeed() {
	m_viewer->slowSpeed();
}

void AppWindow::mediumSpeed() {
	m_viewer->mediumSpeed();
}

void AppWindow::fastSpeed() {
	m_viewer->fastSpeed();
}

