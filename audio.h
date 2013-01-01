enum EAMBIENT_MUSIC_TYPE;
class AudioEngine
{
private:
	bool status;
	//put some more variables to use, maybe after change it to an interface
public:
	AudioEngine( bool start ) : status(start){}
	//virtual bool isActive();
	//virtual bool stop();//or drop()?
	//virtual bool setSpeed(); // bool because it can be used in if statement to provide information about the success of the operation or failure
	//virtual double getSpeed();
	//virtual void setAmbientMusic(enum EAMBIENT_MUSIC_TYPE);
	//virtual bool isMultithreaded();
};
enum EAMBIENT_MUSIC_TYPE
{
	E_AMT_COUNTRY = 0,
	E_AMT_FIGHT,
	E_AMT_CUT_SCENE,
	E_AMT_TEST,
	E_AMT_COUNT // do not use! Exists only for the purpose information
};