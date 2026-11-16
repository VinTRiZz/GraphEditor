#include <gtest/gtest.h>

#ifdef COMPONENTS_SW_qtcustom_secure

#include <Components/CustomQt/Widgets/Etc/SecureLineEdit.h>

#include <QApplication>
#include <QClipboard>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QMimeData>
#include <QString>
#include <QtGlobal>

#include <memory>
#include <string>

namespace
{

QApplication& application()
{
    static int argc = 1;
    static char applicationName[] = "test_securelineedit";
    static char* argv[] = {applicationName, nullptr};

    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    static QApplication app(argc, argv);
    return app;
}

void sendKey(QWidget& widget, int key, Qt::KeyboardModifiers modifiers = Qt::NoModifier,
             const QString& text = QString())
{
    QKeyEvent event(QEvent::KeyPress, key, modifiers, text);
    QApplication::sendEvent(&widget, &event);
}

void typeText(QWidget& widget, const QString& text)
{
    for (const QChar ch : text) {
        sendKey(widget, ch.toUpper().unicode(), Qt::NoModifier, QString(ch));
    }
}

std::string passwordOf(const QtCustom::Etc::SecureLineEdit& edit)
{
    return edit.getPassword().value();
}

}

TEST(SecureLineEdit, MasksPassword)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("secret");

    EXPECT_EQ(passwordOf(edit), "secret");
    EXPECT_EQ(edit.text(), QString("******"));
}

TEST(SecureLineEdit, BackspaceRemovesSelectionAtEnd)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.setSelection(2, 4);

    sendKey(edit, Qt::Key_Backspace);

    EXPECT_EQ(passwordOf(edit), "ab");
    EXPECT_EQ(edit.text(), QString("**"));
    EXPECT_EQ(edit.cursorPosition(), 2);
}

TEST(SecureLineEdit, BackspaceRemovesWholeSelection)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.selectAll();

    sendKey(edit, Qt::Key_Backspace);

    EXPECT_TRUE(passwordOf(edit).empty());
    EXPECT_TRUE(edit.text().isEmpty());
}

TEST(SecureLineEdit, BackspaceRemovesSelectionInMiddle)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.setSelection(1, 3);

    sendKey(edit, Qt::Key_Backspace);

    EXPECT_EQ(passwordOf(edit), "aef");
    EXPECT_EQ(edit.text(), QString("***"));
    EXPECT_EQ(edit.cursorPosition(), 1);
}

TEST(SecureLineEdit, DeleteRemovesSelection)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.setSelection(1, 3);

    sendKey(edit, Qt::Key_Delete);

    EXPECT_EQ(passwordOf(edit), "aef");
    EXPECT_EQ(edit.text(), QString("***"));
}

TEST(SecureLineEdit, TypingReplacesSelection)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.setSelection(1, 3);

    sendKey(edit, Qt::Key_X, Qt::NoModifier, "X");

    EXPECT_EQ(passwordOf(edit), "aXef");
    EXPECT_EQ(edit.text(), QString("****"));
    EXPECT_EQ(edit.cursorPosition(), 2);
}

TEST(SecureLineEdit, BackspaceWithoutSelection)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcd");
    edit.setCursorPosition(2);

    sendKey(edit, Qt::Key_Backspace);

    EXPECT_EQ(passwordOf(edit), "acd");
    EXPECT_EQ(edit.cursorPosition(), 1);
}

TEST(SecureLineEdit, DeleteWithoutSelection)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcd");
    edit.setCursorPosition(2);

    sendKey(edit, Qt::Key_Delete);

    EXPECT_EQ(passwordOf(edit), "abd");
    EXPECT_EQ(edit.cursorPosition(), 2);
}

TEST(SecureLineEdit, PasteReplacesSelection)
{
    application();

    QApplication::clipboard()->setText("XY");

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.setSelection(1, 2);

    sendKey(edit, Qt::Key_V, Qt::ControlModifier);

    EXPECT_EQ(passwordOf(edit), "aXYdef");
    EXPECT_EQ(edit.cursorPosition(), 3);
}

