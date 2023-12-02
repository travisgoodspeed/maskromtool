#include "romhexdialog.h"
#include "ui_romhexdialog.h"

#include "maskromtool.h"

#include "romscene.h"

RomHexDialog::RomHexDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomHexDialog)
{

    QFont font("Andale Mono"); // macOS
    //QFont font("Monospace");   // Linux

    //font.setStyleHint(QFont::TypeWriter);
    //font.setStyleHint(QFont::Monospace);

    ui->setupUi(this);
    //qDebug()<<"Setting font to"<<font.toString();
    ui->plaintextHex->setFont(font);
}

RomHexDialog::~RomHexDialog(){
    delete ui;
}

void RomHexDialog::updatebinary(QByteArray bytes){
    //Save the selection for after we re-render.
    QTextCursor cursor=ui->plaintextHex->textCursor();
    int start=cursor.selectionStart();
    int end=cursor.selectionEnd();

    //Pre-allocate the buffer so we don't keep growing it.
    QString str="";
    str.reserve(bytes.length()*4);

    //Make a giant string of the hex view.
    for(uint32_t i=0; i<bytes.length(); i++){
        if((i%16==0) && i>0)
            str+="\n";
        if(i%16==0)
            str+=QString("%1  ").arg(i, 4, 16, QChar( '0' ));
        str+=QString("%1 ").arg((unsigned char) bytes[i], 2, 16, QChar( '0' ));
    }

    ui->plaintextHex->setPlainText(str);
    cursor.setPosition(start,QTextCursor::MoveAnchor);
    cursor.setPosition(end,QTextCursor::KeepAnchor);
    ui->plaintextHex->setTextCursor(cursor);
}


void RomHexDialog::setMaskRomTool(MaskRomTool *mrt){
    this->mrt=mrt;
}

void RomHexDialog::on_plaintextHex_selectionChanged(){
    QTextCursor cursor=ui->plaintextHex->textCursor();

    if(cursor.selectionStart()!=-1){
        start=positionToAdr(cursor.selectionStart());
        end=positionToAdr(cursor.selectionEnd());

        //Redraw the new selection.
        mrt->scene->updateStatus();
    }else{
        qDebug()<<"Skipping an empty hex selection.";
    }
}



uint32_t RomHexDialog::positionToAdr(int pos){
    /* A given line looks like this:
     * '0000  01 e2 0f 08 c8 41 17 87 33 fc f6 f6 76 3f f6 f6'
     *
     * As fields, that means:
     * '0000  ' -- Address and two spaces.
     * 16-bytes, 3 characters at a time.
     * Newline.
     */

    int row=pos/(6+3*16+1);
    int offset=(pos%(6+3*16+1)-6)/3;

    uint32_t adr=row*16+offset;
    //qDebug()<<"Addr="<<adr<<" -- row="<<row<<"offset="<<offset;
    return adr;
}
