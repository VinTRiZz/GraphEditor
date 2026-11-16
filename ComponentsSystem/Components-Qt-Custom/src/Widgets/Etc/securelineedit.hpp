#pragma once

#include <QLineEdit>
#include <QTimer>

#include <Components/Encryption/Common.h>

#include <vector>

class QContextMenuEvent;
class QDropEvent;
class QFocusEvent;
class QInputMethodEvent;
class QKeyEvent;
class QMouseEvent;

namespace QtCustom::Etc {

class SecureLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit SecureLineEdit(QWidget* parent = nullptr);

    /**
     * @brief isPartlyShown Used for password source showing
     * @return
     */
    bool isPartlyShown() const;
    void setPartlyShown(bool isEnabled);

    void setPassword(const Encryption::SafeString& pass);
    const Encryption::SafeString& getPassword() const;

    void insert(const QString& text);

    void clearPassword();

public slots:
    void revealPassword(uint msec = 5000);

    void hidePassword();

    void setText(const QString& text);
    void clear();

    void cut();
    void copy();
    void paste();
    void undo();
    void redo();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    struct EditState {
        Encryption::SafeString password;
        int cursorPosition {0};
        int selectionStart {0};
        int selectionLength {0};
    };

    void updateDisplay();
    void updateDisplay(int cursorPosition, int selectionStart = -1, int selectionLength = 0);

    QString getCurrentText() const;
    QString actualText() const;
    int actualLength() const;

    void insertText(const QString& text);
    void deleteBackward(bool wholeWord);
    void deleteForward(bool wholeWord);
    bool deleteSelection();

    void pushHistory();
    void clearHistory();

    void commitPassword(const QString& text);
    static QString sanitizeText(const QString& text);

    Encryption::SafeString m_password;
    QTimer m_hideTimer;
    bool m_isRevealed {false};
    bool m_isPartlyShown {false};

    std::vector<EditState> m_undoStack;
    std::vector<EditState> m_redoStack;
};

}