TEST(SecureLineEdit, CutCopiesAndRemovesSelection)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.setSelection(1, 3);

    sendKey(edit, Qt::Key_X, Qt::ControlModifier);

    EXPECT_EQ(passwordOf(edit), "aef");
    EXPECT_EQ(QApplication::clipboard()->text(), QString("bcd"));
}

TEST(SecureLineEdit, CopyCopiesActualPassword)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abcdef");
    edit.setSelection(2, 2);

    sendKey(edit, Qt::Key_C, Qt::ControlModifier);

    EXPECT_EQ(QApplication::clipboard()->text(), QString("cd"));
    EXPECT_EQ(passwordOf(edit), "abcdef");
}

TEST(SecureLineEdit, UndoRedo)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abc");
    typeText(edit, "d");

    EXPECT_EQ(passwordOf(edit), "abcd");

    sendKey(edit, Qt::Key_Z, Qt::ControlModifier);
    EXPECT_EQ(passwordOf(edit), "abc");

    sendKey(edit, Qt::Key_Y, Qt::ControlModifier);
    EXPECT_EQ(passwordOf(edit), "abcd");
}

TEST(SecureLineEdit, RevealAndHidePassword)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("secret");
    edit.setCursorPosition(3);

    edit.revealPassword();
    EXPECT_EQ(edit.text(), QString("secret"));
    EXPECT_EQ(edit.cursorPosition(), 3);

    edit.hidePassword();
    EXPECT_EQ(edit.text(), QString("******"));
    EXPECT_EQ(edit.cursorPosition(), 3);
}

TEST(SecureLineEdit, PartlyShownKeepsLength)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPartlyShown(true);
    edit.setPassword("abcdefgh");

    EXPECT_EQ(edit.text(), QString("ab****gh"));
    EXPECT_EQ(edit.text().length(), 8);
}

TEST(SecureLineEdit, Utf8PasswordSelection)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword(QString::fromUtf8("Привет").toStdString());
    edit.setSelection(1, 3);

    sendKey(edit, Qt::Key_Backspace);

    EXPECT_EQ(QString::fromStdString(passwordOf(edit)), QString::fromUtf8("Пет"));
    EXPECT_EQ(edit.text().length(), 3);
}

TEST(SecureLineEdit, ClearResetsPassword)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("secret");
    edit.clear();

    EXPECT_TRUE(passwordOf(edit).empty());
    EXPECT_TRUE(edit.text().isEmpty());
}

TEST(SecureLineEdit, InsertRespectsMaxLength)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setMaxLength(4);

    edit.insert("abcdef");

    EXPECT_EQ(passwordOf(edit), "abcd");
    EXPECT_EQ(edit.text(), QString("****"));
}

TEST(SecureLineEdit, ControlBackspaceRemovesWord)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("one two three");
    edit.setCursorPosition(13);

    sendKey(edit, Qt::Key_Backspace, Qt::ControlModifier);

    EXPECT_EQ(passwordOf(edit), "one two ");
}

TEST(SecureLineEdit, DropInsertsText)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("ab");

    QMimeData mimeData;
    mimeData.setText("XY");

    QDragEnterEvent enterEvent(QPoint(0, 0), Qt::CopyAction, &mimeData,
                               Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&edit, &enterEvent);

    QDropEvent event(QPointF(0, 0), Qt::CopyAction, &mimeData, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&edit, &event);

    EXPECT_EQ(passwordOf(edit), "XYab");
}

TEST(SecureLineEdit, ReadOnlyIgnoresEditing)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("abc");
    edit.setReadOnly(true);

    typeText(edit, "d");
    sendKey(edit, Qt::Key_Backspace);

    EXPECT_EQ(passwordOf(edit), "abc");
}

TEST(SecureLineEdit, InputMethodCommit)
{
    application();

    QtCustom::Etc::SecureLineEdit edit;
    edit.setPassword("ab");
    edit.setCursorPosition(1);

    QInputMethodEvent event;
    event.setCommitString(QString::fromUtf8("ж"));
    QApplication::sendEvent(&edit, &event);

    EXPECT_EQ(QString::fromStdString(passwordOf(edit)), QString::fromUtf8("aжb"));
}

#endif
