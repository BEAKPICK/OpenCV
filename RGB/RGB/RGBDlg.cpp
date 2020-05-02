
// RGBDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "RGB.h"
#include "RGBDlg.h"
#include "afxdialogex.h"

#define RFACTOR 0.2989f
#define GFACTOR 0.5870f
#define BFACTOR 0.1140f

#define FOREWARD 1
#define BACKWARD 2

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int LUT_BLabeling8[8][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,0,0},
	{0,0,0,0,0,1,1,0},
	{0,0,0,0,0,1,1,1},
	{1,0,0,0,0,1,1,1},
	{1,1,0,0,0,1,1,1},
	{1,1,1,0,0,1,1,1},
	{1,1,1,1,0,1,1,1},
};

class MatImages
{
public:
	Mat rMat;
	Mat gMat;
	Mat bMat;
	Mat rGrayscaleMat;
	Mat gGrayscaleMat;
	Mat bGrayscaleMat;
	Mat rOtsuMat;
	Mat gOtsuMat;
	Mat bOtsuMat;

	Mat rOpeningMat;
	Mat gOpeningMat;
	Mat bOpeningMat;
	Mat rClosingMat;
	Mat gClosingMat;
	Mat bClosingMat;

	Mat rOpeningContourMat;
	Mat gOpeningContourMat;
	Mat bOpeningContourMat;

	Mat rClosingContourMat;
	Mat gClosingContourMat;
	Mat bClosingContourMat;
};

MatImages *matimages;

void changeColor(Mat img, Mat &copy, int i);
void changeContrast(Mat img, Mat &copy);
void binarization(Mat imgWork, Mat &copy);
void loadAll(Mat img, MatImages &matimages);
void opening(Mat img, Mat &copy, Mat filter);
void closing(Mat img, Mat &copy, Mat filter);
void contourTrace(Mat img, Mat &copy);

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRGBDlg 대화 상자



CRGBDlg::CRGBDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RGB_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRGBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Img, m_pic);
}

BEGIN_MESSAGE_MAP(CRGBDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Img_Search, &CRGBDlg::OnBnClickedImgSearch)
	ON_BN_CLICKED(IDC_Img_Save, &CRGBDlg::OnBnClickedImgSave)
	ON_BN_CLICKED(IDC_Red_Btn, &CRGBDlg::OnBnClickedRedBtn)
	ON_BN_CLICKED(IDC_Green_Btn, &CRGBDlg::OnBnClickedGreenBtn)
	ON_BN_CLICKED(IDC_Blue_Btn, &CRGBDlg::OnBnClickedBlueBtn)
	ON_BN_CLICKED(IDC_Contrast_Btn, &CRGBDlg::OnBnClickedContrastBtn)
	ON_BN_CLICKED(IDC_Otsu_Btn, &CRGBDlg::OnBnClickedOtsuBtn)
	ON_BN_CLICKED(IDC_Reset_Btn, &CRGBDlg::OnBnClickedResetBtn)
END_MESSAGE_MAP()


// CRGBDlg 메시지 처리기

BOOL CRGBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRGBDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRGBDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRGBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRGBDlg::OnBnClickedImgSearch()
{
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		if (img.empty()) matimages = new MatImages();
		pathName = dlg.GetPathName();
		CT2CA pszConvertedAnsiString_up(pathName);
		std::string up_pathName_str(pszConvertedAnsiString_up);
		img = cv::imread(up_pathName_str);
		DisplayImage(img, 3);
		loadAll(img, *matimages);
	}
}

