/*!
 * InnerActiveAdModule.h
 */

#ifndef INNERACTIVEADMODULE_H_
#define INNERACTIVEADMODULE_H_

#include <QtCore>
#include <QSystemInfo>
#include <QNetworkAccessManager>
#include "InnerActiveAdWidget.h"

QTM_USE_NAMESPACE

class QNetworkReply;

/*! A TIaErrors enum. */
/** @enum TIaErrors
* 
*	@brief InnerActiveAdModule error codes.
*
*	Possible error code can be return on InnerActiveAdModule API calls.
*/
typedef enum
{
	EIaErrors_Corrupted				= -5, /*!< Ad banner control passed to updateBanner() function is NULL. */
        EIaErrors_AdDataNotReady                        = -4, /*!< Ad data is being retrieved from InnerActive server and still not ready. */
	EIaErrors_NoAppUid				= -3, /*!< Application UID (should be received from InnerActive) is empty. */
	EIaErrors_Overflow				= -2, /*!< paramVal string parameter (passed to setAdProtocolParameter() function) is too long. */
        EIaErrors_Processing                            = -1, /*!< InnerActiveAdModule API call cannot be comleted during ad data is being retrieved from InnerActive server. Please try again later.*/
	EIaErrors_General				= 0,  /*!< Some general error */
	EIaErrors_Ok					= 1,  /*!< InnerActiveAdModule API call completed successfully. */
	
} TIaErrors;


/*! A TIaProtocolParams enum. */
/** @enum TIaProtocolParams
* 
*	@brief InnerActiveAdModule protocol parameters.
*
*	Possible InnerActive protocol parameters could be set to InnerActiveAdModule. Please refer InnerActive protocol documentation.
*/
typedef enum
{
        EIaProtocolParams_DistributionChannelPortal     = 0, /*!< Distribution channel. Must be one of pre-defined channels in the system, possible values provided by innerActive. For example, for the Ovi Store the value should be 551.*/
        EIaProtocolParams_ExternalId			= 1, /*!< External ID. An application specific id - the id of the requesting device in the partner's domain. */
        EIaProtocolParams_Age				= 2, /*!< Age. User's age (value between 1 and 99). */
        EIaProtocolParams_Gender			= 3, /*!< User's gender. May be one of: "M, F, male, female". Case insensitive. */
        EIaProtocolParams_MobileNumber			= 4, /*!< User's mobile number. MSISDN format, with international prefix. */
        EIaProtocolParams_Keywords			= 5, /*!< Keywords. Keywords relevant to this user's specific session. Comma separated list. */
        EIaProtocolParams_Category			= 6, /*!< Category. Category - a single word description of the application. */
        EIaProtocolParams_LocationString		= 7, /*!< Location string. Comma separated list of country, state/province, city. For example: US, NY, NY. */
        EIaProtocolParams_LocationGps			= 8, /*!< GPS location. ISO code location data. In latitude, longitude format. */
} TIaProtocolParams;

/**
* @brief InnerActiveAdModule description.
*
* InnerActiveAdModule (derived from QObject) class object should be created in client application. Client application interacts with innerActive Ad library via this object.
*
*/
class InnerActiveAdModule : public QObject
{
	Q_OBJECT
	
public:
	/**
	* @brief InnerActiveAdModule constructor.
	* 
	* @param pParent pointer to parent QObject.
	* @param appUid inneractive provided application UID.
	* @param adReloadPeriodSec ad validity period (120 seconds by default). Possible values:[0-86400(24hr)]. If periodic ad request should be disabled, 0 should be passed.
	* 
	*/
	InnerActiveAdModule(QObject* pParent, const QString& appUid, uint adReloadPeriodSec = 120);
	
	/**
	* @brief InnerActiveAdModule destructor.
	* 
	*/
	virtual ~InnerActiveAdModule();

	/**
	* @brief Ad banner (widget) creator function.
	* 
	* This function creates ad banner (derived from QWidget) object. Caller is responsible to delete InnerActiveAdWidget object.
	*
	* @param pParent pointer to parent QWidget.
	* @param showAdText defines whether to show ad text next to the ad image (false by default).
	* @return pointer to InnerActiveAdWidget created object
	* @see InnerActiveAdWidget class.
	*/
	InnerActiveAdWidget* 		createAdBanner(QWidget* pParent,
												bool showAdText = false);
	
	/**
	* @brief InnerActive protocol parameters setter.
	*  
	* Using this function enables to alter inneractive ad protocol parameters.
	* 
	* @param paramKey protocol parameter type.
	* @param paramVal protocol parameter value.
	* @see TIaProtocolParams enum.
	*/
	int				setAdProtocolParameter(TIaProtocolParams paramKey, const QString& paramVal);

