#ifndef ASCIIDIALOG_H
#define ASCIIDIALOG_H

#include <QDialog>

namespace Ui {
class ASCIIDialog;
}

class ASCIIDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ASCIIDialog(QWidget *parent = nullptr);
    ~ASCIIDialog();
    void setText(QString ascii);

private:
    Ui::ASCIIDialog *ui;
};

#endif // ASCIIDIALOG_H
