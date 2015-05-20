#include "axon-multiclamp700.h"
#include <iostream>

// Wrapper for QComboBox. Turns red when changed and black when "Set DAQ" is hit.
MultiClampComboBox::MultiClampComboBox(QWidget *parent) : QComboBox(parent) {
	QObject::connect(this, SIGNAL(activated(int)), this, SLOT(redden(void)));
}

MultiClampComboBox::~MultiClampComboBox(void) {}

void MultiClampComboBox::blacken(void) {
	this->setStyleSheet("QComboBox { color:black; }");
}

void MultiClampComboBox::redden(void) {
	this->setStyleSheet("QComboBox { color:red; }");
}

MultiClampSpinBox::MultiClampSpinBox(QWidget *parent) : QSpinBox(parent) {
	QObject::connect(this, SIGNAL(valueChanged(int)), this, SLOT(redden(void)));
}

MultiClampSpinBox::~MultiClampSpinBox(void) {}

void MultiClampSpinBox::blacken(void) {
	this->setStyleSheet("QSpinBox { color:black; }");
}

void MultiClampSpinBox::redden(void) {
	this->setStyleSheet("QSpinBox { color:red; }");
}

// function used to get all available input devices. 
static void getDevice(DAQ::Device *d, void *p) {
	DAQ::Device **device = reinterpret_cast<DAQ::Device **>(p);

	if(!*device) *device = d;
}

extern "C" Plugin::Object * createRTXIPlugin(void) {
	return new MultiClamp();
};

