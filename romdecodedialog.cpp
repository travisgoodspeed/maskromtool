#include "romdecodedialog.h"
#include "ui_romdecodedialog.h"

#include "maskromtool.h"

RomDecodeDialog::RomDecodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RomDecodeDialog)
{
    ui->setupUi(this);

    //Architectures supported by Unidasm.
    QString unidasmlist=
"  8x300          h8             m6803          sab80c515      upd177x        "
"  adsp21xx       h8h            m68030         saturn         upd7725        "
"  alpha          h8s2000        m68040         sc61860        upd7801        "
"  alpha_nt       h8s2600        m6805          scmp           upd78c05       "
"  alpha_unix     h8500          m6808          score7         upd7807        "
"  alpha_vms      h16            m6809          scudsp         upd7810        "
"  alto2          hc11           m68hc05        se3208         upd78014       "
"  a5000          hcd62121       m740           sh2            upd78024       "
"  a5500          hd6120         mb86233        sh4            upd78044a      "
"  am29000        hd61700        mb86235        sh4be          upd78054       "
"  amis2000       hd6301         mb88           sharc          upd78064       "
"  apexc          hd6309         mc88100        sigma2         upd78078       "
"  arc            hd63701        mc88110        sm500          upd78083       "
"  arcompact      hmcs40         mcs48          sm510          upd78138       "
"  arm            hp2100         minx           sm511          upd78148       "
"  arm7           hp21mx         mips1be        sm530          upd78214       "
"  arm7_be        hp_5061_3001   mips1le        sm590          upd78218a      "
"  arm7thumb      hp_5061_3011   mips3be        sm5a           upd78224       "
"  arm7thumbb     hp_09825_67907 mips3le        sm8500         upd78234       "
"  asap           hpc16083       mipsx          smc1102        upd78244       "
"  avr8           hpc16164       mk1            sparclite      upd780024a     "
"  axc51core      hyperstone     mm5799         sparcv7        upd78312       "
"  axc208         ht1130         mm76           sparcv8        upd78322       "
"  b5000          i4004          mm78           sparcv9        upd78328       "
"  b5500          i4040          mn10200        sparcv9vis1    upd78334       "
"  b6000          i8008          mn1400         sparcv9vis2    upd78352       "
"  b6100          i802x          mn1610         sparcv9vis2p   upd78356       "
"  c33            i8051          mn1613         sparcv9vis3    upd78366a      "
"  capricorn      i8052          mn1860         sparcv9vis3b   upd78372       "
"  ccpu           i8085          mn1870         spc700         upd784026      "
"  cdc1700        i8089          mn1880         ssem           upd784038      "
"  cdp1801        i80c52         msm65x2        ssp1601        upd784046      "
"  cdp1802        i860           nanoprocessor  st62xx         upd784054      "
"  cdp1805        i8x9x          nec            st9            upd784216      "
"  clipper        i8xc196        nios2          st9p           upd784218      "
"  coldfire       i8xc51fx       nova           superfx        upd784225      "
"  cop410         i8xc51gb       ns32000        t11            upd780065      "
"  cop420         i960           nuon           tlcs870        upd780988      "
"  cop444         ibm1130        nsc8105        tlcs900        upd78k0kx1     "
"  cop424         ibm1800        p8xc552        tmp90c051      upd78k0kx2     "
"  cp1610         ie15           p8xc562        tmp90840       upi41          "
"  cpu16          interdata16    pace           tmp90844       v60            "
"  cr16a          jaguardsp      palm           tms0980        v620           "
"  cr16b          jaguargpu      patinho_feio   tms1000        v620f          "
"  cr16c          konami         pdp1           tms1100        v75            "
"  cquestlin      ks0164         pdp8           tms1400        v810           "
"  cquestrot      lc57           pic16          tms2100        v850           "
"  cquestsnd      lc58           pic16c5x       tms2400        v850es         "
"  cyber18        lc8670         pic1670        tms32010       v850e2         "
"  ddp516         lh5801         pic16c62x      tms32025       vax            "
"  dp8344         lr35902        pic17          tms32031       vt50           "
"  ds5002fp       m146805        power          tms32051       vt52           "
"  ds80c320       m16c           powerpc        tms32082_mp    vt61           "
"  dsp16          m32c           pps4           tms32082_pp    we32100        "
"  dsp32c         m37710         prime16s       tms34010       x86_16         "
"  dsp56000       m4510          prime32r       tms34020       x86_32         "
"  dsp56156       m58846         prime32s       tms57002       x86_64         "
"  e0c6200        m6502          prime64r       tms7000        xavix          "
"  epg3231        m6509          prime64v       tms9900        xavix2000      "
"  esrip          m6510          psxcpu         tms9980        xavix2         "
"  evo            m65c02         r65c02         tms9995        xerox530       "
"  f2mc16         m65ce02        r65c19         tp0320         xtensa         "
"  f8             m6800          r800           tx0_64kw       z180           "
"  fr             m68000         romp           tx0_8kw        z8             "
"  fscpu32        m68008         rsp            tx0_8kwo       z80            "
"  g65816         m6801          rupi44         ucom4          z8000          "
"  gigatron       m68010         rx01           unsp10                        "
"  gt913          m6802          s2650          unsp12      "
"  h6280          m68020         sab80515       unsp20    ";
    QStringList alist=unidasmlist.split(" ",Qt::SkipEmptyParts);
    alist.sort();
    for(int i=0; i<alist.length(); i++)
        ui->listArchitecture->addItem("unidasm/"+alist[i]);


    /* FIXME: Somehow I built this without the Gameboy plug.  Perhaps we need to
     * generate the list at startup, to include whatever the user has installed?
     * --Travis
     */
    QString r2archs=
      "6502 6502.cs 8051 alpha amd29k any.as any.vasm arc arm.nz arm arm.gnu "
      "arm.v35 avr bf bpf.mr bpf chip8 cr16 cris dalvik dis ebc evm fslsp gb "
      "h8300 hppa i4004 i8080 java jdh8 kvx lanai lh5801 lm32 loongarch lua  "
      "m680x m68k m68k.gnu mcore mcs96 mips mips.gnu msp430 nds32 nios2 null "
      "or1k pdp11 pic pickle ppc ppc.gnu propeller pyc riscv riscv.cs rsp    "
      "s390 s390.gnu sh sh.cs sm5xx snes sparc sparc.gnu stm8 tms320 tricore "
      "tricore.cs v810 v850 vax wasm ws x86 x86.nasm x86.nz xap xcore xtensa "
      "z80";
    QStringList r2list=r2archs.split(" ", Qt::SkipEmptyParts);
    r2list.sort();
    for(int i=0; i<r2list.length(); i++)
        ui->listArchitecture->addItem("r2/"+r2list[i]);

    QString gaarchs=
        " tlcs47 ucom4 s2000 emz1001 pic16c5x marc4 6502 8051";
    QStringList galist=gaarchs.split(" ", Qt::SkipEmptyParts);
    galist.sort();
    for(int i=0; i<galist.length(); i++)
        ui->listArchitecture->addItem("goodasm/"+galist[i]);
}

