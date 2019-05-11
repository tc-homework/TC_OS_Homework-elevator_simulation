#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
	ui->setupUi(this);
		connect(ui->pushButton_exit, &QPushButton::clicked, this, [=]{exit(0);});
		connect(ui->pushButton_run, &QPushButton::clicked, this, &MainWindow::run);
		connect(ui->pushButton_stop, &QPushButton::clicked, this, &MainWindow::my_stop);
	ui->pushButton_stop->setEnabled(false);
}

MainWindow::~MainWindow(){
	delete ui;
}

void MainWindow::run(){
	int FLOOR_NUM = ui->spinBox_floor->value();
	for(int i = 0; i < ui->spinBox_elevator->value(); i++){
		elevator *ele = new elevator(nullptr, i, FLOOR_NUM);
		ele->move( 5 + i % (GetSystemMetrics(SM_CXSCREEN) / ele->width()) * (ele->width() + 5),
				   5 + ((i + 1) * ele->width() / GetSystemMetrics(SM_CXSCREEN)) * (ele->height() + 35)
				 );
		ele->show();
		eles.push_back(ele);
	}
	building *bld = new building(nullptr, eles, FLOOR_NUM, ui->spinBox_alg->value());
	bld->move(100,100);
	bld->show();
	a_building = bld;
	ui->pushButton_run->setEnabled(false);
	ui->pushButton_stop->setEnabled(true);
}

void MainWindow::my_stop(){
	for(auto i : eles) i->close();
	eles.clear();
	a_building->close();
	a_building = nullptr;
	ui->pushButton_run->setEnabled(true);
	ui->pushButton_stop->setEnabled(false);
}


