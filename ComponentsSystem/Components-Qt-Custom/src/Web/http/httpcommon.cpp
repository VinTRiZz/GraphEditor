#include "httpcommon.hpp"

#include <QJsonDocument>

namespace QtCustom::Web::HTTPCommon
{

QUrl createUrl(const QString &serverAddress, const QString &target, const QStringList &args)
{
    return QUrl("http://" + serverAddress + target + args.join("&"));
}

QString getTarget(const QUrl &url)
{
    auto urlString = url.toString(QUrl::RemoveScheme | QUrl::RemovePort | QUrl::RemoveAuthority);
    return urlString;
}

std::variant<QString, QJsonObject> parseBody(const QByteArray &arr)
{
    QJsonParseError err;
    auto document = QJsonDocument::fromJson(arr, &err);
    if (document.isNull()) {
        return err.errorString();
    }
    return document.object();
}

std::variant<QString, QJsonArray> parseBodyArray(const QByteArray &arr)
{
    QJsonParseError err;
    auto document = QJsonDocument::fromJson(arr, &err);
    if (document.isNull()) {
        return err.errorString();
    }
    return document.array();
}

}