void RomDecodeDialog::setMaskRomTool(MaskRomTool* maskRomTool){
    Q_ASSERT(maskRomTool);
    mrt=maskRomTool;

    update();
}

//Updates the string to match the configuration.
void RomDecodeDialog::updateString(){
    ui->textFlags->setPlainText(mrt->gr.description());

    QByteArray ba=mrt->gr.decode();
    mrt->hexDialog.updatebinary(ba);
    mrt->stringsDialog.updatebinary(ba);
    mrt->disDialog.update();
}

//Updates the GUI to reflect mrt->gr.
void RomDecodeDialog::update(){
    ui->checkFlipX->setCheckState(mrt->gr.flippedx ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkFlipY->setCheckState(mrt->gr.flippedy ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkInvertBits->setCheckState(mrt->gr.inverted ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->spinRotation->setValue(mrt->gr.angle);
    ui->listBank->item(mrt->gr.bank)->setSelected(true);
    ui->checkZorrom->setCheckState(mrt->gr.zorrommode ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->wordsizeEdit->setText(QString::number(mrt->gr.wordsize));

    if(mrt->gr.decoder){
        auto item =
            ui->listDecoder->findItems(mrt->gr.decoder->name, Qt::MatchFlag::MatchExactly);
        ui->listDecoder->setCurrentItem(item.value(0));
    }
    for(int i=0; i<ui->listArchitecture->count(); i++){
        QListWidgetItem *item=ui->listArchitecture->item(i);
        if(item->text()==mrt->gr.arch)
            ui->listArchitecture->setCurrentItem(item);
    }


    updateString();
}

RomDecodeDialog::~RomDecodeDialog(){
    delete ui;
}

void RomDecodeDialog::on_listDecoder_itemSelectionChanged(){
    Q_ASSERT(mrt);

    mrt->markUndoPoint(); //Call before updating mrt.

    QString decoder=ui->listDecoder->currentItem()->text();
    mrt->gr.setDecoderByName(decoder);
    updateString();
}


void RomDecodeDialog::on_checkFlipX_stateChanged(int arg1){
    Q_ASSERT(mrt);

    mrt->markUndoPoint(); //Call before updating mrt.

    //Are we flipping on the X axis?
    mrt->gr.flipx(arg1);
    updateString();
}


void RomDecodeDialog::on_checkFlipY_stateChanged(int arg1){
    Q_ASSERT(mrt);
    mrt->markUndoPoint(); //Call before updating mrt.
    mrt->gr.flipy(arg1);
    updateString();
}


void RomDecodeDialog::on_checkZorrom_stateChanged(int arg1){
    Q_ASSERT(mrt);
    mrt->markUndoPoint(); //Call before updating mrt.
    //Zorrom bug-compatibility mode.
    mrt->gr.zorrommode=arg1;
    updateString();
}


void RomDecodeDialog::on_checkInvertBits_stateChanged(int arg1){
    Q_ASSERT(mrt);
    mrt->markUndoPoint(); //Call before updating mrt.
    //Set the inversion, don't repeat it.
    mrt->gr.inverted=arg1;
    updateString();
}


void RomDecodeDialog::on_spinRotation_valueChanged(int arg1){
    Q_ASSERT(mrt);
    mrt->markUndoPoint(); //Call before updating mrt.
    //Sets the rotation, starting at zero.
    mrt->gr.rotate(arg1, true);
    updateString();
}


void RomDecodeDialog::on_listBank_currentItemChanged(QListWidgetItem *current,
                                                     QListWidgetItem *previous){
    Q_ASSERT(mrt);
    mrt->markUndoPoint(); //Call before updating mrt.
    //List is in the order of the banking constants.
    int bank=current->listWidget()->currentRow();
    mrt->gr.setBank(bank);
    updateString();
}

void RomDecodeDialog::on_wordsizeEdit_textEdited(const QString &arg1){
    Q_ASSERT(mrt);

    int wordsize=8;
    bool okay;
    wordsize=ui->wordsizeEdit->text().toInt(&okay);

    //Only update if the size looks good.
    if(okay && wordsize>0 && wordsize<=32){
        mrt->markUndoPoint();
        mrt->gr.wordsize=wordsize;
        updateString();
        qDebug()<<"wordsizeEdit changed to:"<<ui->wordsizeEdit->text();
    }
}


void RomDecodeDialog::on_listArchitecture_currentItemChanged(QListWidgetItem *current,
                                                             QListWidgetItem *previous){
    mrt->markUndoPoint(); //Call before updating mrt.

    mrt->gr.arch=current->text();
    mrt->disDialog.update();
}

