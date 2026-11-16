#pragma once

#include <QUrl>
#include <QString>

#include <QJsonObject>
#include <QJsonArray>

#include <variant>

namespace QtCustom::Web::HTTPCommon
{

/**
 * @brief createUrl     Create URL using HTTP target info
 * @param serverAddress Address base, for example 127.0.0.1:9003
 * @param target        HTTP target, for example /api/v2/status
 * @param args          Arguments of HTTP target in format "NAME=VALUE"
 * @return              QUrl created using arguments
 */
QUrl createUrl(const QString& serverAddress, const QString& target, const QStringList& args = {});


/**
 * @brief getTarget Get target from URL
 * @param url
 * @return
 */
QString getTarget(const QUrl &url);


/**
 * @brief parseBody Parse HTTP body to QJsonObject
 * @param arr       Body
 * @return          String with error or parsed JSON on success
 */
std::variant<QString, QJsonObject> parseBody(const QByteArray& arr);


/**
 * @brief parseBodyArray    Parse HTTP body to QJsonObject
 * @param arr               Body
 * @return                  String with error or parsed JSON on success
 */
std::variant<QString, QJsonArray>  parseBodyArray(const QByteArray& arr);

}