#include "mainFrame.hh"

#include <QtGui/QMessageBox>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QStatusBar>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QCloseEvent>
#include <QtCore/QSettings>

#include <cmath>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <libArrayToolbox/GaussianFilter.hh>
#include <libArrayToolbox/Normalization.hh>

#include <libArrayToolbox/algo/ldiffusion.hh>
#include <libArrayToolbox/algo/ltransform.hh>
#include <libArrayToolbox/algo/lmath.hh>
#include <libArrayToolbox/algo/helper.hh>
#include <libArrayToolbox/algo/lmorph.hh>
#include <libArrayToolbox/algo/lParallel.hh>

#include "SelectDlg.hh"

std::string globalOutFileName;

using std::vector;
using std::string;
using blitz::Array;
using blitz::TinyVector;
using blitz::Range;

#define _range 1000.0

mainFrame::mainFrame(QWidget *parent) :
        QMainWindow(parent)
{
  lv = new lViewerMainWindow();

  lv->setFocusPolicy(Qt::WheelFocus);
  elSize = 1;

  setCentralWidget(lv);
  lv->hide();

  createActions();
  createMenus();
  createStatusBar();

  setWindowTitle(tr("LTC Tool"));
  //
  QSettings settings("LMB", "LTC");
  restoreGeometry(settings.value("geometry").toByteArray());

  lastOpenFolder = settings.value("lastOpenFolder").toString();

  if (lastOpenFolder.length() == 0)
      lastOpenFolder = QDir::home().currentPath();

  currentLabel = 0;
  sliceThick = 1;
}

void
mainFrame::s1Change(int v)
{
  c1 = v;
  v1->setText(QString("radius = %1\t").arg(c1));
}

mainFrame::~mainFrame()
{}

void
mainFrame::closeEvent(QCloseEvent * event)
{
  QSettings settings("LMB", "LTC");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("lastOpenFolder", lastOpenFolder);
  event->accept();
}

void
mainFrame::myTest()
{}

void
mainFrame::about()
{
  QMessageBox::about(
      this,
      tr("Instruction:"),
      tr("Open_Data : open the data file and pick a channel as reference "
         "image.\n"
         "Open_Label_Feature : open the analysis file (corresponding labels "
         "will be loaded).\n\n"

         "double left click -> move to\n"
         "wheel -> navigate in depth\n"
         "ctrl+wheel -> zoom\n\n"

         "Labelling:\n"
         "Load Label: read \"labels\" or \"predicted_labels\".\n"
         "New Label: clear the current labels\n"
         "ctrl+right click -> pick the cell's label\n"
         "ctrl+left click -> assign the current label\n"
         "shift+wheel -> shift the current label\n"
         "Save Class Label : save \"labels\" or \"predicted_labels\" "
         "to hdf5 file.\n\n"

         "Fixing (over) Segmentation:\n"
         "ctrl+right click -> pick the cell's segment-label\n"
         "ctrl+left click -> assign the current segment-label\n"
         "Job -- Merge : merge the fixed over-segmentation, require the "
         "dataset '\\hessian\\l1' \n"
         "Save Fixed Segmentation : save the manual fixed segmentation into "
         "hdf5 file, then call \"merge\" program to finish the job.\n\n"

         "Fixing the link of cell files:\n"
         "Load Cell-File Info : load the reliable linked cell strings from "
         "program \"LinkCells\".\n"
         "shift+wheel -> shift the current fileNo\n"
         "shift+right click -> jump the ends of the current fileNo (click "
         "again for going to another end)\n"
         "shift+left click -> break the connection of the pointed cell to "
         "its parent, hence create a new cell string\n"
         "ctrl+right click -> pick the cell's fileNo\n"
         "ctrl+left click -> assign the current fileNo\n"
         "Save Fixed Link : save the manual fixed link into hdf5 file.\n\n"

         "Developer: Kun Liu \n"
         "Image Analysis Lab\nUniv. Freiburg"));
}