void CRGBDlg::DisplayImage(Mat targetMat, int channel)
{
	CDC *pDC = nullptr;
	CImage* mfcImg = nullptr;

	pDC = m_pic.GetDC();
	CStatic *staticSize = (CStatic *)GetDlgItem(IDC_Img);
	staticSize->GetClientRect(rect);

	cv::Mat tempImage;
	cv::resize(targetMat, tempImage, Size(rect.Width(), rect.Height()));

	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biBitCount = 24;
	bitmapInfo.bmiHeader.biWidth = tempImage.cols;
	bitmapInfo.bmiHeader.biHeight = tempImage.rows;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biClrImportant = 0;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;

	if (targetMat.channels() == 3)
	{
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 24);
	}
	else if (targetMat.channels() == 1)
	{
		cvtColor(tempImage, tempImage, COLOR_GRAY2RGB);
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 24);
	}
	else if (targetMat.channels() == 4)
	{
		bitmapInfo.bmiHeader.biBitCount = 32;
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 32);
	}
	cv::flip(tempImage, tempImage, 0);
	::StretchDIBits(mfcImg->GetDC(), 0, 0, tempImage.cols, tempImage.rows,
		0, 0, tempImage.cols, tempImage.rows, tempImage.data, &bitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

	mfcImg->BitBlt(::GetDC(m_pic.m_hWnd), 0, 0);

	if (mfcImg)
	{
		mfcImg->ReleaseDC();
		delete mfcImg;// mfcImg = nullptr;
	}
	tempImage.release();
	ReleaseDC(pDC);
}

void CRGBDlg::OnBnClickedImgSave()
{
	if (matimages != NULL) 
	{
		imwrite("R.jpg", (*matimages).rMat);
		imwrite("G.jpg", (*matimages).gMat);
		imwrite("B.jpg", (*matimages).bMat);
		imwrite("R_Grayscale.jpg", (*matimages).rGrayscaleMat);
		imwrite("G_Grayscale.jpg", (*matimages).gGrayscaleMat);
		imwrite("B_Grayscale.jpg", (*matimages).bGrayscaleMat);
		imwrite("R_Otsu.jpg", (*matimages).rOtsuMat);
		imwrite("G_Otsu.jpg", (*matimages).gOtsuMat);
		imwrite("B_Otsu.jpg", (*matimages).bOtsuMat);
		imwrite("R_Opening.jpg", (*matimages).rOpeningMat);
		imwrite("R_Closing.jpg", (*matimages).rClosingMat);
		imwrite("G_Opening.jpg", (*matimages).gOpeningMat);
		imwrite("G_Closing.jpg", (*matimages).gClosingMat);
		imwrite("B_Opening.jpg", (*matimages).bOpeningMat);
		imwrite("B_Closing.jpg", (*matimages).bClosingMat);
		imwrite("R_OpeningContour.jpg", (*matimages).rOpeningContourMat);
		imwrite("R_ClosingContour.jpg", (*matimages).rClosingContourMat);
		imwrite("G_OpeningContour.jpg", (*matimages).gOpeningContourMat);
		imwrite("G_ClosingContour.jpg", (*matimages).gClosingContourMat);
		imwrite("B_OpeningContour.jpg", (*matimages).bOpeningContourMat);
		imwrite("B_ClosingContour.jpg", (*matimages).bClosingContourMat);
		MessageBox(_T("이미지 저장 완료!"), _T(""));
	}
	else 
	{
		MessageBox(_T("이미지를 로드해주세요 "), _T(""));
	}
}

