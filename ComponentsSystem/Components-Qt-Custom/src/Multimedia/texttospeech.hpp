#pragma once

#include <QObject>
#include <QString>
#include <QLocale>
#include <QtTextToSpeech/QVoice>
#include <QFuture>

#include <Components/ExtraClasses/Error.h>

#include <memory>

namespace QtCustom::Multimedia {

    /**
     * @brief The TextToSpeech class Voice syntesator, based on QTextToSpeech.
     */
    class TextToSpeech : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Voice configuration
         */
        struct VoiceSettings
        {
            // Recommended values for rhvoice are defaults
            QString voiceName {"Alan"};     ///< Voice name (empty = default)
            QLocale locale;                 ///< Example: QLocale(QLocale::Russian, QLocale::Russia)
            double  pitch  = -0.2;          ///< Pitch, from -1.0 to 1.0
            double  volume = 0.3;           ///< Voice volume, from 0.0 to 1.0
        };

        explicit TextToSpeech(QObject *parent = nullptr);
        ~TextToSpeech() override;

        /**
         * @brief setText   Set current text
         * @param text
         */
        void setText(const QString &text);

        /**
         * @brief setVoiceSpeed Set speed coefficient from -1.0 (slow) to 1.0 (very fast)
         * @param speed         0.0 to normal
         */
        void setVoiceSpeed(double speed);

        /**
         * @brief setVoiceSettings  Configure voice
         * @param settings
         */
        void setVoiceSettings(const VoiceSettings &settings);

        /**
         * @brief getAvailableVoices    Get list of voices, available to use
         * @return
         */
        QStringList getAvailableVoices() const;

        /**
         * @brief start Start or restart speaking
         */
        void start();

        /**
         * @brief stop  Stop speaking immediately
         */
        void stop();

        bool isSpeaking() const;

    signals:
        void sig_started();
        void sig_finished();
        void sig_errorOccurred(const ExtraClasses::ErrorObject& err);

    private:
        struct Impl;
        std::shared_ptr<Impl> d;
    };

} // namespace QtCustom