void
mainFrame::createActions()
{
  quitAct = new QAction(tr("&Quit"), this);
  quitAct->setShortcut(tr("Ctrl+Q"));
  quitAct->setStatusTip(tr("Quit the application"));
  connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

  openChannel1 = new QAction(tr("&Open_Data"), this);
  openChannel1->setShortcut(tr("Ctrl+O"));
  openChannel1->setStatusTip(tr("Open Data Channel"));
  connect(openChannel1, SIGNAL(triggered()), this, SLOT(openc1()));

  openChannel2 = new QAction(tr("Open_Label_Feature"), this);
  openChannel2->setStatusTip(tr("Open Label Channel"));
  connect(openChannel2, SIGNAL(triggered()), this, SLOT(openc2()));

  loadChannel2 = new QAction(tr("Load_Label"), this);
  loadChannel2->setStatusTip(tr("Load Label Channel"));
  connect(loadChannel2, SIGNAL(triggered()), this, SLOT(loadc2()));

  loadcf = new QAction(tr("Load Cell-File Info"), this);
  loadcf->setStatusTip(tr("Load Cell File Info"));
  connect(loadcf, SIGNAL(triggered()), this, SLOT(loadCellFile()));

  newChannel2 = new QAction(tr("New_Label"), this);
  newChannel2->setStatusTip(tr("Initialize Label Channel"));
  connect(newChannel2, SIGNAL(triggered()), this, SLOT(newc2()));

  saveAct = new QAction(tr("&Save Class Label"), this);
  saveAct->setShortcut(tr("Ctrl+S"));
  saveAct->setStatusTip(tr("Save the class labels"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveLabelAct = new QAction(tr("&Save Fixed Segmentation"), this);
  saveLabelAct->setStatusTip(tr("Save the fixed segmentation"));
  connect(saveLabelAct, SIGNAL(triggered()), this, SLOT(saveLabel()));

  saveLinkAct = new QAction(tr("&Save Fixed Link"), this);
  saveLinkAct->setStatusTip(tr("Save the fixed Link"));
  connect(saveLinkAct, SIGNAL(triggered()), this, SLOT(saveLink()));

  aboutAct = new QAction(tr("&About"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  undoAct = new QAction(tr("&Undo"), this);
  connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

  cropAct = new QAction(tr("&Crop"), this);
  connect(cropAct, SIGNAL(triggered()), this, SLOT(crop()));

  equalAct = new QAction(tr("&Equalize Element Size"), this);
  connect(equalAct, SIGNAL(triggered()), this, SLOT(equal()));

  normRangeAct = new QAction(tr("&Normalize Range"), this);
  connect(normRangeAct, SIGNAL(triggered()), this, SLOT(normRange()));

  diffusionAct = new QAction(tr("&Diffusion"), this);
  connect(diffusionAct, SIGNAL(triggered()), this, SLOT(diffusion()));

  normContrastAct = new QAction(tr("&Normalize Contrast"), this);
  connect(normContrastAct, SIGNAL(triggered()), this, SLOT(normContrast()));

  increaseSTAct = new QAction(tr("Increase Slice Thickness"), this);
  increaseSTAct->setEnabled(false);
  increaseSTAct->setShortcut(tr("Ctrl+]"));
  connect(increaseSTAct, SIGNAL(triggered()), this, SLOT(increaseST()));

  decreaseSTAct = new QAction(tr("Decrease Slice Thickness"), this);
  decreaseSTAct->setEnabled(false);
  decreaseSTAct->setShortcut(tr("Ctrl+["));
  connect(decreaseSTAct, SIGNAL(triggered()), this, SLOT(decreaseST()));

  labelAct = new QAction(tr("Label cells"), this);
  labelAct->setEnabled(false);
  labelAct->setCheckable(true);
  connect(labelAct, SIGNAL(triggered()), this, SLOT(setJobToLabelling()));

  fixAct = new QAction(tr("Fix the segmentation"), this);
  fixAct->setEnabled(false);
  fixAct->setCheckable(true);
  connect(fixAct, SIGNAL(triggered()), this, SLOT(setJobToFixing()));

  linkAct = new QAction(tr("Link/Split the cell files"), this);
  linkAct->setEnabled(false);
  linkAct->setCheckable(true);
  connect(linkAct, SIGNAL(triggered()), this, SLOT(setJobToLinking()));

  mergeAct = new QAction(tr("Merge"), this);
  mergeAct->setEnabled(false);
  connect(mergeAct, SIGNAL(triggered()), this, SLOT(merge()));
}

void
mainFrame::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openChannel1);
  fileMenu->addAction(openChannel2);
  fileMenu->addAction(loadChannel2);
  fileMenu->addAction(loadcf);
  fileMenu->addAction(newChannel2);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveLabelAct);
  fileMenu->addAction(saveLinkAct);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAct);

  jobMenu = menuBar()->addMenu(tr("&Job"));
  jobMenu->addAction(labelAct);
  jobMenu->addAction(fixAct);
  jobMenu->addAction(linkAct);
  jobMenu->addSeparator();
  jobMenu->addAction(mergeAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(cropAct);
  editMenu->addAction(undoAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(increaseSTAct);
  viewMenu->addAction(decreaseSTAct);

  preprocessMenu = menuBar()->addMenu(tr("&Preprocess"));
  preprocessMenu->addAction(equalAct);
  preprocessMenu->addAction(normRangeAct);
  preprocessMenu->addAction(diffusionAct);
  preprocessMenu->addSeparator();
  preprocessMenu->addAction(normContrastAct);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void
mainFrame::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}

void
mainFrame::openc1()
{
  QString filename;

  filename = QFileDialog::getOpenFileName(
      this, "Choose a file to show", lastOpenFolder,
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty()) return;
  lastOpenFolder = filename;
  openFile(filename.toStdString(), 1);
  increaseSTAct->setEnabled(true);
}

int
mainFrame::trueLabel(int label)
{
  return (L_reassigned(label) >= 0) ? L_reassigned(label) : label;
}

void
mainFrame::openc2()
{
  blitz::Range all = blitz::Range::all();
  QString filename;

  filename = QFileDialog::getOpenFileName(
      this, "Choose a file to show", lastOpenFolder,
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty())
      return;
  lastOpenFolder = filename;
  std::string fileName = filename.toStdString();

  dataFileName = fileName;
  datasetName = "L2";
  LOG << "Loading '" << fileName << ":" << datasetName << "'" << std::endl;
  L.load(fileName, datasetName);
  elSize = L.elementSizeUm();
  LOG << "Image size is " << L.extent() << std::endl;
  LOG << "The index order is Z Y X !!" << std::endl;
  LOG << "Element size is " << elSize << std::endl;
  assert(lx = L.extent(2));
  assert(ly = L.extent(1));
  assert(lz = L.extent(0));

  assert(lx = L.extent(2));
  assert(ly = L.extent(1));
  assert(lz = L.extent(0));

  //read in features for each cells
  try
  {
    BlitzH5File inFile(dataFileName);
    inFile.readDataset(validFlag, "/validFlag");
    std::cout << "  Loaded " << validFlag.size() << " valid flags" << std::endl;
    inFile.readDataset(centers, "/centers");
    std::cout << "  Loaded " << centers.size() << " centers" << std::endl;
    inFile.readDataset(RD, "/RD");
    std::cout << "  Loaded " << RD.extent(0) << " feature vectors of length "
              << RD.extent(1) << std::endl;
    inFile.readAttribute(backgroundLabel, "backgroundLabel", "/");
    std::cout << "  background label = " << backgroundLabel << std::endl;
    inFile.readAttribute(maxLabel, "maxLabel", "/");
    std::cout << "  max label = " << maxLabel << std::endl;
  }
  catch (BlitzH5Error& e)
  {
    LOG << "Something went wrong while processing '" << dataFileName << "': "
        << e.str() << std::endl;
    return;
  }

  int maxData = blitz::max(L);
  if (maxData > maxLabel)
  {
    std::stringstream msg;
    msg << "ERROR: Invalid Label dataset! The maximum value of "
        << datasetName << " = " << maxData << " > " << maxLabel
        << " = maxLabel";
    throw std::runtime_error(msg.str().c_str());
  }

  L_reassigned.resize(maxLabel + 1);
  L_reassigned = -1;

  try
  {
    BlitzH5File inFile(dataFileName);
    inFile.readDataset(classLabels, "/labels");
    classLabels = where(validFlag, classLabels, -1);
  }
  catch (BlitzH5Error& e)
  {
    LOG << "no labels" << e.str() << std::endl;
    classLabels.resize(validFlag.size());
    classLabels = where(validFlag, 0, -1);
  }

  try
  {
    BlitzH5File inFile(dataFileName);
    inFile.readDataset(classPredictions, "/predicted_labels");
    classPredictions = where(validFlag, classPredictions, -1);
  }
  catch (BlitzH5Error& e)
  {
    LOG << "no predicted labels" << e.str() << std::endl;
    classPredictions.resize(validFlag.size());
    classPredictions = 0;
  }

  showData.resize(data.extent(0), data.extent(1), data.extent(2), 3);
  lC1.reference(showData(all, all, all, 1));
  lC2.reference(showData(all, all, all, 2));
  blitz::Array<unsigned char, 3 + 1>::iterator it = showData.begin();
  blitz::Array<int, 3>::iterator it2 = L.begin();
  blitz::Array<float, 3>::iterator it3 = data.begin();

  labelAct->setEnabled(true);
  fixAct->setEnabled(true);
  mergeAct->setEnabled(true);
  colorMap = getColorMap(maxLabel);

  setJobToLabelling();
  initializeShowLabelling();
  // connect signal

  connect(lv, SIGNAL(query(int, int , int, int)), this,
          SLOT(answer(int, int, int, int)));

  connect(lv, SIGNAL(modify(int, int, int, int)), this,
          SLOT(modifyLocalLabels(int, int, int, int)));

  connect(lv, SIGNAL(wheel(int)), this, SLOT(wheel(int)));
}

void
mainFrame::newc2()
{
  classLabels.resize(validFlag.size());
  classLabels = where(validFlag, 0, -1);

  classPredictions.resize(validFlag.size());
  classPredictions = 0;

  if (currentJob == Labelling)
      initializeShowLabelling();
  else
      setJobToLabelling();
}

void
mainFrame::loadc2()
{
  QString filename;

  filename = QFileDialog::getOpenFileName(
      this, "Choose file to show", lastOpenFolder,
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty())
      return;
  lastOpenFolder = filename;
  std::string fileName = filename.toStdString();

  try
  {
    BlitzH5File inFile(fileName);
    inFile.readDataset(classLabels, "/labels");
    classLabels = where(validFlag, classLabels, -1);
  }
  catch (BlitzH5Error& e)
  {
    LOG << "no labels" << e.str() << std::endl;
  }

  try
  {
    BlitzH5File inFile(fileName);
    inFile.readDataset(classPredictions, "/predicted_labels");
    classPredictions = where(validFlag, classPredictions, -1);
  }
  catch (BlitzH5Error& e)
  {
    LOG << "no predicted_labels" << e.str() << std::endl;
  }

  if (currentJob == Labelling)
      initializeShowLabelling();
  else
      setJobToLabelling();
}

void
mainFrame::loadCellFile()
{
  QString filename;

  filename = QFileDialog::getOpenFileName(
      this, "Choose file to show", lastOpenFolder,
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty())
      return;
  lastOpenFolder = filename;
  std::string fileName = filename.toStdString();

  try
  {
    BlitzH5File inFile(fileName);
    inFile.readDataset(fileNo, "/fileNo");
    inFile.readDataset(parent, "/parent");
    inFile.readDataset(child, "/child");
  }
  catch (BlitzH5Error& e)
  {
    LOG << "no fileNo or (parent, child)" << e.str() << std::endl;
  }

  linkAct->setEnabled(true);

  if (currentJob == Linking)
      initializeShowLinking();
  else
      setJobToLinking();

  currentFileNo = 0;
  currentCell = 0;
  head = 0;
}

void
mainFrame::save()
{
  QString filename;

  filename = QFileDialog::getSaveFileName(
      this, "Choose file name to save", lastOpenFolder + ".classLabels",
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty())
      return;
  lastOpenFolder = filename;

  try
  {
    BlitzH5File outFile(filename.toStdString(), BlitzH5File::WriteOrNew);
    outFile.writeDataset(classLabels, "/labels", 3);
    outFile.writeDataset(classPredictions, "/predicted_labels", 3);
  }
  catch (BlitzH5Error& e)
  {
    std::stringstream s;
    LOG << "Something went wrong while processing '" << outFileName << "': "
        << e.str() << std::endl;
  }
}

void
mainFrame::saveLabel()
{
  QString filename;

  filename = QFileDialog::getSaveFileName(
      this, "Choose file name to save", lastOpenFolder + ".fixedL",
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty())
      return;
  lastOpenFolder = filename;

  datasetName = "L";

  try
  {
    BlitzH5File outFile(filename.toStdString(), BlitzH5File::WriteOrNew);
    outFile.writeDataset(L, "/L", 3);
    outFile.writeAttribute(elSize, "element_size_um", "/L");
    outFile.writeDataset(L_reassigned, "/L_reassigned", 3);
    outFile.writeDataset(classLabels, "/labels", 3);
    outFile.writeDataset(classPredictions, "/predicted_labels", 3);
  }
  catch (BlitzH5Error& e)
  {
    std::stringstream s;
    LOG << "Something went wrong while processing '" << outFileName << "': "
        << e.str() << std::endl;
  }
}

void
mainFrame::saveLink()
{
  QString filename;

  filename = QFileDialog::getSaveFileName(
      this, "Choose file name to save", lastOpenFolder + ".fixedLink",
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty()) return;
  lastOpenFolder = filename;

  try
  {
    BlitzH5File outFile(filename.toStdString(), BlitzH5File::WriteOrNew);
    outFile.writeDataset(parent, "/parent", 3);
    outFile.writeDataset(child, "/child", 3);
    outFile.writeDataset(fileNo, "/fileNo", 3);
  }
  catch (BlitzH5Error& e)
  {
    std::stringstream s;
    LOG << "Something went wrong while processing '" << outFileName << "': "
        << e.str() << std::endl;
  }
}

bool
mainFrame::openFile(std::string fileName, int c)
{
  try
  {
    std::map<std::string, bool> varMap;

    std::vector<std::string> vars;

    try
    {
      BlitzH5File inFile(fileName);
      vars = inFile.allDatasets();
    }
    catch (BlitzH5Error &)
    {}

    std::vector<std::string>::const_iterator iter2 = vars.begin();
    for (; iter2 != vars.end(); ++iter2)
    {
      varMap[*iter2] = true;
    }

    SelectDlg slct(this, varMap, "Variables");
    slct.setSizeGripEnabled(true);

    if (slct.exec() == QDialog::Rejected)
        return false;

    bool varChosen = false;
    std::map<std::string, bool>::iterator iter = varMap.begin();
    for (; iter != varMap.end(); ++iter)
    {
      currentDataName = "File: " + fileName + " Variable: " + iter->first;
      dataFileName = fileName;
      outFileNameBase = "_" + fileName;
      datasetName = iter->first;
      if (iter->second)
      {
        try
        {
          if (c == 1)
          {
            LOG << "Loading '" << fileName << ":" << datasetName << "'"
                << std::endl;
            data.load(dataFileName, datasetName);
            elSize = data.elementSizeUm();
            LOG << "Image size is " << data.extent() << std::endl;
            LOG << "The index order is Z Y X !!" << std::endl;
            LOG << "Element size is " << elSize << std::endl;
            varChosen = true;
            view(data);
            viewRef(data);
            lx = data.extent(2);
            ly = data.extent(1);
            lz = data.extent(0);
            break;
          }
          else if (c == 2)
          {
            LOG << "Loading '" << fileName << ":" << datasetName << "'"
                << std::endl;
            l1.load(dataFileName, datasetName);
            LOG << "l1 size is " << l1.extent() << std::endl;
            varChosen = true;
            break;
          }
        }
        catch (BlitzH5Error &)
        {}
      }
    }
    if (!varChosen)
        return false;
  }
  catch (BlitzH5Error& e)
  {
    std::string message = "Open of file " + fileName + " failed.\n" + e.str();
    QMessageBox::warning(this, "Open...", message.c_str(),
                         QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }
  return true;
}

void
mainFrame::modifyLocalLabels(int x, int y, int z, int flag)
{
  if (currentJob == Labelling)
  {
    int label = trueLabel(L(z, y, x));
    if (flag == 5)
    {
      currentLabel = classLabels(label - 1);
    }
    else if (flag == 2 && label > 0 && label != backgroundLabel &&
             validFlag(label - 1))
    {
      classLabels(label - 1) = currentLabel;
      //prepare update image data
      blitz::TinyVector<double, 3> cc(centers(label - 1) / elSize + 0.5);
      float r = blitz::max(RD(label - 1, blitz::Range::all())) /
          blitz::min(elSize) * 1.5;
      blitz::TinyVector<int, 3> lb(cc(0) - r, cc(1) - r, cc(2) - r);
      blitz::TinyVector<int, 3> ub(cc(0) + r, cc(1) + r, cc(2) + r);
      keepHigherThan3D(lb, blitz::TinyVector<int, 3>(0, 0, 0));
      keepLowerThan3D(ub, L.shape() - 1);
      blitz::RectDomain<3> domain(lb, ub);

      blitz::Array<bool,3> mask(
          blitz::TinyVector<atb::BlitzIndexT,3>(ub - lb + 1));
      blitz::TinyVector<unsigned char, 3> color(
          colorMap[classLabels(label - 1) + 3](0),
          colorMap[classLabels(label - 1) + 3](1),
          colorMap[classLabels(label - 1) + 3](2));
      mask = (L(domain) == label);
      lv->updateData(mask, color, domain);
    }
    QString s = tr(
        "Label : %1   fixedLabel : %2   classLabel : %3   "
        "classPredictions: %4   currentLabel : %5") .arg(
            L(z, y, x)).arg(label).arg(classLabels(label - 1)).arg(
                classPredictions(label - 1)).arg(currentLabel);
    statusBar()->showMessage(s);
  }
  else if (currentJob == Fixing)
  {
    int label = L(z, y, x);
    int rlabel = trueLabel(L(z, y, x));
    if (flag == 5)
    {
      currentLabel = rlabel;
    }
    else if (label != currentLabel && flag == 2 && label > 0 &&
             label != backgroundLabel && validFlag(label - 1))
    {
      if (L_reassigned(currentLabel) == -1)
      {
        L_reassigned(currentLabel) = currentLabel;
        //prepare update image data
        blitz::TinyVector<double, 3> cc(
            centers(currentLabel - 1) / elSize + 0.5);
        float r = blitz::max(RD(currentLabel - 1, blitz::Range::all())) /
            blitz::min(elSize) * 1.5;
        blitz::TinyVector<int, 3> lb(cc(0) - r, cc(1) - r, cc(2) - r);
        blitz::TinyVector<int, 3> ub(cc(0) + r, cc(1) + r, cc(2) + r);
        keepHigherThan3D(lb, blitz::TinyVector<int, 3>(0, 0, 0));
        keepLowerThan3D(ub, L.shape() - 1);
        blitz::RectDomain < 3 > domain(lb, ub);

        blitz::Array<bool,3> mask(
            blitz::TinyVector<atb::BlitzIndexT,3>(ub - lb + 1));
        blitz::TinyVector<unsigned char, 3> color(
            colorMap[(currentLabel)](0), colorMap[(currentLabel)](1),
            colorMap[(currentLabel)](2));
        mask = (L(domain) == currentLabel);
        std::cerr << "domain:" << lb << ub << std::endl;
        std::cerr << "mask size:" << mask.shape() << "domain size:"
                  << L(domain).shape() << std::endl;
        std::cerr << "color:" << color << std::endl;
        lv->updateData(mask, color, domain);
      }
      L_reassigned(label) = currentLabel;
      //prepare update image data
      blitz::TinyVector<double, 3> cc(centers(label - 1) / elSize + 0.5);
      float r = blitz::max(RD(label - 1, blitz::Range::all())) /
          blitz::min(elSize) * 1.5;
      blitz::TinyVector<int, 3> lb(cc(0) - r, cc(1) - r, cc(2) - r);
      blitz::TinyVector<int, 3> ub(cc(0) + r, cc(1) + r, cc(2) + r);
      keepHigherThan3D(lb, blitz::TinyVector<int, 3>(0, 0, 0));
      keepLowerThan3D(ub, L.shape() - 1);
      blitz::RectDomain < 3 > domain(lb, ub);

      blitz::Array<bool,3> mask(
          blitz::TinyVector<atb::BlitzIndexT,3>(ub - lb + 1));
      blitz::TinyVector<unsigned char, 3> color(
          colorMap[(currentLabel)](0), colorMap[(currentLabel)](1),
          colorMap[(currentLabel)](2));
      mask = (L(domain) == label);
      lv->updateData(mask, color, domain);
    }
    else if (trueLabel(label) != -1 && trueLabel(label) != label &&
             flag == 3 && label > 0 && label != backgroundLabel
             && validFlag(label - 1))
    {
      L_reassigned(label) = -1;
      //prepare update image data
      blitz::TinyVector<double, 3> cc(centers(label - 1) / elSize + 0.5);
      float r = blitz::max(RD(label - 1, blitz::Range::all())) /
          blitz::min(elSize) * 1.5;
      blitz::TinyVector<int, 3> lb(cc(0) - r, cc(1) - r, cc(2) - r);
      blitz::TinyVector<int, 3> ub(cc(0) + r, cc(1) + r, cc(2) + r);
      keepHigherThan3D(lb, blitz::TinyVector<int, 3>(0, 0, 0));
      keepLowerThan3D(ub, L.shape() - 1);
      blitz::RectDomain <3> domain(lb, ub);

      blitz::Array<bool,3> mask(
          blitz::TinyVector<atb::BlitzIndexT,3>(ub - lb + 1));

      if (classLabels(label - 1) == 0 && classPredictions(label - 1) > 0)
      {
        blitz::TinyVector<unsigned char, 3> color(
            colorMap[classPredictions(label - 1) + 3](0) * 0.7,
            colorMap[classPredictions(label - 1) + 3](1) * 0.7,
            colorMap[classPredictions(label - 1) + 3](2) * 0.7);
        mask = (L(domain) == label);
        lv->updateData(mask, color, domain);
      }
      else
      {
        blitz::TinyVector<unsigned char, 3> color(
            colorMap[classLabels(label - 1) + 3](0),
            colorMap[classLabels(label - 1) + 3](1),
            colorMap[classLabels(label - 1) + 3](2));
        mask = (L(domain) == label);
        lv->updateData(mask, color, domain);
      }
    }
    QString s = tr(
        "Label : %1   fixedLabel : %2   classLabel : %3   classPredictions: %4"
        " currentLabel : %5") .arg(L(z, y, x)).arg(trueLabel(L(z, y, x))).arg(
            classLabels(trueLabel(L(z, y, x)) - 1)).arg(
                classPredictions(trueLabel(L(z, y, x)) - 1)).arg(currentLabel);
    statusBar()->showMessage(s);
  }
  else if (currentJob == Linking)
  {
    // shift right click --> go to next end node (change the "wheel" function)
    // shift left click --> break the connection (for both parent and child)
    // and hence create a new file.
    // ctrl right left click --> get fileNo
    // ctrl left click --> assign fileNo
    int label = L(z, y, x);
    int _fileNo = fileNo(label - 1);
    if (flag == 6)
    {
      //search for position
      int oneLi = -1;
      if (currentCell > 0 && currentCell <= maxLabel &&
          fileNo(currentCell - 1) == currentFileNo)
      {
        oneLi = currentCell - 1;
      }
      else
      {
        for (int li = 0; li < maxLabel; li++)
        {
          if (fileNo(li) == currentFileNo)
          {
            oneLi = li;
            break;
          }
        }
      }
      if (head)
      {
        while (oneLi != -1 && child(oneLi) != -1)
        {
          oneLi = child(oneLi);
        }
      }
      else
      {
        while (oneLi != -1 && parent(oneLi) != -1)
        {
          oneLi = parent(oneLi);
        }
      }

      head = 1 - head;
      LOG << "head" << head << std::endl;
      blitz::TinyVector<int, 3> cc(centers(oneLi) / elSize + 0.5);
      LOG << cc << std::endl;
      lv->explorer->setDataPos(cc(2), cc(1), cc(0));
      lv->refreshImage();
    }
    else if (flag == 5)
    {
      currentFileNo = _fileNo;
      currentCell = label;
    }
    else if (flag == 2 && label > 0 && label != backgroundLabel &&
             validFlag(label - 1))
    {
      fileNo(label - 1) = currentFileNo;
      int j = child(label - 1);
      while (j != -1)
      {
        fileNo(j) = currentFileNo;
        j = child(j);
      }
      j = parent(label - 1);
      while (j != -1)
      {
        fileNo(j) = currentFileNo;
        j = parent(j);
      }
      //prepare update image data
      blitz::TinyVector<int, 3> cc(centers(label - 1) / elSize + 0.5);
      float r = blitz::max(RD(label - 1, blitz::Range::all())) /
          blitz::min(elSize) * 1.5;
      blitz::TinyVector<int, 3> lb(cc(0) - r, cc(1) - r, cc(2) - r);
      blitz::TinyVector<int, 3> ub(cc(0) + r, cc(1) + r, cc(2) + r);
      keepHigherThan3D(lb, blitz::TinyVector<int, 3>(0, 0, 0));
      keepLowerThan3D(ub, L.shape() - 1);
      blitz::RectDomain<3> domain(lb, ub);

      blitz::Array<bool,3> mask(
          blitz::TinyVector<atb::BlitzIndexT,3>(ub - lb + 1));
      blitz::TinyVector<unsigned char, 3> color(
          colorMap[currentFileNo + 2](0), colorMap[currentFileNo + 2](1),
          colorMap[currentFileNo + 2](2));
      mask = (L(domain) == label);
      lv->updateData(mask, color, domain);
    }
    else if (flag == 3 && label > 0 && label != backgroundLabel &&
             validFlag(label - 1))
    {
      if (parent(label - 1) != -1)
      {
        child(parent(label - 1)) = -1;
        parent(label - 1) = -1;
      }
      currentFileNo = blitz::max(fileNo) + 1;
      fileNo(label - 1) = currentFileNo;
      int j = child(label - 1);
      while (j != -1)
      {
        fileNo(j) = currentFileNo;
        j = child(j);
      }
      //prepare update image data
      blitz::TinyVector<int, 3> cc(centers(label - 1) / elSize + 0.5);
      float r = blitz::max(RD(label - 1, blitz::Range::all())) /
          blitz::min(elSize) * 1.5;
      blitz::TinyVector<int, 3> lb(cc(0) - r, cc(1) - r, cc(2) - r);
      blitz::TinyVector<int, 3> ub(cc(0) + r, cc(1) + r, cc(2) + r);
      keepHigherThan3D(lb, blitz::TinyVector<int, 3>(0, 0, 0));
      keepLowerThan3D(ub, L.shape() - 1);
      blitz::RectDomain<3> domain(lb, ub);

      blitz::Array<bool,3> mask(
          blitz::TinyVector<atb::BlitzIndexT,3>(ub - lb + 1));
      blitz::TinyVector<unsigned char, 3> color(
          colorMap[currentFileNo + 2](0), colorMap[currentFileNo + 2](1),
          colorMap[currentFileNo + 2](2));
      mask = (L(domain) == label);
      lv->updateData(mask, color, domain);
    }
    QString s = tr(
        "Label : %1   fileNo : %2   parent : %3   child: %4   currentFileNo : "
        "%5") .arg(label) .arg(fileNo(label - 1)).arg(parent(label - 1)).arg(
            child(label - 1)).arg(currentFileNo);
    statusBar()->showMessage(s);
  }
}

template<typename T>
void
mainFrame::view(T d, bool update)
{
  if (!update)
  {
    lv->setData(d);
    lv->setST(sliceThick);
    lv->show();
  }
  else
  {
    lv->updateData(d);
    lv->setST(sliceThick);
    lv->show();
  }
}

template<typename T>
void
mainFrame::viewRef(T d, bool update)
{
  if (!update)
  {
    lv->setRefData(d);
    lv->show();
  }
}

void
mainFrame::undo()
{
  data.resize(backData.shape());
  data = backData;
  view(data);

  lx = lxb;
  ly = lyb;
  lz = lzb;
}

void
mainFrame::backup()
{
  backData.resize(data.shape());
  backData = data;
  lxb = lx;
  lyb = ly;
  lzb = lz;
}

void
mainFrame::crop()
{
  bool ok = false;
  minX = QInputDialog::getInteger(
      this, tr("minX"), tr("X start at:"), 0, 0, lx - 1, 10, &ok);
  if (!ok)
      return;
  ok = false;
  maxX = QInputDialog::getInteger(
      this, tr("maxX"), tr("X end at:"), lx - 1, 0, lx - 1, 10, &ok);
  if (!ok)
      return;
  ok = false;
  minY = QInputDialog::getInteger(
      this, tr("minY"), tr("Y start at:"), 0, 0, ly - 1, 10, &ok);
  if (!ok)
      return;
  ok = false;
  maxY = QInputDialog::getInteger(
      this, tr("maxY"), tr("Y end at:"), ly - 1, 0, ly - 1, 10, &ok);
  if (!ok)
      return;
  ok = false;
  minZ = QInputDialog::getInteger(
      this, tr("minZ"), tr("Z start at:"), 0, 0, lz - 1, 10, &ok);
  if (!ok)
      return;
  ok = false;
  maxZ = QInputDialog::getInteger(
      this, tr("maxZ"), tr("Z end at:"), lz - 1, 0, lz - 1, 10, &ok);
  if (!ok)
      return;
  backup();
  lz = (maxZ - minZ + 1);
  ly = (maxY - minY + 1);
  lx = (maxX - minX + 1);
  data.resize((maxZ - minZ + 1), (maxY - minY + 1), (maxX - minX + 1));
  data = backData(Range(minZ, maxZ), Range(minY, maxY), Range(minX, maxX));
  view(data);
}

void
mainFrame::equal()
{
  //always scale element size to the smallest one;
  backup();
  data.setInterpolator(atb::LinearInterpolator<float,3>(atb::RepeatBT));
  data.rescale(blitz::TinyVector<double,3>(blitz::min(elSize)));
  view(data);
}

void
mainFrame::normRange()
{
  backup();
  atb::normalize(data, data, atb::MINMAX);
}

void
mainFrame::diffusion()
{
  backup();
  bool ok = false;

  kappa = QInputDialog::getDouble(
      this, tr("kappa"), tr(
          "the threshold for reliable edge (from gray value(0~1) the suitable "
          "range is 0.01 ~ 0.2):"), 0.1, 0, 2147483647, 5, &ok);
  if (!ok) return;
  ok = false;

  delta_t_df = QInputDialog::getDouble(
      this, tr("delta_t_df"), tr(
          "the time step for diffusion (should be small to keep the nonlinear "
          "behavior of diffusion):"), 0.5, 0.0625, 1, 5, &ok);
  if (!ok) return;
  ok = false;

  n_iters_df = QInputDialog::getInteger(
      this, tr("n_iters_df"), tr("iterations of diffusion:"), 5, 1, 100, 1,
      &ok);
  if (!ok) return;
  nonlinearDiffusion3D_AOS(data, elSize, n_iters_df, kappa, delta_t_df, 0, V);
  view(data, true);
}

void
mainFrame::normContrast()
{
  backup();
  bool ok = false;

  sigmaVar = QInputDialog::getDouble(
      this, tr("sigmaVar"), tr(
          "the gaussian sigma for compute neighbour mean and variance"),
      10, 1, 100, 5, &ok);
  if (!ok)
      return;
  ok = false;

  epsilon = QInputDialog::getDouble(
      this, tr("epsilon"), tr(
          "the small value to suppress the gain on very-low-contrast region"),
      0.001, 0.00001, 100, 5, &ok);
  if (!ok)
      return;

  atb::GaussianFilter<float,3>::apply(
      data, blitz::TinyVector<double,3>(1.0), data2,
      blitz::TinyVector<double,3>(sigmaVar),
      blitz::TinyVector<atb::BlitzIndexT,3>(0), atb::RepeatBT);
  data -= data2;
  data2 = blitz::pow2(data);
  atb::GaussianFilter<float,3>::apply(
      data2, blitz::TinyVector<double,3>(1.0), data2,
      blitz::TinyVector<double,3>(sigmaVar),
      blitz::TinyVector<atb::BlitzIndexT,3>(0), atb::RepeatBT);
  data /= blitz::sqrt(data2) + epsilon;
  atb::normalize(data, data, atb::MINMAX);
  view(data, true);
}

void
mainFrame::answer(int x, int y, int z, int /*flag*/)
{
  if (currentJob == Labelling)
  {
    QString s = tr(
        "Label : %1   fixedLabel : %2   classLabel : %3   "
        "classPredictions: %4   currentLabel : %5").arg(L(z, y, x)).arg(
            trueLabel(L(z, y, x))).arg(
                classLabels(trueLabel(L(z, y, x)) - 1)).arg(
                    classPredictions(trueLabel(L(z, y, x)) - 1)).arg(
                        currentLabel);
    statusBar()->showMessage(s);
  }
  else if (currentJob == Fixing)
  {
    QString s = tr(
        "Label : %1   fixedLabel : %2   classLabel : %3   classPredictions: %4 "
        "currentLabel : %5") .arg(L(z, y, x)).arg(trueLabel(L(z, y, x))).arg(
            classLabels(trueLabel(L(z, y, x)) - 1)).arg(
                classPredictions(trueLabel(L(z, y, x)) - 1)).arg(currentLabel);
    statusBar()->showMessage(s);
  }
  else if (currentJob == Linking)
  {
    int label = L(z, y, x);
    QString s = tr(
        "Label : %1   fileNo : %2   parent : %3   child: %4   currentFileNo : "
        "%5") .arg(label) .arg(fileNo(label - 1)).arg(parent(label - 1)).arg(
            child(label - 1)).arg(currentFileNo);
    statusBar()->showMessage(s);
  }
}

void
mainFrame::wheel(int delta)
{
  if (currentJob == Labelling)
  {
    currentLabel = currentLabel + (delta / 120);
    QString s = QString("currentLabel : %1").arg(currentLabel);
    statusBar()->showMessage(s);
  }
  else if (currentJob == Linking)
  {
    currentFileNo = currentFileNo + (delta / 120);
    QString s = QString("currentFileNo : %1").arg(currentFileNo);
    statusBar()->showMessage(s);
  }
}

void
mainFrame::increaseST()
{
  sliceThick++;
  if (sliceThick >= 20)
      increaseSTAct->setEnabled(false);
  decreaseSTAct->setEnabled(true);
  lv->setST(sliceThick);
  lv->refreshImage();
}

void
mainFrame::decreaseST()
{
  sliceThick--;
  sliceThick = std::max(sliceThick, 1);
  if (sliceThick == 1)
      decreaseSTAct->setEnabled(false);
  lv->setST(sliceThick);
  lv->refreshImage();
}

void
mainFrame::setJobToLabelling()
{
  currentJob = Labelling;
  labelAct->setChecked(true);
  fixAct->setChecked(false);
  linkAct->setChecked(false);
  initializeShowLabelling();
}

void
mainFrame::initializeShowLabelling()
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(L.size()); ++i)
  {
    int ll = 0;
    int label = trueLabel(L.dataFirst()[i]);
    if (label > 0)
    {
      if (classLabels(label - 1) == 0 && classPredictions(label - 1) > 0)
          ll = classPredictions(label - 1) + 3;
      else ll = classLabels(label - 1) + 3;
    }
    if (label == backgroundLabel) ll = 1;
    if (classLabels(label - 1) == 0 && classPredictions(label - 1) > 0)
    {
      showData.dataFirst()[3 * i    ] = (colorMap[ll])(0) * 0.7;
      showData.dataFirst()[3 * i + 1] = (colorMap[ll])(1) * 0.7;
      showData.dataFirst()[3 * i + 2] = (colorMap[ll])(2) * 0.7;
    }
    else
    {
      showData.dataFirst()[3 * i    ] = (colorMap[ll])(0);
      showData.dataFirst()[3 * i + 1] = (colorMap[ll])(1);
      showData.dataFirst()[3 * i + 2] = (colorMap[ll])(2);
    }
  }
  view(showData);
}

void
mainFrame::setJobToFixing()
{
  currentJob = Fixing;
  fixAct->setChecked(true);
  labelAct->setChecked(false);
  linkAct->setChecked(false);
  initializeShowFixing();
}

void
mainFrame::initializeShowFixing()
{
  blitz::Range all = blitz::Range::all();
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int z = 0; z < showData.extent(0); z++)
  {
    blitz::Array<unsigned char,3> sliceRandomColor = showData(z, all, all, all);
    blitz::Array<int, 3 - 1> sliceLabel = L(z, all, all);

    blitz::Array<unsigned char, 3>::iterator it = sliceRandomColor.begin();
    blitz::Array<int, 3 - 1>::iterator it2 = sliceLabel.begin();

    for (; it2 != sliceLabel.end(); it2++)
    {
      if (L_reassigned(*it2) >= 0)
      {
        int _label = L_reassigned(*it2);
        *it++ = (colorMap[_label])(0);
        *it++ = (colorMap[_label])(1);
        *it++ = (colorMap[_label])(2);
      }
      else
      {
        int ll = 0;
        int _label = *it2;
        if (_label > 0)
        {
          if (classLabels(_label - 1) == 0 && classPredictions(_label - 1) > 0)
              ll = classPredictions(_label - 1) + 3;
          else
              ll = classLabels(_label - 1) + 3;
        }
        if (_label == backgroundLabel)
            ll = 1;
        if (classLabels(_label - 1) == 0 && classPredictions(_label - 1) > 0)
        {
          *it++ = (colorMap[ll])(0) * 0.7;
          *it++ = (colorMap[ll])(1) * 0.7;
          *it++ = (colorMap[ll])(2) * 0.7;
        }
        else
        {
          *it++ = (colorMap[ll])(0);
          *it++ = (colorMap[ll])(1);
          *it++ = (colorMap[ll])(2);
        }
      }
    }
  }
  view(showData);
}