void loadAll(Mat img, MatImages &matimages)
{
	if (!img.empty())
	{
		Mat filter = Mat(3, 3, CV_8UC1);	//원래 그냥 int값으로 줘도 되지만 필터를 정의하는 것이 더 의미있다고 판단.

		//red
		/*색깔 바꾸기*/
		matimages.rMat = img.clone();
		changeColor(img, matimages.rMat, 1);
		imshow("R", matimages.rMat);
		
		/*그레이 스케일*/
		matimages.rGrayscaleMat = Mat(matimages.rMat.size(), CV_8UC1);
		changeContrast(matimages.rMat, matimages.rGrayscaleMat);
		imshow("R_Grayscale", matimages.rGrayscaleMat);

		/*Otsu 이진화*/
		matimages.rOtsuMat = matimages.rGrayscaleMat.clone();
		binarization(matimages.rGrayscaleMat, matimages.rOtsuMat);
		imshow("R_Otsu", matimages.rOtsuMat);

		/*Opening*/
		matimages.rOpeningMat = matimages.rOtsuMat.clone();
		opening(matimages.rOtsuMat, matimages.rOpeningMat, filter);
		imshow("R_Opening", matimages.rOpeningMat);

		/*Closing*/
		matimages.rClosingMat = matimages.rOtsuMat.clone();
		closing(matimages.rOtsuMat, matimages.rClosingMat, filter);
		imshow("R_Closing", matimages.rClosingMat);

		/*Opening Contour*/
		matimages.rOpeningContourMat = matimages.rOpeningMat.clone();
		contourTrace(matimages.rOpeningMat, matimages.rOpeningContourMat);
		imshow("R_OpeningContour", matimages.rOpeningContourMat);
		
		/*Closing Contour*/
		matimages.rClosingContourMat = matimages.rClosingMat.clone();
		contourTrace(matimages.rClosingMat, matimages.rClosingContourMat);
		imshow("R_ClosingContour", matimages.rClosingContourMat);

		//green
		/*색깔 바꾸기*/
		matimages.gMat = img.clone();
		changeColor(img, matimages.gMat, 2);
		imshow("G", matimages.gMat);

		/*그레이 스케일*/
		matimages.gGrayscaleMat = Mat(matimages.gMat.size(), CV_8UC1);
		changeContrast(matimages.gMat, matimages.gGrayscaleMat);
		imshow("G_Grayscale", matimages.gGrayscaleMat);

		/*Otsu 이진화*/
		matimages.gOtsuMat = matimages.gGrayscaleMat.clone();
		binarization(matimages.gGrayscaleMat, matimages.gOtsuMat);
		imshow("G_Otsu", matimages.gOtsuMat);

		/*Opening*/
		matimages.gOpeningMat = matimages.gOtsuMat.clone();
		opening(matimages.gOtsuMat, matimages.gOpeningMat, filter);
		imshow("G_Opening", matimages.gOpeningMat);

		/*Closing*/
		matimages.gClosingMat = matimages.gOtsuMat.clone();
		closing(matimages.gOtsuMat, matimages.gClosingMat, filter);
		imshow("G_Closing", matimages.gClosingMat);

		/*Opening Contour*/
		matimages.gOpeningContourMat = matimages.gOpeningMat.clone();
		contourTrace(matimages.gOpeningMat, matimages.gOpeningContourMat);
		imshow("G_OpeningContour", matimages.gOpeningContourMat);

		/*Closing Contour*/
		matimages.gClosingContourMat = matimages.gClosingMat.clone();
		contourTrace(matimages.gClosingMat, matimages.gClosingContourMat);
		imshow("G_ClosingContour", matimages.gClosingContourMat);

		//blue
		/*색깔 바꾸기*/
		matimages.bMat = img.clone();
		changeColor(img, matimages.bMat, 3);
		imshow("B", matimages.bMat);

		/*그레이 스케일*/
		matimages.bGrayscaleMat = Mat(matimages.bMat.size(), CV_8UC1);
		changeContrast(matimages.bMat, matimages.bGrayscaleMat);
		imshow("B_Grayscale", matimages.bGrayscaleMat);

		/*Otsu 이진화*/
		matimages.bOtsuMat = matimages.bGrayscaleMat.clone();
		binarization(matimages.bGrayscaleMat, matimages.bOtsuMat);
		imshow("B_Otsu", matimages.bOtsuMat);

		/*Opening*/
		matimages.bOpeningMat = matimages.bOtsuMat.clone();
		opening(matimages.bOtsuMat, matimages.bOpeningMat, filter);
		imshow("B_Opening", matimages.bOpeningMat);

		/*Closing*/
		matimages.bClosingMat = matimages.bOtsuMat.clone();
		closing(matimages.bOtsuMat, matimages.bClosingMat, filter);
		imshow("B_Closing", matimages.bClosingMat);

		/*Opening Contour*/
		matimages.bOpeningContourMat = matimages.bOpeningMat.clone();
		contourTrace(matimages.bOpeningMat, matimages.bOpeningContourMat);
		imshow("B_OpeningContour", matimages.bOpeningContourMat);

		/*Closing Contour*/
		matimages.bClosingContourMat = matimages.bClosingMat.clone();
		contourTrace(matimages.bClosingMat, matimages.bClosingContourMat);
		imshow("B_ClosingContour", matimages.bClosingContourMat);
	}
}


void CRGBDlg::OnBnClickedRedBtn()
{
	if (!img.empty()) 
	{
		//Mat img_copy = img.clone();
		//changeColor(img, img_copy, 1);
		//DisplayImage(img_copy, 3);
		//imgWork.release();
		//imgWork = img_copy;
	}
}


