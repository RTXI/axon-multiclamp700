#include <QtGui>
#include "axon-multiclamp-700.h"
#include <iostream>

extern "C" Plugin::Object * createRTXIPlugin(void) {
	return new MultiClamp();
};

static DefaultGUIModel::variable_t vars[] = {
	{ "Mode Input", "", DefaultGUIModel::INPUT, },
	{ "Mode Output", "", DefaultGUIModel::OUTPUT, },
	{ "Acquisition Mode", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "Input Channel", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "Output Channel", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "VClamp Gain", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, }, 
	{ "IClamp Gain", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "VClamp Sensitivity", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
	{ "IClamp Sensitivity", "", DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, }, 
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

MultiClamp::MultiClamp(void) : DefaultGUIModel("Axon MultiClamp 700 Controller", ::vars, ::num_vars) {
	setWhatsThis("<p>Yeah, I'll get to this later... <br>-Ansel</p>");
	DefaultGUIModel::createGUI(vars, num_vars);
	initParameters();
	customizeGUI();
	update( INIT );
	refresh();
};

MultiClamp::~MultiClamp(void) {};

void MultiClamp::initParameters(void) {
	input_channel = 3;
	output_channel = 5;

	mode = 3; // NONE;
	vclamp_gain = iclamp_gain = 2; //G1;
	vclamp_sens = iclamp_sens = 2; //V20mV;
};

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
//			updateModeGUI(mode);
			ampButtonGroup->button(mode)->setChecked(true);
			vclampGainBox->setCurrentIndex(vclamp_gain);
			iclampGainBox->setCurrentIndex(iclamp_gain);
			vclampSensBox->setCurrentIndex(vclamp_sens);
			vclampSensBox->setCurrentIndex(iclamp_sens);
			break;
		
		case MODIFY: //update( MODIFY ) is called by DefaultGUIModel::doLoad, so this is needed.
			input_channel = getParameter("Input Channel").toInt();
			output_channel = getParameter("Output Channel").toInt();
			vclamp_gain = getParameter("VClamp Gain").toInt();
			iclamp_gain = getParameter("IClamp Gain").toInt();
			vclamp_sens = getParameter("VClamp Sensitivity").toInt();
			iclamp_sens = getParameter("IClamp Sensitivity").toInt();
			mode = getParameter("Acquisition Mode").toInt();
			inputBox->setValue(input_channel);
			outputBox->setValue(output_channel);
//			updateModeGUI(mode);
			ampButtonGroup->button(mode)->setChecked(true);
			vclampGainBox->setCurrentIndex(vclamp_gain);
			iclampGainBox->setCurrentIndex(iclamp_gain);
			vclampSensBox->setCurrentIndex(vclamp_sens);
			vclampSensBox->setCurrentIndex(iclamp_sens);	
			break;

		default:
			break;
	}
}

//void MultiClamp::updateModeGUI(AmpMode_t mode) {
/*void MultiClamp::updateModeGUI(int mode) {
	switch(mode) {
		case 1: //ICLAMP:
			iclamp_button->setChecked(true);
			break;
	
		case 2: //VCLAMP:
			vclamp_button->setChecked(true);
			break;
	
		case 3: //NONE:
			none_button->setChecked(true);
			break;

		default:
			break;
	}
	return;
}*/

/*
void MultiClamp::updateGUItoParam(void) {
	input_channel = inputBox->value();
	output_channel = outputBox->value();
	vclamp_gain = vclampGainBox->currentIndex();
	iclamp_gain = iclampGainBox->currentIndex();
	vclamp_sens = vclampSensBox->currentIndex();
	iclamp_sens = iclampSensBox->currentIndex();
	mode = ampButtonGroup->checkedId();

	setParameter("Input Channel", input_channel);
	setParameter("Output Channel", output_channel);
	setParameter("VClamp Gain", vclamp_gain);
	setParameter("IClamp Gain", iclamp_gain);
	setParameter("VClamp Sensitivity", vclamp_sens);
	setParameter("IClamp Sensitivity", iclamp_sens);
	setParameter("Acquisition Mode", mode);
}
*/

void MultiClamp::updateInputChannel(int value) {
	input_channel = value;
	setParameter("Input Channel", input_channel);
	return;
}

void MultiClamp::updateOutputChannel(int value) {
	output_channel = value;
	setParameter("Output Channel", output_channel);
	return;
}

void MultiClamp::updateMode(int value) {
	mode = value;
	setParameter("Acquisition Mode", mode);
	return;
}

void MultiClamp::updateVClampGain(int value) {
	vclamp_gain = value;
	setParameter("VClamp Gain", vclamp_gain);
	return;
}

void MultiClamp::updateIClampGain(int value) {
	iclamp_gain = value;
	setParameter("IClamp Gain", iclamp_gain);
	return;
}

void MultiClamp::updateVClampSens(int value) {
	vclamp_sens = value;
	setParameter("VClamp Sensitivity", vclamp_sens);
	return;
}

void MultiClamp::updateIClampSens(int value) {
	iclamp_sens = value;
	setParameter("IClamp Sens", iclamp_sens);
	return;
}

