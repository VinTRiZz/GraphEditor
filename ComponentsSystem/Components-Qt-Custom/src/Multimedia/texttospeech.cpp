#include "texttospeech.hpp"

#include <QTextToSpeech>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>
#include <QEventLoop>
#include <QPointer>

namespace QtCustom::Multimedia {

struct TextToSpeech::Impl
{
    QString                 text;
    double                  speed = 0.2;
    VoiceSettings           settings {};
    QPointer<QTextToSpeech> engine;
    QFuture<void>           future;
    bool                    speaking = false;
};

TextToSpeech::TextToSpeech(QObject *parent)
    : QObject(parent)
    , d(std::make_shared<Impl>())
{
    auto engines = QTextToSpeech::availableEngines();
    if (!engines.contains("rhvoice")) {
        // Install (Debian 13):
        /*
            sudo apt install -y speech-dispatcher-rhvoice rhvoice-russian rhvoice-english;
            mkdir -p ~/.config/speech-dispatcher &&
            nano ~/.config/speech-dispatcher/speechd.conf &&
            systemctl --user restart speech-dispatcher
        */
        // test:    spd-say -o rhvoice "Example text for testing"
        qWarning() << "QtCustom::TextToSpeech: rhvoice module not found. Highly recommended to install it";
    }
}

TextToSpeech::~TextToSpeech()
{
    stop();
    if (d->future.isRunning()) {
        d->future.waitForFinished();
    }
}

void TextToSpeech::setText(const QString &text)
{
    d->text = text;
}

void TextToSpeech::setVoiceSpeed(double speed)
{
    d->speed = qBound(-1.0, speed, 1.0);
}

void TextToSpeech::setVoiceSettings(const VoiceSettings &settings)
{
    d->settings = settings;
}

QStringList TextToSpeech::getAvailableVoices() const
{
    QTextToSpeech localEngine;
    QStringList names;
    for (const auto& vc : localEngine.availableVoices()) {
        names << vc.name();
    }
    return names;
}

bool TextToSpeech::isSpeaking() const
{
    return d->speaking;
}

void TextToSpeech::start()
{
    if (d->text.trimmed().isEmpty()) {
        return;
    }

    stop();
    d->speaking = true;

    const QString        text     = d->text;
    const double         speed    = d->speed;
    const VoiceSettings  settings = d->settings;

    d->future = QtConcurrent::run([this, text, speed, settings]() {
        QTextToSpeech localEngine;
        QEventLoop loop;

        QObject::connect(&localEngine, &QTextToSpeech::stateChanged,
                         &loop,
                         [this, &loop](QTextToSpeech::State state) {
            switch (state) {
            case QTextToSpeech::Speaking:
                emit sig_started();
                break;
            case QTextToSpeech::Ready:
                if (d->speaking) {
                    emit sig_finished(); // Ready before speaking = init
                }
                loop.quit();
                break;
            case QTextToSpeech::BackendError:
                emit sig_errorOccurred({ ExtraClasses::SystemUnknown, "TTS backend error" });
                loop.quit();
                break;
            default:
                break;
            }
        });

        if (settings.locale != QLocale()) {
            localEngine.setLocale(settings.locale);
        }

        if (!settings.voiceName.isEmpty()) {
            const auto voices = localEngine.availableVoices();
            for (const QVoice &voice : voices) {
                if (voice.name() == settings.voiceName) {
                    localEngine.setVoice(voice);
                    break;
                }
            }
        }

        localEngine.setRate(speed);
        localEngine.setPitch(settings.pitch);
        localEngine.setVolume(settings.volume);

        localEngine.say(text);
        loop.exec();

        d->speaking = false;
    });
}

void TextToSpeech::stop()
{
    if (!d->speaking) {
        return;
    }

    d->speaking = false;

    if (d->engine) {
        QMetaObject::invokeMethod(d->engine, "stop", Qt::QueuedConnection);
    }

    if (d->future.isRunning()) {
        d->future.waitForFinished();
    }
}

} // namespace QtCustom