void CRGBDlg::OnBnClickedGreenBtn()
{
	if (!img.empty()) 
	{
		//Mat img_copy = img.clone();
		//changeColor(img, img_copy, 2);
		//DisplayImage(img_copy, 3);
		//imgWork.release();
		//imgWork = img_copy;
	}
}


void CRGBDlg::OnBnClickedBlueBtn()
{
	if (!img.empty()) 
	{
		//Mat img_copy = img.clone();
		//changeColor(img, img_copy, 3);
		//DisplayImage(img_copy, 3);
		//imgWork.release();
		//imgWork = img_copy;
	}
}

void CRGBDlg::OnBnClickedResetBtn()
{
	if (!img.empty())
	{
		//DisplayImage(img, 3);
		//imgWork.release();
		//imgWork = img;
	}
}

void CRGBDlg::OnBnClickedContrastBtn()
{
	//if (!imgWork.empty())
	//{
		/*Mat img_copy = Mat(imgWork.rows, imgWork.cols, CV_8UC1);
		changeContrast(imgWork, img_copy);
		DisplayImage(img_copy, 1);
		imgWork.release();
		imgWork = img_copy;*/
	//}
}

void CRGBDlg::OnBnClickedOtsuBtn()
{
	//if (!imgWork.empty())
	//{
		//Mat img_copy = Mat(imgWork.size(), CV_8U);
		//binarization(imgWork, img_copy);
		//DisplayImage(img_copy, 1);
		//imgWork.release();
		//imgWork = img_copy;
	//}
}

struct Histogram {
	long int sumH;
	long int H[256];
	float HAvg;
	float nH[256];
	float w0[256];
	float m0[256];
	float m1[256];
};

void binarization(Mat imgWork, Mat &copy)
{
	if (!imgWork.empty())
	{
		Histogram histogram;
		histogram.sumH = 0;
		histogram.HAvg = 0;

		for (int x = 0; x < 256; x++)
		{
			histogram.H[x] = 0;
			histogram.nH[x] = 0;
			histogram.m0[x] = 0;
			histogram.w0[x] = 0;
			histogram.m1[x] = 0;
		}

		for (int y = 0; y < imgWork.rows; y++)
		{
			unsigned char* ptr = imgWork.data + (imgWork.cols*y);
			for (int x = 0; x < imgWork.cols; x++)
			{
				histogram.H[ptr[x]] += 1;
				histogram.sumH += 1;
			}
		}

		float sumPixelXnH = 0;
		float sumnH = 0;

		for (int x = 0; x < 256; x++)
		{
			histogram.nH[x] = histogram.H[x] / (float)histogram.sumH;
			sumPixelXnH += (float)(histogram.H[x] * histogram.nH[x]);
			sumnH += histogram.nH[x];
			histogram.w0[x] = sumnH;
			histogram.m0[x] = sumPixelXnH / sumnH;
		}
		histogram.HAvg = sumPixelXnH;

		sumPixelXnH = 0;
		sumnH = 0;
		for (int x = 255; x >=0; x--)
		{
			histogram.m1[x] = sumPixelXnH / sumnH;
			sumPixelXnH += (float)(histogram.H[x] * histogram.nH[x]);
			sumnH += histogram.nH[x];
		}

		float maxDistance = 0;
		unsigned char pivot = 0;
		float tempWDistanceAvg = 0;
		for (int x = 0; x < 256; x++)
		{
			tempWDistanceAvg = (powf((histogram.m0[x] - histogram.HAvg), 2) * histogram.w0[x])
				+ (powf((histogram.m1[x] - histogram.HAvg), 2) * (1-histogram.w0[x]));
			if (maxDistance < tempWDistanceAvg)
			{
				pivot = (unsigned char) x;
				maxDistance = tempWDistanceAvg;
			}
		}

		for (int y = 0; y < imgWork.rows; y++)
		{
			unsigned char* ptr = imgWork.data + (imgWork.cols*y);
			unsigned char* copyPtr = copy.data + (copy.cols*y);
			for (int x = 0; x < imgWork.cols; x++)
			{
				if (ptr[x] >= pivot)
				{
					copyPtr[x] = 255;
				}
				else
				{
					copyPtr[x] = 0;
				}
			}
		}
	}
}

