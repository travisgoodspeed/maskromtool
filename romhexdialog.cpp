#include "romhexdialog.h"
#include "ui_romhexdialog.h"

#include "maskromtool.h"

#include "romscene.h"

RomHexDialog::RomHexDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomHexDialog)
{
    /* macOS is really picky, but this seems to work
     * on Mac, Linux and Windows.  Removing the style hint
     * breaks Windows.
     */
    QFont font("Andale Mono"); // macOS
    font.setStyleHint(QFont::Monospace);

    ui->setupUi(this);
    ui->plaintextHex->setFont(font);

    //Quick safety checks.
    assert(positionToAdr(adrToPosition(0x1337))==0x1337);
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

//Selects a region, such as when a string is rendered.
void RomHexDialog::select(uint32_t adr, uint32_t len){
    qDebug()<<"Updating selection to"<<
                    QString::number(adr,16).prepend("0x")+
                    "+"+
                    QString::number(len,16).prepend("0x");
    start=adrToPosition(adr);
    end=adrToEndPosition(adr+len);

    //Update the selection in the text editor.
    QTextCursor cursor=ui->plaintextHex->textCursor();
    cursor.setPosition(start,QTextCursor::MoveAnchor);
    cursor.setPosition(end,QTextCursor::KeepAnchor);
    ui->plaintextHex->setTextCursor(cursor);

    //Redraw the new selection.
    mrt->scene->updateStatus();
}

//Sets a pointer to the MRT instance.
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

    /*qDebug()<<
        "Addr="<<QString::number(adr,16).prepend("0x")<<
        " -- row="<<row<<"offset="<<offset;
    */

    return adr;
}



int RomHexDialog::adrToPosition(uint32_t adr){
    /* This takes an address and converts it to a row and line.
     * You can combine the two functions to normalize a
     * selection such that no extra characters are selected.
     */

    int row=adr/16;
    int offset=(adr%16)*3+6;
    int pos=row*(6+1+3*16)+offset;

    return pos;
}

int RomHexDialog::adrToEndPosition(uint32_t adr){
    /* This takes an address and converts it to a row and line.
     * You can combine the two functions to normalize a
     * selection such that no extra characters are selected.
     */

    return adrToPosition(adr-1)+2;
}


//Highlights the selection as DRC warnings.
void RomHexDialog::on_buttonShow_clicked(){
    /* For data, we show all the bits of the selected bytes,
     * but for damage, users are only interested in those
     * that are damaged.
     */

    if(!showingDamage)
        mrt->highlightAdrRange(start, end);
    else
        mrt->highlightAdrRangeDamage(start, end);
}

