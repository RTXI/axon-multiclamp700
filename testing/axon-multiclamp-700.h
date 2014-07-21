#include <QtGui>
#include <default_gui_model.h>

class MultiClamp : public DefaultGUIModel {
	
	Q_OBJECT
	
	public:
		MultiClamp(void);
		virtual ~MultiClamp(void);
	
		void initParameters(void);
		void customizeGUI(void);

	
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);

	private:
/*
		enum AmpMode_t {
			ICLAMP = 1,
			VCLAMP = 2,
			NONE = 3
		};
		enum Gain_t {
			G1 = 1,
			G2 = 2,
			G5 = 3,
			G10 = 4,
			G20 = 5,
			G50 = 6,
			G100 = 7,
			G200 = 8,
			G500 = 9,
			G1000 = 10,
			G2000 = 11
		};
		enum Sens_t {
			V20mV = 1,
			V100mV = 2
		};
*/

//		AmpMode_t mode;	
		int mode;
		int input_channel, output_channel;
//		Gain_t vclamp_gain, iclamp_gain;
		int vclamp_gain, iclamp_gain, vclamp_sens, iclamp_sens;
//		Sens_t vclamp_sens, iclamp_sens;
	
		QPushButton *iclamp_button, *vclamp_button, *none_button;
		QButtonGroup *ampButtonGroup;
		QSpinBox *inputBox, *outputBox;
		QComboBox *vclampGainBox, *iclampGainBox;
		QComboBox *vclampSensBox, *iclampSensBox;

//		void updateModeGUI(AmpMode_t);
		void updateModeGUI(int);

	public slots:
//		void updateGUItoParam(void);
		void updateInputChannel(int);
		void updateOutputChannel(int);
		void updateMode(int);
		void updateVClampGain(int);
		void updateIClampGain(int);
		void updateVClampSens(int);
		void updateIClampSens(int);
};