void changeContrast(Mat imgWork, Mat &copy)
{
	if (!imgWork.empty() && imgWork.channels() != 1)
	{
		for (int y = 0; y < imgWork.rows; y++)
		{
			unsigned char* ptr = copy.data + (copy.cols*y);
			unsigned char* ptr2 = imgWork.data + 3 * (imgWork.cols*y);
			for (int x = 0; x < imgWork.cols; x++)
			{
				ptr[x] = (unsigned char) (ptr2[3 * x + 0] * BFACTOR +ptr2[3 * x + 1] * GFACTOR + ptr2[3 * x + 2] * RFACTOR);
			}
		}
	}
}

void dilation(Mat img, Mat &copy, Mat filter)
{
	if (filter.rows % 2 == 0 || filter.cols % 2 == 0) return;	//필터는 row와 cols가 항상 홀수여야한다.
	//그리고 img와 copy의 크기는 항상 같다.
	int startx = (int)(filter.cols*0.5);
	int starty = (int)(filter.rows*0.5);
	for (int y = starty; y < img.rows-starty; y++)
	{
		unsigned char* ptr = copy.data + (copy.cols*y);
		for (int x = startx; x < img.cols-startx; x++)
		{
			if (ptr[x] == 0)
			{
				int starth = (int)(y - filter.rows*0.5);
				int starti = (int)(x - filter.cols*0.5);
				int clear = 0;
				for (int h = starth; h < starth + filter.rows; h++)
				{
					unsigned char* imgPtr = img.data + (img.cols*h);
					for (int i = starti; i < starti + filter.cols; i++)
					{
						if (imgPtr[i] == 255)
						{
							ptr[x] = 255;
							clear = 1;
							break;
						}
					}
					if (clear) break;
				}
			}
		}
	}
}

void erosion(Mat img, Mat &copy, Mat filter)
{
	if (filter.rows % 2 == 0 || filter.cols % 2 == 0) return;	//필터는 row와 cols가 항상 홀수여야한다.
	//그리고 img와 copy의 크기는 항상 같다.
	int startx = (int)(filter.cols*0.5);
	int starty = (int)(filter.rows*0.5);
	for (int y = starty; y < img.rows-starty; y++)
	{
		unsigned char* ptr = copy.data + (copy.cols*y);
		for (int x = startx; x < img.cols-startx; x++)
		{
			if (ptr[x] == 255)
			{
				int starth = (int)(y - filter.rows*0.5);
				int starti = (int)(x - filter.cols*0.5);
				int clear = 0;
				for (int h = starth; h < starth + filter.rows; h++)
				{
					unsigned char* imgPtr = img.data + (img.cols*h);
					for (int i = starti; i < starti + filter.cols; i++)
					{
						if (imgPtr[i] == 0)
						{
							ptr[x] = 0;
							clear = 1;
							break;
						}
					}
					if (clear) break;
				}
			}
		}
	}
}

void readNeighbor8(Mat img, int y, int x, int neighbor8[8])
{
	unsigned char* ptr = img.data + (img.cols*y);
	unsigned char* ptr2 = img.data + (img.cols*(y-1));
	unsigned char* ptr3 = img.data + (img.cols*(y+1));
	neighbor8[0] = ptr[x + 1];
	neighbor8[1] = ptr3[x + 1];
	neighbor8[2] = ptr3[x];
	neighbor8[3] = ptr3[x-1];
	neighbor8[4] = ptr[x-1];
	neighbor8[5] = ptr2[x-1];
	neighbor8[6] = ptr2[x];
	neighbor8[7] = ptr2[x+1];
}

void calCoord(int addOrient, int &y, int &x)
{
	switch (addOrient)
	{
	case 0:
		x += 1;
		break;
	case 1:
		x += 1;
		y += 1;
		break;
	case 2:
		y += 1;
		break;
	case 3:
		x -= 1;
		y += 1;
		break;
	case 4:
		x -= 1;
		break;
	case 5:
		x -= 1;
		y -= 1;
		break;
	case 6:
		y -= 1;
		break;
	case 7:
		x += 1;
		y -= 1;
		break;
	}
}