	/**
	* @brief Ad data update function.
	* 
	* Updates banner control (InnerActiveAdWidget*) passed to this function with latest ad data.
	* 
	* @return EIaErrors_Ok, EIaErrors_Processing, EIaErrors_AdDataNotReady, EIaErrors_Corrupted
	* @see TIaErrors enum.
	*/
	int				updateBanner(InnerActiveAdWidget* bannerControl);
	
	/**
	* @brief Open current ad link.
	* 
	* Activates current ad link in device browser. Symbian platform: in order to open the link in already running browser, SwEvent capability is required. 
	* 
	* @return EIaErrors_Ok, EIaErrors_AdDataNotReady
	* @see TIaErrors enum.
	*/
	int				activateAdLink();
	
	/**
	* @brief Opens modal ad dialog.  
	* 
	* @param parent QWidget* parent for Ad dialog.
	* @param skipTimeOutSeconds the dialog is closed after this time period (seconds) if greater than "0", otherwise the dialog is open until user closes it. 
	* @param skipText text description for skip action indication like "Skip in" with addition of seconds left. "Skip in 10".
	* @param lskBtnText left soft key (if applicable) or left on-dialog button text.  
	* @param rskBtnText right soft key (if applicable) or right on-dialog button text. 
	* @param adActivationOnRsk defines whether the ad link activation should be done on right softkey (if applicable) or right on-dialog button, otherwise left button handles ad link activation.  
	* @param showAdText defines whether to show ad text next to ad image.
	* @return EIaErrors_Ok, EIaErrors_Processing, EIaErrors_AdDataNotReady
	* @see TIaErrors enum.
	*/
	int				openAdDialog(QWidget* parent = 0, 
								int skipTimeOutSeconds = 0, 
								const QString& skipText = QString(),
								const QString& lskBtnText = tr("Skip"),
								const QString& rskBtnText = tr("Click"),
								bool adActivationOnRsk = true,
								bool showAdText = false);
	
public slots:
	/**
	* @brief Performs request for new ad data.
	*  
	* Retrieves new ad data from inneractive server. Ad module emits signal startReloadAd() at the beginning of the update process and adDataReady() / adDataCorrupted() / networkError(const QString& networkError) according to operation success.
	* @return EIaErrors_Ok, EIaErrors_Processing, EIaErrors_NoAppUid
	* @see TIaErrors enum.
	*/
        int			requestAd();
	
signals:
	/**
	* @brief Indicates start of the ad update process.
	*/
	void			startReloadAd();
	
	/**
	* @brief Indicates network error with description (if relevant).
	*/
	void			networkError(const QString& networkError);
	
	/**
	* @brief Indicates successful end of ad update operation - caller should update ad banner using updateBanner(InnerActiveAdWidget* bannerControl) function.
	*/
	void			adDataReady();
	
	/**
	* @brief Indicates corrupted ad data was received - caller should try to reload ad data using requestAd() function.
	*/
	void			adDataCorrupted();
	
private slots:
    void 			httpFinished();
    void 			httpReadyRead();

    void 			linkClickedHandler(const QUrl& url);
    
private:
    void 			startRequest(QUrl url);
    void			startAdImageDownload();
    void 			showAd(const QString& html);

    void 			saveClientId();
    QString                     readClientId() ;
    
    QString			phoneModelName();
    QString			phoneModelNameWorkaround();
    QString			createUserAgent(const QString& browserUserAgent, const QString& phoneModelName);
    
private:
    
    bool					iInitialized;
    int 					iHttpMode;
    
    QUrl 					iUrl;
    QNetworkAccessManager                       iNetAccessManager;
    QNetworkReply*                              ipNetworkReply;

    bool 					iAdReady;
    bool					iProcessing;

    QTimer*					ipReloadAdTimer;

    QString					iParamPo;
    QString					iParamEid;
    QString					iParamAge;
    QString					iParamGender;
    QString					iParamMobileNumber;
    QString					iParamKeywords;
    QString					iParamCategory;
    QString					iParamLocationString;
    QString					iParamLocationGps;
	
    uint 					iAdReloadTimeout;
	
    QByteArray                                  iLogoByteArray;
    QByteArray                                  iAdImageByteArray;
    QString					iXmlAdText;
    QString					iXmlAdLink;
    QString					iXmlAdImageUrl;
	
    QString					iAppUid;
    QString					iSdkVersion;
    QString					iInnerActiveClientId;
    QString					iUserAgent;
	
    QSystemDeviceInfo*                          ipSysDeviceInfo;


};



#endif /* INNERACTIVEADMODULE_H_ */
