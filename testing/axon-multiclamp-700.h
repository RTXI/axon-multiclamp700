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
		int mode;
		int input_channel, output_channel;
		int vclamp_gain, iclamp_gain, vclamp_sens, iclamp_sens;
	
		QPushButton *iclamp_button, *vclamp_button, *none_button;
		QButtonGroup *ampButtonGroup;
		QSpinBox *inputBox, *outputBox;
		QComboBox *vclampGainBox, *iclampGainBox;
		QComboBox *vclampSensBox, *iclampSensBox;

	private slots:
		void updateInputChannel(int);
		void updateOutputChannel(int);
		void updateMode(int);
		void updateVClampGain(int);
		void updateIClampGain(int);
		void updateVClampSens(int);
		void updateIClampSens(int);
};
