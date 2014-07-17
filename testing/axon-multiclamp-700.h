#include <QtGui>
#include <default_gui_model.h>

class MultiClamp : public DefaultGUIModel {
	
	Q_OBJECT
	
	public:
		MultiClamp(void);
		virtual ~MultiClamp(void);
	
		void initParameters(void);
//		void createGUI(DefaultGUIModel::variable_t *, int);
		void customizeGUI(void);
	
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);

	private:
		enum AmpMode_t {
			ICLAMP = 0,
			VCLAMP = 2,
			NONE = 3
		};
	
		int input_channel, output_channel;
		double vclamp_gain, iclamp_gain;
	
		QPushButton *iclamp_button, *vclamp_button, *none_button;
};
