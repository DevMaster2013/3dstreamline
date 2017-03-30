#include "stdafx.h"
#include "RecorderApp.h"
#include "RecorderDlg.h"
#include "afxdialogex.h"

#include <FolderDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRecorderDlg::CRecorderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RECORDERAPP_DIALOG, pParent)
	, _addedSources()
	, _server(new util::Server())
	, _processingThread(INVALID_HANDLE_VALUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRecorderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IMAGE_SOURCE_TYPE, _comboImageSourceTypes);
	DDX_Control(pDX, IDC_LIST_LOG_MESSAGES, _lstLogMessages);
	DDX_Control(pDX, IDC_LIST_ADDED_SOURCES, _lstAddedSources);
}

BEGIN_MESSAGE_MAP(CRecorderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_SOURCE_TYPE, &CRecorderDlg::OnCbnSelchangeComboImageSourceType)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_INPUT_FOLDER, &CRecorderDlg::OnBnClickedButtonBrowseInputFolder)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_OUTPUT_FOLDER, &CRecorderDlg::OnBnClickedButtonBrowseOutputFolder)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_CALIBRATION_FILE, &CRecorderDlg::OnBnClickedButtonBrowseCalibrationFile)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SOURCE, &CRecorderDlg::OnBnClickedButtonAddSource)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SOURCE, &CRecorderDlg::OnBnClickedButtonUpdateSource)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SOURCE, &CRecorderDlg::OnBnClickedButtonDeleteSource)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SOURCES, &CRecorderDlg::OnBnClickedButtonClearSources)
	ON_LBN_SELCHANGE(IDC_LIST_ADDED_SOURCES, &CRecorderDlg::OnLbnSelchangeListAddedSources)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CRecorderDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CRecorderDlg::OnBnClickedButtonDisconnect)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_START, &CRecorderDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CRecorderDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()

BOOL CRecorderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_isActive = true;

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_UPDATE_SOURCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DELETE_SOURCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR_SOURCES)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

	fillImageSourceTypes();

	return TRUE;
}

void CRecorderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CRecorderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRecorderDlg::fillImageSourceTypes()
{
	_comboImageSourceTypes.AddString("Images Folder");
	_comboImageSourceTypes.SetCurSel(0);
	OnCbnSelchangeComboImageSourceType();
}

void CRecorderDlg::addNewSource(int insertIndex)
{
	int sourceType;
	CString inputFolder;
	CString outputFolder;
	CString calibFile;
	int frameRate;

	sourceType = _comboImageSourceTypes.GetCurSel();
	GetDlgItemText(IDC_EDIT_INPUT_FOLDER, inputFolder);
	GetDlgItemText(IDC_EDIT_OUTPUT_FOLDER, outputFolder);
	GetDlgItemText(IDC_EDIT_CALIBRATION_FILE, calibFile);
	frameRate = GetDlgItemInt(IDC_EDIT_FRAME_RATE);

	ImageSource* addedSource = ImageSource::createImageSource(sourceType);
	addedSource->setCalibrationFile(calibFile.GetBuffer());
	addedSource->setFrameRate(frameRate);
	addedSource->setInputName(inputFolder.GetBuffer());
	addedSource->setOutputFolder(outputFolder.GetBuffer());	
	_addedSources.push_back(addedSource);

	CString addedString = ImageSource::getImageSourceName(sourceType) + CString(" : ") + inputFolder;
	if (_lstAddedSources.FindString(0, addedString) == LB_ERR)
	{
		if (insertIndex == -1)
		{
			insertIndex = _lstAddedSources.AddString(addedString);			
		}
		else
		{
			_lstAddedSources.InsertString(insertIndex, addedString);
		}
		_lstAddedSources.SetItemDataPtr(insertIndex, addedSource);
		addLogMessage("New source added");
	}	

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_UPDATE_SOURCE)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_DELETE_SOURCE)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_CLEAR_SOURCES)->EnableWindow(_addedSources.size() > 0);
}

void CRecorderDlg::deleteSelectedSource()
{
	int curSel = _lstAddedSources.GetCurSel();
	if (curSel >= 0)
	{
		ImageSource* source = (ImageSource*)_lstAddedSources.GetItemDataPtr(curSel);
		if (source != nullptr)
		{
			auto found = std::find(_addedSources.begin(), _addedSources.end(), source);
			if (found != _addedSources.end())
				_addedSources.erase(found);
			delete source;
		}
		_lstAddedSources.DeleteString(curSel);
		addLogMessage("Source deleted");
	}

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_UPDATE_SOURCE)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_DELETE_SOURCE)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_CLEAR_SOURCES)->EnableWindow(_addedSources.size() > 0);
}

void CRecorderDlg::updateSelectedSource()
{
	int curSel = _lstAddedSources.GetCurSel();
	deleteSelectedSource();
	addNewSource(curSel);
}

void CRecorderDlg::clearAllSources()
{
	for each (auto& source in _addedSources)
	{
		if (source != nullptr)
			delete source;
	}
	_addedSources.clear();
	_lstAddedSources.ResetContent();
	addLogMessage("All sources deleted");
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_UPDATE_SOURCE)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_DELETE_SOURCE)->EnableWindow(_addedSources.size() > 0);
	GetDlgItem(IDC_BUTTON_CLEAR_SOURCES)->EnableWindow(_addedSources.size() > 0);
}