void
mainFrame::merge()
{
  blitz::Range all = blitz::Range::all();
  QString filename;

  filename = QFileDialog::getOpenFileName(
      this, "Choose file to show", lastOpenFolder,
      "Supported Files (*.h5);;HDF5-Files (*.h5);;All files (*)");
  if (filename.isEmpty())
      return;
  lastOpenFolder = filename;
  openFile(filename.toStdString(), 2);

  // relabel
  int z;
#pragma omp parallel for
  for (z = 0; z < showData.extent(0); z++)
  {
    blitz::Array<int, 3 - 1> sliceLabel = L(z, all, all);
    blitz::Array<int, 3 - 1>::iterator it2 = sliceLabel.begin();

    for (; it2 != sliceLabel.end(); it2++)
    {
      *it2 = trueLabel(*it2);
    }
  }

  // watershed
  // this will make empty "label"s..but I do not see problem now
  morphWatershed(l1, L, 26);
  L_reassigned = -1;
  //  maxLabel = blitz::max(L);

  fixAct->setChecked(false);
  labelAct->setChecked(false);
  initializeShowFixing();
}

void
mainFrame::setJobToLinking()
{
  currentJob = Linking;
  linkAct->setChecked(true);
  labelAct->setChecked(false);
  fixAct->setChecked(false);
  initializeShowLinking();
}

void
mainFrame::initializeShowLinking()
{
  blitz::Range all = blitz::Range::all();
  int z;

#pragma omp parallel for
  for (z = 0; z < showData.extent(0); z++)
  {
    blitz::Array<unsigned char,3> sliceRandomColor = showData(z, all, all, all);
    blitz::Array<int, 3 - 1> sliceLabel = L(z, all, all);

    blitz::Array<unsigned char, 3>::iterator it = sliceRandomColor.begin();
    blitz::Array<int, 3 - 1>::iterator it2 = sliceLabel.begin();

    for (; it2 != sliceLabel.end(); it2++)
    {
      //      *it++ = (unsigned char) (sqrt(*it3) * 255);
      int ll = 0;
      int _label = *it2;
      if (_label > 0)
      {
        ll = fileNo(_label - 1) + 2;
      }
      if (_label == backgroundLabel)
          ll = 1;

      *it++ = (colorMap[ll])(0);
      *it++ = (colorMap[ll])(1);
      *it++ = (colorMap[ll])(2);
    }
  }
  view(showData);
}
