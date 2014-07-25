#include <QtGui>
#include <daq.h>
#include <default_gui_model.h>

class MultiClampComboBox : public QComboBox {
	Q_OBJECT

	public:
		MultiClampComboBox(QWidget * =0);
		~MultiClampComboBox(void);
		void blacken(void);
		QPalette palette;

	public slots:
		void redden(void);
};

class MultiClampSpinBox : public QSpinBox {
	Q_OBJECT

	public:
		MultiClampSpinBox(QWidget * =0);
		~MultiClampSpinBox(void);
		void blacken(void);
		QPalette palette;

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
		double convertGUItoSens(int);
		int convertGaintoGUI(double);
		int convertSenstoGUI(double);

	private slots:
		void updateInputChannel(int);
		void updateOutputChannel(int);
		void updateMode(int);
		void updateVClampGain(int);
		void updateIClampGain(int);
		void updateVClampSens(int);
		void updateIClampSens(int);
};