static DefaultGUIModel::variable_t vars[] = {
	{ "Mode Output", "", DefaultGUIModel::OUTPUT, },
	{ "Acquisition Mode", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "Input Channel", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "Output Channel", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "VClamp Gain", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, }, 
	{ "IClamp Gain", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "VClamp Sensitivity", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "IClamp Sensitivity", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, }, 
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

// constructor. 
MultiClamp::MultiClamp(void) : DefaultGUIModel("Axon MultiClamp 700 Controller", ::vars, ::num_vars) {
	setWhatsThis("<p>Yeah, I'll get to this later... <br>-Ansel</p>");
	DefaultGUIModel::createGUI(vars, num_vars);
	initParameters();
	customizeGUI();
	update( INIT );
	refresh();
	QTimer::singleShot(0, this, SLOT(resizeMe()));
};

MultiClamp::~MultiClamp(void) {};

// initialize parameters. 
void MultiClamp::initParameters(void) {
	input_channel = 0;
	output_channel = 1;

	mode = 3; // NONE;
	vclamp_gain = iclamp_gain = 1; //G1;
	vclamp_sens = 20; 
	iclamp_sens = 400; //V20mV;

	device = 0; 
	DAQ::Manager::getInstance()->foreachDevice(getDevice, &device); // get available devices. does nothing in none found

	iclamp_ai_gain = 1; // 1 V/V
	iclamp_ao_gain = 1.0 / 400e-12; // 400 pA/V
	izero_ai_gain = 1; // 1 V/V
	izero_ao_gain = 0; // No output
	vclamp_ai_gain = 2e-9 / 1.0; // .5 V/nA or 2 nA/V
	vclamp_ao_gain = 1.0 / 20e-3; // 20 mV/V
};

// update called when 1. initialized, 2. modified by Set DAQ, or 3. loaded from a saved workspace
void MultiClamp::update(DefaultGUIModel::update_flags_t flag) {
	switch(flag) {
		case INIT:
			setParameter("Input Channel", input_channel);
			setParameter("Output Channel",output_channel);
			setParameter("VClamp Gain", vclamp_gain);
			setParameter("IClamp Gain", iclamp_gain);
			setParameter("VClamp Sensitivity", vclamp_sens);
			setParameter("IClamp Sensitivity", iclamp_sens);
			setParameter("Acquisition Mode", mode);

			inputBox->setValue(input_channel);
			outputBox->setValue(output_channel);
			ampButtonGroup->button(mode)->setChecked(true);
			ampButtonGroup->button(mode)->setStyleSheet("QRadioButton {font: bold;}");

			vclampGainBox->setCurrentIndex(convertGaintoGUI(vclamp_gain));
			iclampGainBox->setCurrentIndex(convertGaintoGUI(iclamp_gain));
			vclampSensBox->setCurrentIndex(convertVSenstoGUI(vclamp_sens)); //vclamp_sens
			iclampSensBox->setCurrentIndex(convertISenstoGUI(iclamp_sens));
			break;
		
		case MODIFY: //update( MODIFY ) is called by DefaultGUIModel::doLoad, so this is needed.
			input_channel = getParameter("Input Channel").toInt();
			output_channel = getParameter("Output Channel").toInt();
			vclamp_gain = getParameter("VClamp Gain").toDouble();
			iclamp_gain = getParameter("IClamp Gain").toDouble();
			vclamp_ao_gain = getParameter("VClamp Sensitivity").toDouble();  //vclamp_sens
			iclamp_ao_gain = getParameter("IClamp Sensitivity").toDouble(); //iclamp_sens

			// change the bolded amp mode to reflect the selected button when Set DAQ was hit
			ampButtonGroup->button(mode)->setStyleSheet("QRadioButton {font: normal;}");
			mode = getParameter("Acquisition Mode").toInt();
			ampButtonGroup->button(mode)->setStyleSheet("QRadioButton {font: bold;}");
			ampButtonGroup->button(mode)->setChecked(true);

			// blacken the buttons once the values are saved
			inputBox->setValue(input_channel);
			inputBox->blacken();
			outputBox->setValue(output_channel);
			outputBox->blacken();
			vclampGainBox->setCurrentIndex(convertGaintoGUI(vclamp_gain));
			vclampGainBox->blacken();
			iclampGainBox->setCurrentIndex(convertGaintoGUI(iclamp_gain));
			iclampGainBox->blacken();
			vclampSensBox->setCurrentIndex(convertVSenstoGUI(vclamp_sens)); //vclamp_sens
			vclampSensBox->blacken();
			iclampSensBox->setCurrentIndex(convertISenstoGUI(iclamp_sens)); //iclamp_sens
			iclampSensBox->blacken();

			// update the DAQ with the new parameters
			updateDAQ();
			break;

		default:
			break;
	}
}

// updates the DAQ based on the settings set in the GUI
void MultiClamp::updateDAQ(void) {
	if (!device) return;

	switch(mode) {
		case 1: //IClamp
			device->setAnalogGain(DAQ::AI, input_channel, iclamp_ai_gain / iclamp_gain);
			device->setAnalogGain(DAQ::AO, output_channel, iclamp_ao_gain);
			output(0) = 0.0;
			break;

		case 2: //VClamp
			device->setAnalogGain(DAQ::AI, input_channel, vclamp_ai_gain / vclamp_gain);
			device->setAnalogGain(DAQ::AO, output_channel, vclamp_ao_gain);
			output(0) = 5.0;
			break;

		case 3: //I=0
			device->setAnalogGain(DAQ::AI, input_channel, izero_ai_gain / iclamp_gain);
			device->setAnalogGain(DAQ::AO, output_channel, izero_ao_gain);
			output(0) = 0.0;
			break;

		default:
			std::cout<<"Error: default case called in updateDAQ()"<<std::endl;
			break;
	}
}

void MultiClamp::updateInputChannel(int value) {
	parameter["Input Channel"].edit->setText(QString::number(value));
	parameter["Input Channel"].edit->setModified(true);
	return;
}

void MultiClamp::updateOutputChannel(int value) {
	parameter["Output Channel"].edit->setText(QString::number(value));
	parameter["Output Channel"].edit->setModified(true);
	return;
}

void MultiClamp::updateMode(int value) {
	parameter["Acquisition Mode"].edit->setText(QString::number(value));
	parameter["Acquisition Mode"].edit->setModified(true);

//	update(MODIFY);
	modify();
	return;
}

void MultiClamp::updateVClampGain(int value) {
	double temp;

	temp = convertGUItoGain(value);
	vclamp_gain = temp;
	parameter["VClamp Gain"].edit->setText(QString::number(temp));
	parameter["VClamp Gain"].edit->setModified(true);
	return;
}

void MultiClamp::updateIClampGain(int value) {
	double temp;

	temp = convertGUItoGain(value);
	iclamp_gain = temp;
	parameter["IClamp Gain"].edit->setText(QString::number(temp));
	parameter["IClamp Gain"].edit->setModified(true);
	return;
}

void MultiClamp::updateVClampSens(int value) {
//	double temp;

	vclamp_sens = convertGUItoVSens(value);
	vclamp_ao_gain = 1.0 / (vclamp_sens * 1e-3);
	parameter["VClamp Sensitivity"].edit->setText(QString::number(vclamp_sens));
	parameter["VClamp Sensitivity"].edit->setModified(true);
	return;
}

void MultiClamp::updateIClampSens(int value) {
//	double temp;

	iclamp_sens = convertGUItoISens(value);
	iclamp_ao_gain = 1.0 / (iclamp_sens * 1e-12);
	parameter["IClamp Sensitivity"].edit->setText(QString::number(iclamp_sens));
	parameter["IClamp Sensitivity"].edit->setModified(true);
	return;
}

void MultiClamp::customizeGUI(void) {
	QGridLayout *customLayout = DefaultGUIModel::getLayout();
	
	customLayout->itemAtPosition(1,0)->widget()->hide();
	DefaultGUIModel::pauseButton->hide();
	DefaultGUIModel::unloadButton->hide();
	DefaultGUIModel::modifyButton->setText("Set DAQ");

	// Input and Output channels
	QHBoxLayout *ioGroupLayout = new QHBoxLayout;
	inputBox = new MultiClampSpinBox;
	inputBox->setRange(0,100);
	outputBox = new MultiClampSpinBox;
	outputBox->setRange(0,100);
	QLabel *inputBoxLabel = new QLabel;
	QLabel *outputBoxLabel = new QLabel;
	inputBoxLabel->setText("Input");
	outputBoxLabel->setText("Output");
	ioGroupLayout->addWidget(inputBoxLabel);
	ioGroupLayout->addWidget(inputBox);
	ioGroupLayout->addWidget(outputBoxLabel);
	ioGroupLayout->addWidget(outputBox);

	// VClamp settings
	QGroupBox * vclampGroup = new QGroupBox("VClamp Settings");
	vclampGroup->setStyleSheet("QGroupBox { font: bold; }");
	QFormLayout *vclampGroupLayout = new QFormLayout;
	vclampGroup->setLayout(vclampGroupLayout);
	vclampGainBox = new MultiClampComboBox;
	vclampSensBox = new MultiClampComboBox;
	vclampGainBox->insertItem( 0, tr( "1" ) );
	vclampGainBox->insertItem( 1, tr( "2" ) );
	vclampGainBox->insertItem( 2, tr( "5" ) );
	vclampGainBox->insertItem( 3, tr( "10" ) );
	vclampGainBox->insertItem( 4, tr( "20" ) );
	vclampGainBox->insertItem( 5, tr( "50" ) );
	vclampGainBox->insertItem( 6, tr( "100" ) );
	vclampGainBox->insertItem( 7, tr( "200" ) );
	vclampGainBox->insertItem( 8, tr( "500" ) );
	vclampGainBox->insertItem( 9, tr( "1000" ) );
	vclampGainBox->insertItem( 10, tr( "2000" ) );
	vclampSensBox->insertItem( 0, tr( "20 mV / V" ) );
	vclampSensBox->insertItem( 1, tr( "100 mV / V" ) );
	vclampGroupLayout->addRow( tr("Output Gain"), vclampGainBox);
	vclampGroupLayout->addRow( tr("Sensitivity"), vclampSensBox);
	
	// IClamp settings
	QGroupBox * iclampGroup = new QGroupBox("IClamp Settings");
	iclampGroup->setStyleSheet("QGroupBox { font: bold; } ");
	QFormLayout *iclampGroupLayout = new QFormLayout;
	iclampGroup->setLayout(iclampGroupLayout);
	iclampGainBox = new MultiClampComboBox;
	iclampSensBox = new MultiClampComboBox;
	iclampGainBox->insertItem( 0, tr( "1" ) );
	iclampGainBox->insertItem( 1, tr( "2" ) );
	iclampGainBox->insertItem( 2, tr( "5" ) );
	iclampGainBox->insertItem( 3, tr( "10" ) );
	iclampGainBox->insertItem( 4, tr( "20" ) );
	iclampGainBox->insertItem( 5, tr( "50" ) );
	iclampGainBox->insertItem( 6, tr( "100" ) );
	iclampGainBox->insertItem( 7, tr( "200" ) );
	iclampGainBox->insertItem( 8, tr( "500" ) );
	iclampGainBox->insertItem( 9, tr( "1000" ) );
	iclampGainBox->insertItem( 10, tr( "2000" ) );
	iclampSensBox->insertItem( 0, tr( "400 pA / V" ) );
	iclampSensBox->insertItem( 1, tr( "2 nA / V" ) );
	iclampGroupLayout->addRow( tr("Output Gain"), iclampGainBox);
	iclampGroupLayout->addRow( tr("Sensitivity"), iclampSensBox);
	
	// create amp control buttons
	QGroupBox *ampModeGroup = new QGroupBox("Amplifier Mode");
	ampModeGroup->setStyleSheet("QGroupBox { font: bold; } ");
	QHBoxLayout *ampModeGroupLayout = new QHBoxLayout;
	ampModeGroup->setLayout(ampModeGroupLayout);
	iclamp_button = new QRadioButton("IClamp");
	vclamp_button = new QRadioButton("VClamp");
	none_button = new QRadioButton("I = 0");
	iclamp_button->setCheckable(true);
	vclamp_button->setCheckable(true);
	none_button->setCheckable(true);
	ampButtonGroup = new QButtonGroup;
	ampButtonGroup->addButton(iclamp_button, 1);
	ampButtonGroup->addButton(vclamp_button, 2);
	ampButtonGroup->addButton(none_button, 3);
	ampButtonGroup->setExclusive(true);
	ampModeGroupLayout->addWidget(vclamp_button);
	ampModeGroupLayout->addWidget(none_button);
	ampModeGroupLayout->addWidget(iclamp_button);

	// add widgets to layout and set the new layout
	customLayout->addLayout(ioGroupLayout, 0, 0);
	customLayout->addWidget(ampModeGroup, 2, 0);
	customLayout->addWidget(vclampGroup, 3, 0);
	customLayout->addWidget(iclampGroup, 4, 0);
	setLayout(customLayout);

	// Make connections for buttons
	QObject::connect(vclampGainBox, SIGNAL(activated(int)), this, SLOT(updateVClampGain(int)));
	QObject::connect(vclampSensBox, SIGNAL(activated(int)), this, SLOT(updateVClampSens(int)));
	QObject::connect(iclampSensBox, SIGNAL(activated(int)), this, SLOT(updateIClampSens(int)));
	QObject::connect(iclampGainBox, SIGNAL(activated(int)), this, SLOT(updateIClampGain(int)));
	QObject::connect(inputBox, SIGNAL(valueChanged(int)), this, SLOT(updateInputChannel(int)));
	QObject::connect(outputBox, SIGNAL(valueChanged(int)), this, SLOT(updateOutputChannel(int)));
	QObject::connect(ampButtonGroup, SIGNAL(buttonPressed(int)), this, SLOT(updateMode(int)));
}


// converts the index of an option in the GUI to the gain it represents
double MultiClamp::convertGUItoGain(int index) {
	switch(index) {
		case 0:
			return 1;
		case 1:
			return 2;
		case 2:
			return 5;
		case 3:
			return 10;
		case 4:
			return 20;
		case 5:
			return 50;
		case 6:
			return 100;
		case 7:
			return 200;
		case 8:
			return 500;
		case 9:
			return 1000;
		case 10:
			return 2000;
		default:
			return 1;
	};
}

// take the current gain and return the corresponding option in the gui
int MultiClamp::convertGaintoGUI(double gain) {
	int intgain;
	intgain = int(gain);

	switch(intgain) {
		case 1:
			return 0;
		case 2:
			return 1;
		case 5:
			return 2;
		case 10:
			return 3;
		case 20:
			return 4;
		case 50:
			return 5;
		case 100:
			return 6;
		case 200:
			return 7;
		case 500:
			return 8;
		case 1000:
			return 9;
		case 2000:
			return 10;
		default:
			return 0;
	}
};

// converts the index of an option in the GUI to the sensitivity it represents
double MultiClamp::convertGUItoVSens(int index) {
	switch(index) {
		case 0:
			return 20;
		case 1:
			return 100;
		default:
			return 100;
	};
}

// converts the index of an option in the GUI to the sensitivity it represents
double MultiClamp::convertGUItoISens(int index) {
	switch(index) {
		case 0:
			return 400;
		case 1:
			return 2000;
		default:
			return 2000;
	};
}

// take the current sensitivity and return the corresponding option in the gui
int MultiClamp::convertVSenstoGUI(double sens) {
	int intsens;
	intsens = int(sens);

	switch(intsens) {
		case 20:
			return 0;
		case 100:
			return 1;
		default:
			return 0;
	}
}

// take the current sensitivity and return the corresponding option in the gui
int MultiClamp::convertISenstoGUI(double sens) {
	int intsens;
	intsens = int(sens);

	switch(intsens) {
		case 400:
			return 0;
		case 2000:
			return 1;
		default:
			return 0;
	}
}
