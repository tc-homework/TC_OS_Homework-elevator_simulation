#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "windows.h"
#include "building.h"
#include <QCloseEvent>
#include <QMainWindow>

// This class is the main window after the program runs

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow{
	Q_OBJECT
	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();
	private:
		Ui::MainWindow *ui;
	protected:
		void closeEvent(QCloseEvent *event){ exit(0); } // rewrite the event after clicked of 'X'.
	public:
		std::vector<elevator*> eles; // Used to store elevators, it should be empty before every simulation.
		building *a_building = nullptr; //Used to store a build class. See "building.h".
	public:
		void run(); // To start simiulations.
		void my_stop(); // To stop simiulations.
};

#endif // MAINWINDOW_H
