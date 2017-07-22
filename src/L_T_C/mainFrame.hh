#ifndef L_T_C_MAINFRAME_HH
#define L_T_C_MAINFRAME_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>

#include "lviewermainwindow.hh"

#include <vector>
#include <set>

class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QPushButton;
class QCheckBox;
class QSlider;

class mainFrame : public QMainWindow
{
    Q_OBJECT

public:
    mainFrame(QWidget *parent = 0);
    ~mainFrame();

private slots:
    void s1Change(int);
    void openc1();
    void openc2();
    void newc2();
    void loadc2();
    void loadCellFile();
    void save();
    void saveLabel();
    void saveLink();
    void about();
    void myTest();

    void setJobToLabelling();
    void setJobToFixing();
    void setJobToLinking();
    void merge();

    void undo();
    void increaseST();
    void decreaseST();

    void crop();
    void equal();
    void normRange();
    void diffusion();
    void normContrast();

  void answer(int, int, int, int);
    void modifyLocalLabels(int, int, int, int);
    void wheel(int);

private:
    void createActions();
    void createMenus();
    void createStatusBar();

    bool openFile(std::string, int c);
    void startViewer();
    void backup();

    template<typename T>
    void view(T d, bool update = false);

    template<typename T>
    void viewRef(T d, bool update = false);

    void closeEvent ( QCloseEvent * event );

    void initializeShowLabelling();
    void initializeShowFixing();
    void  initializeShowLinking();
    int trueLabel(int label);


    QString lastOpenFolder;
    bool flagInteraction;

    QTextEdit *textEdit;
    QSlider* s1;
    QSlider* s2;
    QSlider* s3;
    QLabel* v1;
    QLabel* v2;
    QLabel* v3;
    QDockWidget * dock;

    lViewerMainWindow * lv;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *jobMenu;
    QMenu *helpMenu;
    QMenu *preprocessMenu;
    QMenu *segmentationMenu;
    QMenu *analysisMenu;
    QMenu *batchMenu;

    QAction *undoAct;

    QAction *increaseSTAct;
    QAction *decreaseSTAct;

    QAction *openChannel1;
    QAction *openChannel2;
    QAction *newChannel2;
    QAction *loadChannel2;
    QAction *loadcf;
    QAction *saveAct;
    QAction *saveLabelAct;
    QAction *saveLinkAct;
    QAction *quitAct;
    QAction *aboutAct;

    QAction *cropAct;
    QAction *equalAct;
    QAction *normRangeAct;
    QAction *diffusionAct;
    QAction *normContrastAct;

    QAction *labelAct;
    QAction *fixAct;
    QAction *linkAct;
    QAction *mergeAct;

private:

    double c1,c2,t;
    int currentLabel;
    int sliceThick;

    enum JOB{ Labelling, Fixing, Linking};
    int currentJob;

private:

    std::string dataFileName;
    std::string datasetName;
    std::string outFileNameBase;
    std::string outFileName;

    std::string currentDataName;

    //Parameters
    int V;
    int FlagNormalization;

    //diffusion
    int PreDiffusion;
    double kappa, delta_t_df;
    int n_iters_df;

    //normContrast
    double sigmaVar;
    float epsilon;

    //watershed
    double sigma_hessian;
    double thresholdOnL1, compensationOnZ;
    int morphSize, thresholdOnVolume;

    //levelset
    double countour_weight, expansion_weight;
    double lambda, delta_t_ls;
    int n_iters_ls;

    //Data
    atb::Array<float,3> data;
    atb::Array<float,3> data2;

    blitz::Array<unsigned char,4> showData;
    atb::Array<unsigned char,3> lC1;
    atb::Array<unsigned char,3> lC2;

    atb::Array<float,3> backData;
    blitz::Array<float,4> backDataMC;

    int lx,ly,lz, lxb, lyb, lzb;
    int minX,maxX,minY,maxY,minZ,maxZ;

    atb::Array<float,3> image;
    atb::Array<float,3> l1;
    atb::Array<int,3> L;

    blitz::TinyVector<double, 3> elSize;

    std::vector< blitz::TinyVector<unsigned char,3> > colorMap;

    int maxLabel;
    int backgroundLabel;
    blitz::Array<float,1> volumes;
    blitz::Array<blitz::TinyVector<double,3>,1> centers;
    blitz::Array<bool,1> validFlag;
    blitz::Array<blitz::TinyVector<double,3>,1> normCenters;
    blitz::Array<float,2> RD;

    blitz::Array<int,1> classLabels;
    blitz::Array<int,1> classPredictions;

    blitz::Array<int,1> L_reassigned;

    blitz::Array<int,1> parent;
    blitz::Array<int,1> child;
    blitz::Array<int,1> fileNo;
    blitz::Array<int,1> fileNo_reassigned;

    int currentFileNo;
    int currentCell;
    int head;
};



#endif // TESTQT_H
