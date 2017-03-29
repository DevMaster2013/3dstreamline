#pragma once
#include "afxwin.h"
#include <vector>
#include "ImageSource.h"
#include <Server.h>
#include <ICommunicatorObserver.h>

class CRecorderDlg : public CDialogEx, public util::ICommunicatorObserver
{
public:
	CRecorderDlg(CWnd* pParent = NULL);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RECORDERAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CComboBox _comboImageSourceTypes;
	CListBox _lstLogMessages;
	CListBox _lstAddedSources;

private:
	std::vector<ImageSource*> _addedSources;
	util::Server* _server;

private:
	void fillImageSourceTypes();
	void addNewSource(int insertIndex = -1);
	void deleteSelectedSource();
	void updateSelectedSource();
	void clearAllSources();
	void showSourceInfo(int index);
	void serverConnect();
	void serverDisconnect();
	void start();
	void stop();
	void addLogMessage(const std::string& logMessage);

public:
	afx_msg void OnCbnSelchangeComboImageSourceType();
	afx_msg void OnBnClickedButtonBrowseInputFolder();
	afx_msg void OnBnClickedButtonBrowseOutputFolder();
	afx_msg void OnBnClickedButtonBrowseCalibrationFile();
	afx_msg void OnBnClickedButtonAddSource();
	afx_msg void OnBnClickedButtonUpdateSource();
	afx_msg void OnBnClickedButtonDeleteSource();
	afx_msg void OnBnClickedButtonClearSources();
	afx_msg void OnLbnSelchangeListAddedSources();

private:
	virtual void onDataRecieved(void * data, size_t dataLength) override;
	virtual void onClinetConnected(const char * clientName) override;
public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnClose();
};
