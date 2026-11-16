#include "securelineedit.hpp"

#include <QAction>
#include <QByteArray>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QDropEvent>
#include <QFocusEvent>
#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>

#include <algorithm>
#include <cstddef>

namespace QtCustom::Etc {

namespace {

constexpr std::size_t MaxHistorySize = 100;

bool isWordChar(const QChar& ch)
{
    return ch.isLetterOrNumber() || ch == QChar('_');
}

int wordBackwardLength(const QString& text, int position)
{
    int i = position;
    while (i > 0 && text.at(i - 1).isSpace()) {
        --i;
    }

    if (i <= 0) {
        return position - i;
    }

    if (isWordChar(text.at(i - 1))) {
        while (i > 0 && isWordChar(text.at(i - 1))) {
            --i;
        }
    } else {
        while (i > 0 && !text.at(i - 1).isSpace() && !isWordChar(text.at(i - 1))) {
            --i;
        }
    }
    return position - i;
}

int wordForwardLength(const QString& text, int position)
{
    int i = position;
    const int size = text.size();
    while (i < size && text.at(i).isSpace()) {
        ++i;
    }

    if (i >= size) {
        return i - position;
    }

    if (isWordChar(text.at(i))) {
        while (i < size && isWordChar(text.at(i))) {
            ++i;
        }
    } else {
        while (i < size && !text.at(i).isSpace() && !isWordChar(text.at(i))) {
            ++i;
        }
    }
    return i - position;
}

}

SecureLineEdit::SecureLineEdit(QWidget *parent)
    : QLineEdit(parent) {
    setEchoMode(QLineEdit::Normal);
    setMaxLength(256);
    setAcceptDrops(true);

    m_hideTimer.setSingleShot(true);
    connect(&m_hideTimer, &QTimer::timeout, this, &SecureLineEdit::hidePassword);
}

bool SecureLineEdit::isPartlyShown() const
{
    return m_isPartlyShown;
}

void SecureLineEdit::setPartlyShown(bool isEnabled)
{
    if (m_isPartlyShown == isEnabled) {
        return;
    }
    m_isPartlyShown = isEnabled;
    updateDisplay();
}

void SecureLineEdit::setPassword(const Encryption::SafeString &pass)
{
    m_hideTimer.stop();
    m_isRevealed = false;
    m_password = pass;
    clearHistory();
    updateDisplay(actualLength());
}

const Encryption::SafeString &SecureLineEdit::getPassword() const {
    return m_password;
}

void SecureLineEdit::setText(const QString &text) {
    setPassword(Encryption::SafeString(text.toStdString()));
}

void SecureLineEdit::insert(const QString &text) {
    insertText(text);
}

void SecureLineEdit::clear() {
    clearPassword();
}

void SecureLineEdit::clearPassword() {
    m_hideTimer.stop();
    m_isRevealed = false;
    clearHistory();
    m_password.secureClear();
    m_password.value().clear();
    QLineEdit::clear();
}

void SecureLineEdit::revealPassword(uint msec) {
    if (m_password->empty()) {
        return;
    }

    m_isRevealed = true;
    updateDisplay();
    m_hideTimer.start(static_cast<int>(msec));
}

void SecureLineEdit::hidePassword() {
    m_hideTimer.stop();
    if (!m_isRevealed) {
        return;
    }
    m_isRevealed = false;
    updateDisplay();
}

void SecureLineEdit::cut()
{
    if (isReadOnly() || !hasSelectedText()) {
        return;
    }
    copy();
    deleteSelection();
}

void SecureLineEdit::copy()
{
    if (!hasSelectedText()) {
        return;
    }
    const QString password = actualText().mid(selectionStart(), selectionLength());
    QGuiApplication::clipboard()->setText(password);
}

void SecureLineEdit::paste()
{
    if (isReadOnly()) {
        return;
    }
    insertText(QGuiApplication::clipboard()->text(QClipboard::Clipboard));
}

void SecureLineEdit::undo()
{
    if (isReadOnly() || m_undoStack.empty()) {
        return;
    }

    EditState current;
    current.password = m_password;
    current.cursorPosition = cursorPosition();
    current.selectionStart = hasSelectedText() ? selectionStart() : cursorPosition();
    current.selectionLength = selectionLength();
    m_redoStack.push_back(std::move(current));
    if (m_redoStack.size() > MaxHistorySize) {
        m_redoStack.erase(m_redoStack.begin());
    }

    EditState state = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    m_password = state.password;
    updateDisplay(state.cursorPosition, state.selectionStart, state.selectionLength);
}

void SecureLineEdit::redo()
{
    if (isReadOnly() || m_redoStack.empty()) {
        return;
    }

    EditState current;
    current.password = m_password;
    current.cursorPosition = cursorPosition();
    current.selectionStart = hasSelectedText() ? selectionStart() : cursorPosition();
    current.selectionLength = selectionLength();
    m_undoStack.push_back(std::move(current));
    if (m_undoStack.size() > MaxHistorySize) {
        m_undoStack.erase(m_undoStack.begin());
    }

    EditState state = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    m_password = state.password;
    updateDisplay(state.cursorPosition, state.selectionStart, state.selectionLength);
}

void SecureLineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Undo)) {
        undo();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Redo)) {
        redo();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Cut)) {
        cut();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Copy)) {
        copy();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Paste)) {
        paste();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::SelectAll)) {
        selectAll();
        event->accept();
        return;
    }

    const Qt::KeyboardModifiers modifiers = event->modifiers();
    const bool altGr = modifiers.testFlag(Qt::AltModifier);
    const bool wholeWord = (modifiers & Qt::ControlModifier) && !altGr;
    const bool shortcutModifier = (modifiers & (Qt::ControlModifier | Qt::MetaModifier))
                                  && !altGr;

    switch (event->key()) {
    case Qt::Key_Backspace:
        deleteBackward(wholeWord);
        event->accept();
        return;
    case Qt::Key_Delete:
        deleteForward(wholeWord);
        event->accept();
        return;
    case Qt::Key_Insert:
        if (event->modifiers() & Qt::ShiftModifier) {
            paste();
        }
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        QLineEdit::keyPressEvent(event);
        return;
    default:
        break;
    }

    const QString text = sanitizeText(event->text());
    if (!text.isEmpty() && !shortcutModifier) {
        insertText(text);
        event->accept();
        return;
    }

    QLineEdit::keyPressEvent(event);
}

