/*
 * Copyright (C) 2011 Weill Medical College of Cornell University
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <daq.h>
#include <default_gui_model.h>

class MultiClampComboBox : public QComboBox {
	Q_OBJECT

	public:
		MultiClampComboBox(QWidget * =0);
		~MultiClampComboBox(void);
		void blacken(void);

	public slots:
		void redden(void);
};

class MultiClampSpinBox : public QSpinBox {
	Q_OBJECT

	public:
		MultiClampSpinBox(QWidget * =0);
		~MultiClampSpinBox(void);
		void blacken(void);

	public slots:
		void redden(void);
};

class MultiClamp : public DefaultGUIModel {
	
	Q_OBJECT
	
	public:
		MultiClamp(void);
		virtual ~MultiClamp(void);
	
		void initParameters(void);
		void customizeGUI(void);
		void updateDAQ(void);

	protected:
		virtual void update(DefaultGUIModel::update_flags_t);

	private:
		int mode;
		int input_channel, output_channel;
		int vclamp_gain, iclamp_gain, vclamp_sens, iclamp_sens;

		DAQ::Device *device;

		double iclamp_ai_gain;
		double iclamp_ao_gain;
		double izero_ai_gain;
		double izero_ao_gain;
		double vclamp_ai_gain;
		double vclamp_ao_gain;
	
		QRadioButton *iclamp_button, *vclamp_button, *none_button;
		QButtonGroup *ampButtonGroup;
		MultiClampSpinBox *inputBox, *outputBox;
		MultiClampComboBox *vclampGainBox, *iclampGainBox;
		MultiClampComboBox *vclampSensBox, *iclampSensBox;

		double convertGUItoGain(int);
		double convertGUItoVSens(int);
		double convertGUItoISens(int);
		int convertGaintoGUI(double);
		int convertVSenstoGUI(double);
		int convertISenstoGUI(double);

	private slots:
		void updateInputChannel(int);
		void updateOutputChannel(int);
		void updateMode(int);
		void updateVClampGain(int);
		void updateIClampGain(int);
		void updateVClampSens(int);
		void updateIClampSens(int);
};
