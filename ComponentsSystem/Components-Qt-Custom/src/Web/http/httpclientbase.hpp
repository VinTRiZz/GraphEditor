#pragma once

#include <QObject>
#include <QNetworkAccessManager>

#include <Components/ExtraClasses/Error.h>

#include <map>

namespace QtCustom::Web {

/**
 * @brief The HttpErrorCodes enum Error codes, caused by HTTP client exchange
 */
enum HttpErrorCodes : int
{
    // TODO: Make unified (this ones used from ProjectAgency)
    EC_InvalidAddress   = 201,
    EC_ConnectionError  = 204,
    EC_InvalidArgument = 5,
    EC_ProtocolUnknown = 100,
};

/**
 * @brief The HTTPClientBase class Base for any HTTP client class
 */
class HTTPClientBase : public QObject,
                       public ExtraClasses::ErrorUserBase<ExtraClasses::ErrorBase>
{
    Q_OBJECT
public:
    explicit HTTPClientBase(QObject *parent = nullptr);

    /**
     * @brief isServerListening Used to check server
     * @param host              Address of a server
     * @param port              Port to check
     * @return                  true if ping succeed
     */
    static bool isServerListening(const QString &host, quint16 port);

    /**
     * @brief setServer     Set address of a server to send requests
     * @param serverAddress Already checked, correct server address if format ip:port
     */
    bool setServer(const QString& serverAddress);
    QString getServer() const;

    /**
     * @brief startFileUpload   Start file upload as multipart HTTP item
     * @param localFilePath     File to send (absolute or relative)
     * @param fileTarget        Target on server to send POST request
     * @return                  Reply object, handling file upload
     * @throws                  std::invalid_argument exception if file is invalid
     */
    QNetworkReply* startFileUpload(const QString& localFilePath, const QString& fileTarget);

    /**
     * @brief startFileDownload Start file download as multipart HTTP item
     * @param saveDirectory     Local file path to save as (absolute or relative)
     * @param fileTarget        Target on server to send GET request
     * @return                  Reply object, handling file download
     * @throws                  std::invalid_argument exception if directory is invalid
     */
    QNetworkReply* startFileDownload(const QString& localSavefile, const QString& fileTarget);

signals:
    void sig_validAddressSet();
    void sig_errorOccurs(const ExtraClasses::ErrorBase& err) const;

    void sig_simpleResponseGet(const QString& reqPath, const QString& respData) const;
    void sig_simpleResponsePost(const QString& reqPath, const QString& respData) const;
    void sig_simpleResponsePut(const QString& reqPath, const QString& respData) const;
    void sig_simpleResponseDelete(const QString& reqPath, const QString& respData) const;

private:
    QString                             m_serverAddress;
    mutable QNetworkAccessManager       m_requester; // Class state may not be affected anytime
    std::map<QByteArray, QByteArray>    m_commonHeaders;

protected:

    /**
     * @brief setCommonHeader   Adds header for createRequest generator. Does not check value correctness
     * @param headerName        Header name, in UTF-8, must be correct header
     * @param headerData        Header data, in UTF-8, must be correct value
     */
    void setCommonHeader(const QString& headerName, const QString& headerData);


    /**
     * @brief createRequest Create request object using added common headers, target and parameters. Does not check value correctness
     * @param target        Path, for example /api/v2/status
     * @param args          Args of target, in format "NAME=VALUE"
     * @return              Generated object, ready for using
     */
    QNetworkRequest createRequest(const QString& target, const QStringList& args = {}) const;
    QNetworkAccessManager& getRequester();

    // Simple requests
    void sendSimpleRequestGet(const QString& reqPath) const;
    void sendSimpleRequestPost(const QString& reqPath, const QString& reqData) const;
    void sendSimpleRequestPut(const QString& reqPath, const QString& reqData) const;
    void sendSimpleRequestDelete(const QString& reqPath) const;
};

}