void contourTraceUnit(Mat img, Mat &copy, int &y, int &x, int label, int tag)
{
	int neighbor8[8] = { 0, };
	int start = 0;
	int currentOrient = 0;
	int previousOrient = 0;
	int i = 0;
	int endX = x, endY = y, previousX = x, previousY = y;
	int addOrient = 0;
	if (tag == BACKWARD)
	{
		currentOrient = 6;
		previousOrient = 6;
	}
	do 
	{
		unsigned char* ptr = copy.data + (copy.cols*y);
		ptr[x] = 255;
		readNeighbor8(copy, y, x, neighbor8);
		start = (8 + currentOrient - 2) % 8;
		for (i = 0; i < 8; i++)
		{
			addOrient = (start + i) % 8;
			if (neighbor8[addOrient] != 0) break;
		}
		if (i < 8)
		{
			calCoord(addOrient, y, x);
			currentOrient = addOrient;
		}
		if (LUT_BLabeling8[previousOrient][currentOrient])
		{
			unsigned char* ptr = copy.data + (copy.cols*previousY);
			//ptr[previousX] = label;
			ptr[previousX] = 255;
		}
		previousX = x;
		previousY = y;
		previousOrient = currentOrient;
	} while ((y != endY) || (x != endX));
}

/*여기서의 img는 opening혹은 closing영상, 8-connectivity가 전제됨*/
void contourTrace(Mat img, Mat &copy)
{
	unsigned char refPixel1 = 0;
	unsigned char refPixel2 = 0;
	int labelnumber = 1;
	for (int y = 0; y < copy.rows; y++)
	{
		unsigned char* ptr = copy.data + (copy.cols*y);
		for (int x = 0; x < copy.cols; x++)
		{
			if (ptr[x] == 0)
			{
				ptr[x] = 1;
			}
			else ptr[x] = 0;
		}
	}

	for (int y = 1; y < copy.rows-1; y++)
	{
		unsigned char* ptr = copy.data + (copy.cols*y);
		for (int x = 1; x < copy.cols-1; x++)
		{
			if (ptr[x] == 1) // object
			{
				refPixel1 = ptr[x-1];
				unsigned char* ptr2 = copy.data + (copy.cols*(y-1));
				refPixel2 = ptr2[x - 1];
				//refPixel2 = *(ptr-copy.cols-1);
				//if (refPixel1 > 1)//propagation
				//{
				//	 ptr[x] = refPixel1;
				//}
				if (refPixel1 == 255 || refPixel2 == 255)
				{
					continue;
				}
				else if ((refPixel1 == 0) && (refPixel2 >= 2))//hole
				{
					ptr[x] = refPixel2;
					contourTraceUnit(img, copy, y, x, labelnumber, BACKWARD);
				}
				else if ((refPixel1 == 0) && (refPixel2 == 0))//region start
				{
					labelnumber++;
					ptr[x] = labelnumber;
					contourTraceUnit(img, copy, y, x, labelnumber, FOREWARD);
				}
			}
			else if (ptr[x] == 255) continue;
			else ptr[x] = 0;
		}
	}
}

void opening(Mat img, Mat &copy, Mat filter)
{
	erosion(img, copy, filter);
	Mat copyCopy = copy.clone();
	dilation(copy, copyCopy, filter);
	copy.release();
	copy = copyCopy.clone();
}

void closing(Mat img, Mat &copy, Mat filter)
{
	dilation(img, copy, filter);
	Mat copyCopy = copy.clone();
	erosion(copy, copyCopy, filter);
	copy.release();
	copy = copyCopy.clone();
}

void changeColor(Mat img, Mat &copy, int i)
{
	if (i == 1)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
				ptr1[3 * x + 0] = 0;
				ptr1[3 * x + 1] = 0;
				ptr1[3 * x + 2] = ptr1[3 * x + 2];
			}
		}
	}
	else if (i == 2)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
				ptr1[3 * x + 0] = 0;
				ptr1[3 * x + 1] = ptr1[3 * x + 1];
				ptr1[3 * x + 2] = 0;
			}
		}
	}
	else if (i == 3)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
				ptr1[3 * x + 0] = ptr1[3 * x + 0];
				ptr1[3 * x + 1] = 0;
				ptr1[3 * x + 2] = 0;
			}
		}
	}

}