void CRecorderDlg::showSourceInfo(int index)
{
	if (index < 0) return;

	ImageSource* source = (ImageSource*)_lstAddedSources.GetItemDataPtr(index);
	if (source != nullptr)
	{
		SetDlgItemText(IDC_EDIT_INPUT_FOLDER, source->getInputName().c_str());
		SetDlgItemText(IDC_EDIT_OUTPUT_FOLDER, source->getOutputFolder().c_str());
		SetDlgItemText(IDC_EDIT_CALIBRATION_FILE, source->getCalibrationFile().c_str());
		SetDlgItemInt(IDC_EDIT_FRAME_RATE, source->getFrameRate());
		_comboImageSourceTypes.SetCurSel(source->getSourceType());
	}
}

void CRecorderDlg::serverConnect()
{
	CString connectionPort;
	GetDlgItemText(IDC_EDIT_CONNECTION_PORT, connectionPort);

	if (_server != nullptr && !(_server->isConnected()))
	{
		if (_server->initialize() && _server->connect("", connectionPort))
		{
			_server->addObserver(this);
			addLogMessage("Server connected");

			GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
		}
	}
}

void CRecorderDlg::serverDisconnect()
{
	if (_server != nullptr && _server->isConnected())
	{
		_server->finalize();
		addLogMessage("Server disconnected");

		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);
	}
}

void CRecorderDlg::start()
{
	_processingThread = CreateThread(nullptr, 0, processingThread, this, CREATE_ALWAYS, 0);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}

void CRecorderDlg::stop()
{
	if (_processingThread != INVALID_HANDLE_VALUE)
	{
		_isActive = false;
		WaitForSingleObject(_processingThread, 1000);
		_processingThread = INVALID_HANDLE_VALUE;

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	}
}

void CRecorderDlg::addLogMessage(const std::string & logMessage)
{
	SYSTEMTIME locTime;
	GetLocalTime(&locTime);
	CString time;
	time.Format("%02d:%02d:%02d", locTime.wHour, locTime.wMinute, locTime.wSecond);

	CString message = time + " : " + logMessage.c_str();
	_lstLogMessages.AddString(message);
}

void CRecorderDlg::startAllSources()
{
	bool stillThereAreData = false;
	do
	{
		stillThereAreData = false;
		for each (auto& source in _addedSources)
		{
			if (!_isActive)
				break;

			if (!source->isEndOfStream())
			{
				stillThereAreData = true;

				cv::Mat image;
				std::string imageName;
				source->processNextImage(image, imageName);

				// Log the processed image
				addLogMessage(imageName + " processed");

				// Send the image name to the clients
				_server->send((void*)(imageName.c_str()), imageName.length());
			}
		}

	} while (_isActive && stillThereAreData);	
}

void CRecorderDlg::OnCbnSelchangeComboImageSourceType()
{
	int curSel = _comboImageSourceTypes.GetCurSel();
	if (curSel == 0)	// Image Source Folder
	{

	}
}

void CRecorderDlg::OnBnClickedButtonBrowseInputFolder()
{
	CString selectedPath;
	GetDlgItemText(IDC_EDIT_INPUT_FOLDER, selectedPath);

	util::FolderDialog dlg(selectedPath.GetBuffer());
	if (dlg.showDialog())
	{
		selectedPath = dlg.getFolderPath();
		SetDlgItemText(IDC_EDIT_INPUT_FOLDER, selectedPath);
	}
}


void CRecorderDlg::OnBnClickedButtonBrowseOutputFolder()
{
	CString selectedPath;
	GetDlgItemText(IDC_EDIT_OUTPUT_FOLDER, selectedPath);

	util::FolderDialog dlg(selectedPath.GetBuffer());
	if (dlg.showDialog())
	{
		selectedPath = dlg.getFolderPath();
		SetDlgItemText(IDC_EDIT_OUTPUT_FOLDER, selectedPath);
	}
}


void CRecorderDlg::OnBnClickedButtonBrowseCalibrationFile()
{
	CFileDialog dlg(TRUE, "txt", 0, 6UL, "Calibration Files (*.txt)|*.txt|");
	if (dlg.DoModal())
	{
		CString selectedFile = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT_CALIBRATION_FILE, selectedFile);
	}
}

void CRecorderDlg::OnBnClickedButtonAddSource()
{
	addNewSource();
}

void CRecorderDlg::OnBnClickedButtonUpdateSource()
{
	updateSelectedSource();
}

void CRecorderDlg::OnBnClickedButtonDeleteSource()
{
	deleteSelectedSource();
}

void CRecorderDlg::OnBnClickedButtonClearSources()
{
	clearAllSources();
}

void CRecorderDlg::OnLbnSelchangeListAddedSources()
{
	showSourceInfo(_lstAddedSources.GetCurSel());
}

void CRecorderDlg::onDataRecieved(void * data, size_t dataLength)
{
	addLogMessage("Data with length (" + std::to_string(dataLength) + ") recieved");
}

void CRecorderDlg::onClinetConnected()
{
	addLogMessage("Client connected");
}

void CRecorderDlg::OnBnClickedButtonConnect()
{
	serverConnect();
}

void CRecorderDlg::OnBnClickedButtonDisconnect()
{
	serverDisconnect();
}

void CRecorderDlg::OnClose()
{
	stop();

	clearAllSources();
	serverDisconnect();
	if (_server != nullptr)
		delete _server;
	__super::OnClose();
}

DWORD CRecorderDlg::processingThread(LPVOID parameter)
{
	CRecorderDlg* dlg = (CRecorderDlg*)parameter;
	dlg->startAllSources();
	return 0;
}

void CRecorderDlg::OnBnClickedButtonStart()
{
	start();
}


void CRecorderDlg::OnBnClickedButtonStop()
{
	stop();
}
