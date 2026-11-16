#include "httpclientbase.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QNetworkCookieJar>

#include <QFile>
#include <QFileInfo>

#include "httpcommon.hpp"

namespace QtCustom::Web {

HTTPClientBase::HTTPClientBase(QObject *parent)
    : QObject{parent}
{

}

bool HTTPClientBase::isServerListening(const QString &host, quint16 port) {
    QTcpSocket socket;
    socket.connectToHost(host, port);
    if (socket.waitForConnected(1000)) {
        socket.disconnectFromHost();
        return true;
    }
    return false;
}

bool HTTPClientBase::setServer(const QString &serverAddress)
{
    m_error.reset();
    auto splittedAddr = serverAddress.split(":", Qt::SkipEmptyParts);
    if (splittedAddr.size() < 2) {
        m_error.setCode(EC_InvalidAddress);
        m_error.setDetailText("No port in address");
        emit sig_errorOccurs(m_error);
        return false;
    }
    auto port = splittedAddr.back().toInt();
    if (port < 0 || port > 65535) {
        m_error.setCode(EC_InvalidAddress);
        m_error.setDetailText("Invalid port");
        emit sig_errorOccurs(m_error);
        return false;
    }
    if (!isServerListening(splittedAddr.front(), port)) {
        m_error.setCode(EC_ConnectionError);
        m_error.setDetailText(std::string("Failed to ping server. Is address correct? Address: ") + serverAddress.toStdString());
        emit sig_errorOccurs(m_error);
        return false;
    }
    m_serverAddress = serverAddress;
    emit sig_validAddressSet();
    return true;
}

QString HTTPClientBase::getServer() const
{
    return m_serverAddress;
}

QNetworkReply *HTTPClientBase::startFileUpload(const QString &localFilePath, const QString &fileTarget)
{
    std::shared_ptr<QFile> file = std::make_shared<QFile>(localFilePath);
    file->open(QIODevice::ReadOnly);
    if (!file->isOpen()) {
        m_error.setCode(EC_InvalidArgument);
        m_error.setDetailText("Download failed: Invalid send file path");
        emit sig_errorOccurs(m_error);
        return {};
    }

    auto request = createRequest(fileTarget);
    auto reply = m_requester.post(request, file.get());
    connect(reply, &QNetworkReply::finished, [reply, file]() { // For file correct existance during upload
        file->close();
    });
    return reply;
}

QNetworkReply *HTTPClientBase::startFileDownload(const QString &localSavefile, const QString &fileTarget)
{
    std::shared_ptr<QFile> file = std::make_shared<QFile>(localSavefile);
    if (!file->open(QIODevice::WriteOnly)) {
        m_error.setCode(EC_InvalidArgument);
        m_error.setDetailText("Download failed: Invalid local save file path");
        emit sig_errorOccurs(m_error);
        return {};
    }

    auto request = createRequest(fileTarget);
    auto reply = m_requester.get(request);
    connect(reply, &QNetworkReply::readyRead, [reply, file]() {
        file->write(reply->readAll());
    });
    connect(reply, &QNetworkReply::finished, [reply, file]() { // For file correct existance during download
        file->close();
    });
    return reply;
}

void HTTPClientBase::setCommonHeader(const QString &headerName, const QString &headerData)
{
    m_commonHeaders[headerName.toUtf8()] = headerData.toUtf8();
}

QNetworkRequest HTTPClientBase::createRequest(const QString &target, const QStringList &args) const
{
    QNetworkRequest res;
    res.setUrl(HTTPCommon::createUrl(m_serverAddress, target, args));

    for (auto& [headerName, headerData] : m_commonHeaders) {
        res.setRawHeader(headerName, headerData);
    }

    return res;
}

QNetworkAccessManager &HTTPClientBase::getRequester()
{
    return m_requester;
}

void HTTPClientBase::sendSimpleRequestGet(const QString &reqPath) const
{
    auto req = createRequest(reqPath);
    // COMPLOG_DEBUG("HTTPClientBase: REQUESTING GET", reqPath.toStdString());
    auto resp = m_requester.get(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, reqPath, resp](){
                if (resp->error() != QNetworkReply::NoError) {
                    ExtraClasses::ErrorBase err;
                    err.setCode(EC_ProtocolUnknown);
                    auto detailStr = QString("Request [GET] error: %1 (response text: \"%2\")").arg(
                        resp->errorString(),
                        resp->readAll());
                    err.setDetailText(detailStr.toStdString());
                    emit sig_errorOccurs(err);
                    return;
                }
                emit sig_simpleResponseGet(reqPath, resp->readAll());
            });
}

void HTTPClientBase::sendSimpleRequestPost(const QString &reqPath, const QString &reqData) const
{
    auto req = createRequest(reqPath);
    // COMPLOG_DEBUG("HTTPClientBase: REQUESTING POST", reqPath.toStdString(), reqData.toStdString());
    auto resp = m_requester.post(req, reqData.toUtf8());
    connect(resp, &QNetworkReply::finished,
            this, [this, reqPath, resp](){
                if (resp->error() != QNetworkReply::NoError) {
                    ExtraClasses::ErrorBase err;
                    err.setCode(EC_ProtocolUnknown);
                    auto detailStr = QString("Request [POST] error: %1 (response text: \"%2\")").arg(
                        resp->errorString(),
                        resp->readAll());
                    err.setDetailText(detailStr.toStdString());
                    emit sig_errorOccurs(err);
                    return;
                }
                emit sig_simpleResponsePost(reqPath, resp->readAll());
            });
}

void HTTPClientBase::sendSimpleRequestPut(const QString &reqPath, const QString &reqData) const
{
    auto req = createRequest(reqPath);
    // COMPLOG_DEBUG("HTTPClientBase: REQUESTING PUT", reqPath.toStdString(), reqData.toStdString());
    auto resp = m_requester.put(req, reqData.toUtf8());
    connect(resp, &QNetworkReply::finished,
            this, [this, reqPath, resp](){
                if (resp->error() != QNetworkReply::NoError) {
                    ExtraClasses::ErrorBase err;
                    err.setCode(EC_ProtocolUnknown);
                    auto detailStr = QString("Request [PUT] error: %1 (response text: \"%2\")").arg(
                        resp->errorString(),
                        resp->readAll());
                    err.setDetailText(detailStr.toStdString());
                    emit sig_errorOccurs(err);
                    return;
                }
                emit sig_simpleResponsePut(reqPath, resp->readAll());
            });
}

void HTTPClientBase::sendSimpleRequestDelete(const QString &reqPath) const
{
    auto req = createRequest(reqPath);
    // COMPLOG_DEBUG("HTTPClientBase: REQUESTING DELETE", reqPath.toStdString());
    auto resp = m_requester.deleteResource(req);
    connect(resp, &QNetworkReply::finished,
            this, [this, reqPath, resp](){
                if (resp->error() != QNetworkReply::NoError) {
                    ExtraClasses::ErrorBase err;
                    err.setCode(EC_ProtocolUnknown);
                    auto detailStr = QString("Request [DELETE] error: %1 (response text: \"%2\")").arg(
                        resp->errorString(),
                        resp->readAll());
                    err.setDetailText(detailStr.toStdString());
                    emit sig_errorOccurs(err);
                    return;
                }
                emit sig_simpleResponseDelete(reqPath, resp->readAll());
            });
}

}