void MultiClamp::customizeGUI(void) {
	QGridLayout *customLayout = DefaultGUIModel::getLayout();
	
//	customLayout->itemAtPosition(1,0)->widget()->setVisible(false);
	customLayout->itemAtPosition(10,0)->widget()->setVisible(false);

	// Input and Output channels
	QGroupBox *ioGroup = new QGroupBox("DAQ Channels");
	ioGroup->setStyleSheet("QGroupBox { font: bold; } ");
	QHBoxLayout *ioGroupLayout = new QHBoxLayout;
	inputBox = new QSpinBox;
	outputBox = new QSpinBox;
	QLabel *inputBoxLabel = new QLabel;
	QLabel *outputBoxLabel = new QLabel;
	inputBoxLabel->setText("Input");
	outputBoxLabel->setText("Output");
	ioGroupLayout->addWidget(inputBoxLabel);//, Qt::AlignLeft);
	ioGroupLayout->addWidget(inputBox);//, Qt::AlignRight);
	ioGroupLayout->addWidget(outputBoxLabel);//, Qt::AlignLeft);
	ioGroupLayout->addWidget(outputBox);//, Qt::AlignRight);
	ioGroupLayout->insertSpacing(2, 30);
	ioGroup->setLayout(ioGroupLayout);

	// VClamp settings
	QGroupBox * vclampGroup = new QGroupBox("VClamp Settings");
	vclampGroup->setStyleSheet("QGroupBox { font: bold; }");
	QFormLayout *vclampGroupLayout = new QFormLayout;
	vclampGroup->setLayout(vclampGroupLayout);
	vclampGainBox = new QComboBox;
	vclampSensBox = new QComboBox;
	vclampGainBox->insertItem( 1, tr( "1" ) );
	vclampGainBox->insertItem( 2, tr( "2" ) );
	vclampGainBox->insertItem( 3, tr( "5" ) );
	vclampGainBox->insertItem( 4, tr( "10" ) );
	vclampGainBox->insertItem( 5, tr( "20" ) );
	vclampGainBox->insertItem( 6, tr( "50" ) );
	vclampGainBox->insertItem( 7, tr( "100" ) );
	vclampGainBox->insertItem( 8, tr( "200" ) );
	vclampGainBox->insertItem( 9, tr( "500" ) );
	vclampGainBox->insertItem( 10, tr( "1000" ) );
	vclampGainBox->insertItem( 11, tr( "2000" ) );
	vclampSensBox->insertItem( 1, tr( "20 mV / V" ) );
	vclampSensBox->insertItem( 2, tr( "100 mV / V" ) );
	vclampGroupLayout->addRow( tr("Output Gain"), vclampGainBox);
	vclampGroupLayout->addRow( tr("Sensitivity"), vclampSensBox);
	
	// IClamp settings
	QGroupBox * iclampGroup = new QGroupBox("IClamp Settings");
	iclampGroup->setStyleSheet("QGroupBox { font: bold; } ");
	QFormLayout *iclampGroupLayout = new QFormLayout;
	iclampGroup->setLayout(iclampGroupLayout);
	iclampGainBox = new QComboBox;
	iclampSensBox = new QComboBox;
	iclampGainBox->insertItem( 1, tr( "1" ) );
	iclampGainBox->insertItem( 2, tr( "2" ) );
	iclampGainBox->insertItem( 3, tr( "5" ) );
	iclampGainBox->insertItem( 4, tr( "10" ) );
	iclampGainBox->insertItem( 5, tr( "20" ) );
	iclampGainBox->insertItem( 6, tr( "50" ) );
	iclampGainBox->insertItem( 7, tr( "100" ) );
	iclampGainBox->insertItem( 8, tr( "200" ) );
	iclampGainBox->insertItem( 9, tr( "500" ) );
	iclampGainBox->insertItem( 10, tr( "1000" ) );
	iclampGainBox->insertItem( 11, tr( "2000" ) );
	iclampSensBox->insertItem( 1, tr( "20 mV / V" ) );
	iclampSensBox->insertItem( 2, tr( "100 mV / V" ) );
	iclampGroupLayout->addRow( tr("Output Gain"), iclampGainBox);
	iclampGroupLayout->addRow( tr("Sensitivity"), iclampSensBox);
	
	// create amp control buttons
	QGroupBox *ampModeGroup = new QGroupBox("Amplifier Mode");
	ampModeGroup->setStyleSheet("QGroupBox { font: bold; } ");
	QHBoxLayout *ampModeGroupLayout = new QHBoxLayout;
	ampModeGroup->setLayout(ampModeGroupLayout);
	
	iclamp_button = new QPushButton("IClamp");
	vclamp_button = new QPushButton("VClamp");
	none_button = new QPushButton("I=0");
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
	customLayout->addWidget(ioGroup, 0, 0);
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
//	QObject::connect(vclamp_button, SIGNAL(toggled(bool)), this, SLOT(updateGUItoParam(void)));
//	QObject::connect(iclamp_button, SIGNAL(toggled(bool)), this, SLOT(updateGUItoParam(void)));
//	QObject::connect(none_button, SIGNAL(toggled(bool)), this, SLOT(updateGUItoParam(void)));
}