void SecureLineEdit::inputMethodEvent(QInputMethodEvent *event) {
    if (!isReadOnly()) {
        const QString commit = sanitizeText(event->commitString());
        if (!commit.isEmpty()) {
            insertText(commit);
        }
    }
    event->accept();
}

void SecureLineEdit::focusOutEvent(QFocusEvent *event) {
    hidePassword();
    QLineEdit::focusOutEvent(event);
}

void SecureLineEdit::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);

    QAction *undoAction = menu.addAction(tr("&Undo"));
    undoAction->setEnabled(!isReadOnly() && !m_undoStack.empty());
    connect(undoAction, &QAction::triggered, this, &SecureLineEdit::undo);

    QAction *redoAction = menu.addAction(tr("&Redo"));
    redoAction->setEnabled(!isReadOnly() && !m_redoStack.empty());
    connect(redoAction, &QAction::triggered, this, &SecureLineEdit::redo);

    menu.addSeparator();

    const bool hasSelection = hasSelectedText();
    QAction *cutAction = menu.addAction(tr("Cu&t"));
    cutAction->setEnabled(!isReadOnly() && hasSelection);
    connect(cutAction, &QAction::triggered, this, &SecureLineEdit::cut);

    QAction *copyAction = menu.addAction(tr("&Copy"));
    copyAction->setEnabled(hasSelection);
    connect(copyAction, &QAction::triggered, this, &SecureLineEdit::copy);

    QAction *pasteAction = menu.addAction(tr("&Paste"));
    pasteAction->setEnabled(!isReadOnly()
                            && !QGuiApplication::clipboard()->text(QClipboard::Clipboard).isEmpty());
    connect(pasteAction, &QAction::triggered, this, &SecureLineEdit::paste);

    menu.addSeparator();

    QAction *selectAllAction = menu.addAction(tr("Select &All"));
    selectAllAction->setEnabled(!m_password->empty());
    connect(selectAllAction, &QAction::triggered, this, &QLineEdit::selectAll);

    menu.exec(event->globalPos());
    event->accept();
}

void SecureLineEdit::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton && !isReadOnly()) {
        const QString selectionText =
            QGuiApplication::clipboard()->text(QClipboard::Selection);
        if (!selectionText.isEmpty()) {
            setCursorPosition(cursorPositionAt(event->pos()));
            insertText(selectionText);
            event->accept();
            return;
        }
    }

    QLineEdit::mouseReleaseEvent(event);
}

void SecureLineEdit::dropEvent(QDropEvent *event) {
    if (isReadOnly() || !event->mimeData()->hasText()) {
        event->ignore();
        return;
    }

    setCursorPosition(cursorPositionAt(event->pos()));
    insertText(event->mimeData()->text());
    event->acceptProposedAction();
}

void SecureLineEdit::updateDisplay() {
    const int cursor = cursorPosition();
    const int selectionBegin = hasSelectedText() ? selectionStart() : -1;
    const int selectionSize = selectionLength();

    updateDisplay(cursor, selectionBegin, selectionSize);
}

