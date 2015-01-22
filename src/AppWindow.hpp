#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <vector>
#include "Viewer.hpp"

class AppWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
	void newGame();
	void resetGame();
	void wireframeMode();
	void faceMode();
	void multicolouredMode();
	void slowSpeed();
	void mediumSpeed();
	void fastSpeed();

private:
    void createActions();
    void createMenu();

    // Each menu itself
    QMenu* m_menu_app;
	QMenu* m_menu_draw;
	QMenu* m_menu_speed;

    std::vector<QAction*> m_menu_actions;
    Viewer* m_viewer;
};

#endif
