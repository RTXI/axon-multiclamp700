#include <QtGui>
#include "axon-multiclamp-700.h"
#include <iostream>

extern "C" Plugin::Object * createRTXIPlugin(void) {
	return new MultiClamp();
};

static DefaultGUIModel::variable_t vars[] = {
	{ "Mode Input", "", DefaultGUIModel::INPUT, },
	{ "Mode Output", "", DefaultGUIModel::OUTPUT, },
	{ "Input Channel", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER, },
	{ "Output Channel", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER, },
	{ "VClamp Gain", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, }, 
	{ "IClamp Gain", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

MultiClamp::MultiClamp(void) : DefaultGUIModel("Axon MultiClamp 700 Controller", ::vars, ::num_vars) {
	setWhatsThis("<p>Yeah, I'll get to this later... <br>-Ansel</p>");
	DefaultGUIModel::createGUI(vars, num_vars);
	initParameters();
	createGUI(vars, num_vars);
//	customizeGUI();
	update( INIT );
	refresh();
};

MultiClamp::~MultiClamp(void) {};

void MultiClamp::initParameters(void) {
	input_channel = 0;
	output_channel = 1;

	vclamp_gain = iclamp_gain = 1;
};

void MultiClamp::update(DefaultGUIModel::update_flags_t flag) {
	switch(flag) {
		case INIT:
			setParameter("Input Channel", input_channel);
			setParameter("Output Channel", output_channel);
			setParameter("VClamp Gain", vclamp_gain);
			setParameter("IClamp Gain", iclamp_gain);
			break;
		
		case MODIFY:
			break;

		case PAUSE:
			break;

		case UNPAUSE:
			break;

		case PERIOD:
			break;

		default:
			break;
	}
}

void customizeGUI(void) {
	
}

/*
void MultiClamp::createGUI(DefaultGUIModel::variable_t *var, int size) {
	QWidget::setAttribute(Qt::WA_DeleteOnClose);

	// Make Mdi
	DefaultGUIModel::subWindow = new QMdiSubWindow;
	subWindow->setWindowFlags(Qt::WindowCloseButtonHint);
	subWindow->setOption(QMdiSubWindow::RubberBandResize, true);
	subWindow->setOption(QMdiSubWindow::RubberBandMove, true);
	MainWindow::getInstance()->createMdi(subWindow);

	layout = new QGridLayout;
	setLayout(layout);
	subWindow->setWidget(this);
	
	// create utilitybox
	utilityGroup = new QGroupBox; 
	QHBoxLayout *utilityLayout = new QHBoxLayout;
	utilityGroup->setLayout(utilityLayout);

	pauseButton = new QPushButton("Pause", this);
	pauseButton->setCheckble(true);
	QObject::connect(pauseButton, SIGNAL(toggled(bool)),this,SLOT(pause(bool)));
	utilityLayout->addWidget(pauseButton);

	modifyButton = new QPushButton("Modify", this);
	QObject::connect(modifyButton,SIGNAL(clicked(void)),this,SLOT(modify(void)));
	utilityLayout->addWidget(modifyButton);

	unloadButton = new QPushButton("Unload", this);
	QObject::connect(unloadButton,SIGNAL(clicked(void)),this,SLOT(modify(void)));
	utilityLayout->addWidget(unloadButton);

	// create amp control buttons
	QGroupBox *ampModeGroup = new QGroupBox("Amplifier Mode");
	QHBoxLayout *ampModeGroupLayout = new QHBoxLayout;
	ampModeGroup->setLayout(ampModeGroupLayout);
	
	iclamp_button = new QPushButton("IClamp");
	vclamp_button = new QPushButton("VClamp");
	none_button = new QPushButton("I=0");
	iclamp_button->setCheckable(true);
	vclamp_button->setCheckable(true);
	none_button->setCheckable(true);
	QButtonGroup *ampButtonGroup = new QButtonGroup;
	ampButtonGroup->addButton(iclamp_button, 0);
	ampButtonGroup->addButton(vclamp_button, 1);
	ampButtonGroup->addButton(none_button, 2);
	ampButtonGroup->setExclusive(true);
	ampModeGroupLayout->addWidget(vclamp_button);
	ampModeGroupLayout->addWidget(none_button);
	ampModeGroupLayout->addWidget(iclamp_button);

	// Add groups to layout
	layout->addWidget(ampModeGroup,0,0);
	layout->addWidget(ampModeGroup,1,0);
	show();
}
*/