void SecureLineEdit::updateDisplay(int cursorPosition, int selectionStart, int selectionLength) {
    QString text = getCurrentText();
    const int textLength = text.length();

    QLineEdit::setText(text);
    setCursorPosition(qBound(0, cursorPosition, textLength));

    if (selectionStart >= 0 && selectionLength > 0) {
        const int begin = qBound(0, selectionStart, textLength);
        const int length = qBound(0, selectionLength, textLength - begin);
        if (length > 0) {
            setSelection(begin, length);
        }
    }

    text.fill(QChar('\0'));
}

QString SecureLineEdit::getCurrentText() const
{
    const QString password = actualText();

    if (m_isRevealed) {
        return password;
    }

    if (m_isPartlyShown && password.length() > 4) {
        QString masked = password;
        for (int i = 2; i < masked.length() - 2; ++i) {
            masked[i] = QChar('*');
        }
        return masked;
    }

    return QString(password.length(), QChar('*'));
}

QString SecureLineEdit::actualText() const
{
    return QString::fromUtf8(m_password->data(), static_cast<int>(m_password->size()));
}

int SecureLineEdit::actualLength() const
{
    return actualText().length();
}

void SecureLineEdit::insertText(const QString &text)
{
    if (isReadOnly()) {
        return;
    }

    QString insertion = sanitizeText(text);
    QString password = actualText();

    int position = hasSelectedText() ? selectionStart() : cursorPosition();
    int replacedLength = hasSelectedText() ? selectionLength() : 0;

    position = qBound(0, position, password.length());
    replacedLength = qBound(0, replacedLength, password.length() - position);

    const int limit = maxLength();
    if (limit > 0) {
        const int available = limit - (password.length() - replacedLength);
        if (available < insertion.length()) {
            insertion.truncate(std::max(0, available));
        }
    }

    if (insertion.isEmpty() && replacedLength == 0) {
        return;
    }

    pushHistory();
    password.replace(position, replacedLength, insertion);
    commitPassword(password);
    updateDisplay(position + insertion.length());
}

void SecureLineEdit::deleteBackward(bool wholeWord)
{
    if (isReadOnly()) {
        return;
    }
    if (deleteSelection()) {
        return;
    }

    const int position = cursorPosition();
    if (position <= 0) {
        return;
    }

    QString password = actualText();
    const int length = wholeWord ? wordBackwardLength(password, position) : 1;
    if (length <= 0 || length > position) {
        return;
    }

    pushHistory();
    password.remove(position - length, length);
    commitPassword(password);
    updateDisplay(position - length);
}

void SecureLineEdit::deleteForward(bool wholeWord)
{
    if (isReadOnly()) {
        return;
    }
    if (deleteSelection()) {
        return;
    }

    const int position = cursorPosition();
    QString password = actualText();
    if (position >= password.length()) {
        return;
    }

    const int length = wholeWord ? wordForwardLength(password, position) : 1;
    if (length <= 0) {
        return;
    }

    pushHistory();
    password.remove(position, length);
    commitPassword(password);
    updateDisplay(position);
}

bool SecureLineEdit::deleteSelection()
{
    if (!hasSelectedText()) {
        return false;
    }

    QString password = actualText();
    const int begin = selectionStart();
    const int length = selectionLength();

    pushHistory();
    password.remove(begin, length);
    commitPassword(password);
    updateDisplay(begin);
    return true;
}

void SecureLineEdit::pushHistory()
{
    EditState state;
    state.password = m_password;
    state.cursorPosition = cursorPosition();
    state.selectionStart = hasSelectedText() ? selectionStart() : cursorPosition();
    state.selectionLength = selectionLength();

    m_undoStack.push_back(std::move(state));
    if (m_undoStack.size() > MaxHistorySize) {
        m_undoStack.erase(m_undoStack.begin());
    }
    m_redoStack.clear();
}

void SecureLineEdit::clearHistory()
{
    m_undoStack.clear();
    m_redoStack.clear();
}

void SecureLineEdit::commitPassword(const QString &text)
{
    QByteArray bytes = text.toUtf8();
    m_password = std::string(bytes.constData(), static_cast<std::size_t>(bytes.size()));
    bytes.fill('\0');
}

QString SecureLineEdit::sanitizeText(const QString &text)
{
    QString result;
    result.reserve(text.length());

    for (const QChar ch : text) {
        const QChar::Category category = ch.category();
        if (category == QChar::Other_Control
            || category == QChar::Separator_Line
            || category == QChar::Separator_Paragraph) {
            continue;
        }
        result.append(ch);
    }

    return result;
}

}
