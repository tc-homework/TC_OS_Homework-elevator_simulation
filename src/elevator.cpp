#include "elevator.h"
#include "ui_elevator.h"

elevator::elevator(QWidget *parent, int _no, int _FLOOR_NUM) : QWidget(parent), ui(new Ui::elevator){
	ui->setupUi(this);
	no = _no;
	FLOOR_NUM = _FLOOR_NUM;
	QGroupBox *box = ui->groupBox_destination;

	// resize the window and box's size to include all the buttons.
	if(FLOOR_NUM > 20){
		box ->setGeometry(60, 10, 20 + 210 * (FLOOR_NUM / 21 + 1), 200);
		this->setGeometry(0, 0, 210 * (FLOOR_NUM / 21 + 1) + 100, 380);
	}

	// Draw btns.
	for(int i = 0; i < FLOOR_NUM; i++){
		QPushButton *btn = new QPushButton(box);
			btn->setGeometry(20+40*(i%5)+210*(i/20), 30+40*(i%20/5), 30, 30);
			btn->setText(QString::number(i+1, 10));
			btn->show();
			connect(btn, &QPushButton::clicked, this, [=] {
				Qbtns[unsigned(i)]->setEnabled(false);
				destsInsider.push_back(i);
			});
		Qbtns.push_back(btn);
	}

	// Set title , label, slider on the window.
	this->setWindowTitle("电梯: " + QString::number(no+1, 10));
	ui->label_NUM_FLOOR->setText(QString::number(FLOOR_NUM, 10));
	ui->verticalSlider_currentFloor->setMaximum(FLOOR_NUM);

	// Setup the timer. Run timer_elevator_tick() every ELEVATOR_TIMER_TICK ms.
	QTimer *timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &elevator::timer_elevator_tick);
	timer->start(ELEVATOR_TIMER_TICK);

	// Setup buttons: ["开门", "关门", "报警"].
	connect(ui->pushButton_opendoor, &QPushButton::clicked, this, [=]{
		if(status == 0) { if(door == 0 || door == 2) open_door(); }
		else QMessageBox::about(nullptr, "Error!", "运行中无法开门.");
	});
	connect(ui->pushButton_closedoor, &QPushButton::clicked, this, [=]{
		if(door == 0 || door == 2) QMessageBox::about(nullptr, "Error!", "门已经关上了.");
	});
	connect(ui->pushButton_alert, &QPushButton::clicked, this, [=]{ QMessageBox::about(nullptr, "Alert!", "已发出警报.");});
}

elevator::~elevator(){
	delete ui;
}

void elevator::open_door(){
	door = 3; renew_label();//Opening: 800ms.
	QElapsedTimer t1;
	t1.start();
	while(t1.elapsed() < 800) QCoreApplication::processEvents();

	door = 1; renew_label();// Opened: 1000ms.
	QElapsedTimer t2;
	t2.start();
	while(t2.elapsed() < 1000) QCoreApplication::processEvents();

	door = 2;  renew_label(); //Closing: 800ms.
	QElapsedTimer t3;
	t3.start();
	while(t3.elapsed() < 800) QCoreApplication::processEvents();

	door = 0;  renew_label(); //Closed.
}

void elevator::renew_label(){
	ui->label_status->setText(statusStr[status]);
	ui->label_door->setText(doorStr[door]);
	ui->label_current->setText(QString::number(currentFloor+1, 10));
	ui->label_current_2->setText(QString::number(currentFloor+1, 10));
	ui->verticalSlider_currentFloor->setValue(currentFloor + 1);
}

bool elevator::recive_request(bool up, int floor, bool forceRecive){
	if(!forceRecive && (    ( up && status == 2 && currentFloor > floor )
						 || ( !up && status == 1 && currentFloor < floor )
						)) return false;
	bool hasIn = false;
	for(auto i : destsOutside) if(i == floor) hasIn = true;
	if(!hasIn) destsOutside.push_back(floor);

	// Force to check.
	status == 0 ? check_when_pause() : check_when_run();
	return true;
}

void elevator::cancel_request(int floor){
	auto it = std::find(destsOutside.begin(), destsOutside.end(), floor);
	if(it != destsOutside.end()){
		destsOutside.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}
}

void elevator::check_when_pause(){
	dests.insert(dests.end(), destsInsider.begin(), destsInsider.end());
	dests.insert(dests.end(), destsOutside.begin(), destsOutside.end());
	if(dests.size() == 0) return ;

	bool upDest   = false; // If has tasks needing upstair.
	bool downDest = false; // If has tasks needing downstair.
	for(auto i : dests){
		if(i <  currentFloor) downDest = true;
		if(i >  currentFloor) upDest = true;
		if(i == currentFloor) open_door();
	}

	auto it = std::find(destsInsider.begin(), destsInsider.end(), currentFloor);
	if(it != destsInsider.end()){
		destsInsider.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}
	it = std::find(destsOutside.begin(), destsOutside.end(), currentFloor);
	if(it != destsOutside.end()){
		destsOutside.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}

	if(currentFloor >= FLOOR_NUM / 2 && (upDest))				  status = 1;
	else if(currentFloor >= FLOOR_NUM / 2 && !upDest && downDest) status = 2;
	else if(currentFloor <= FLOOR_NUM / 2 && (downDest))		  status = 2;
	else if(currentFloor <= FLOOR_NUM / 2 && upDest && !downDest) status = 1;
	dests.clear();
}

void elevator::check_when_run(){
	dests.insert(dests.end(), destsInsider.begin(), destsInsider.end());
	dests.insert(dests.end(), destsOutside.begin(), destsOutside.end());
	if(dests.size() == 0){
		status = 0; // To stop.
		return;
	}
	bool upDest   = false;
	bool downDest = false;
	for(auto i : dests){
		if(i < currentFloor){downDest = true;}
		if(i > currentFloor){upDest = true;}
		if(i == currentFloor){
			auto beforeStatus = status;
			status = 0;
			open_door();
			status = beforeStatus;
		}
	}
	auto it = std::find(destsInsider.begin(), destsInsider.end(), currentFloor);
	if(it != destsInsider.end()){
		destsInsider.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}
	it = std::find(destsOutside.begin(), destsOutside.end(), currentFloor);
	if(it != destsOutside.end()){
		destsOutside.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}

	if(status == 1 && !upDest && downDest)		status = 2;
	else if(status == 2 && upDest && !downDest) status = 1;
	else if(!upDest && !downDest)				status = 0;
	dests.clear();
}

void elevator::timer_elevator_tick(){
	currentFloor += status == 1 ? 1 : status == 2 ? -1 : 0;
	status == 0 ? check_when_pause() : check_when_run();
	renew_label();
}
