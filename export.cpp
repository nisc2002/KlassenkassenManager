#include "export.h"
#include "ui_export.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QPushButton>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QPrintDialog>
#include <QMessageBox>
#include "xlsxdocument.h"

Export::Export(QWidget *parent, int mode) :
    QDialog(parent),
    ui(new Ui::Export)
{
    ui->setupUi(this);
    this->mode = mode;

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Abbrechen");


    fatFont.setFontBold(true);                  //set format properties
    redFont.setFontColor(QColor(Qt::red));

    fatRedFont.setFontBold(true);
    fatRedFont.setFontColor(QColor(Qt::red));
}

Export::~Export()
{
    delete ui;
}

void Export::on_buttonBox_accepted()
{
    if (mode == 1) {                               //export as Excel
        filename = QFileDialog::getSaveFileName(this, "Speichern als", "C://", ".xlsx");

        if (filename == "") {
            choice = 0;
            return;
        }

        if (ui->radioOverView->isChecked()) {
            choice = 1;
        }
        else if (ui->radioAll->isChecked()) {
            choice = 2;
        }
        else if (ui->radioSelected->isChecked()) {
            choice = 3;
        }
    }
    else if (mode == 2) {                                    //export as PFD
        filename = QFileDialog::getSaveFileName(this, "Speichern als", "C://", ".pdf");

        if (filename == "") {
            choice = 0;
            return;
        }

        if (ui->radioOverView->isChecked()) {
            choice = 4;
        }
        else if (ui->radioAll->isChecked()) {
            choice = 5;
        }
        else if (ui->radioSelected->isChecked()) {
            choice = 6;
        }
    }
}

void Export::pdfOverView(QVector<Student> &stud, double total)     //somewhat fixed
{
    QString strStream;                                  //https://stackoverflow.com/questions/3147030/qtableview-printing/4079676#4079676
    QTextStream out(&strStream);

    QPrinter printer;                                    //https://doc.qt.io/qt-5/qtprintsupport-index.html
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename + ".pdf");          //add file ending

    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n").arg("Export")
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n"
        <<  QString("<h3>%1</h3>\n").arg("Übersicht");
    out <<  "<table border=1 cellspacing=0 cellpadding=2\n>";      //https://stackoverflow.com/questions/19993869/cannot-move-to-next-page-to-print-html-content-with-qprinter

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    out << QString("<th>%1</th>").arg("Name");
    out << QString("<th>%1</th>").arg("Vorname");
    out << QString("<th>%1</th>").arg("Guthaben");
    out << "</tr></thead>\n";

    for (int i = 0; i < stud.size(); i++)
    {
        out << "<tr>";          //rows
        out << QString("<td bkcolor=0>%1</td>").arg(stud[i].getName());
        out << QString("<td bkcolor=0>%1</td>").arg(stud[i].getVorname());
        out << QString("<td bkcolor=0 style=\"text-align:right\">%1</td>").arg(QString::number(stud[i].getBalance(), 'f', 2));
        out << "</tr>\n";
    }

    out << "<tr> </tr>\n <tr>";     //empty row
    out << "<td bkcolor=0></td>";
    out << "<td bkcolor=0>Total: </td>";
    out << QString("<td bkcolor=0 style=\"text-align:right\">%1</td>").arg(QString::number(total, 'f', 2));     //print total
    out << "</tr>";

    out <<  "</table>\n";
    out <<  "</body>\n"
        "</html>\n";

    QTextDocument *document = new QTextDocument();
    document->setHtml(strStream);

    document->print(&printer);
    delete document;
}

void Export::pdfAll(QVector<Student> &stud)
{
    QString strStream;                              //https://stackoverflow.com/questions/3147030/qtableview-printing/4079676#4079676
    QTextStream out(&strStream);

    QPrinter printer;                               //https://doc.qt.io/qt-5/qtprintsupport-index.html
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename + ".pdf");   //add file ending

    for (int i = 0; i < stud.size(); i++)
    {
        out <<  "<html>\n"
            "<head>\n"
            "<meta Content=\"Text/html; charset=Windows-1251\">\n"
            <<  QString("<title>%1</title>\n").arg("Export")
            <<  "</head>\n"
            "<body bgcolor=#ffffff link=#5000A0>\n"
            <<  QString("<h3>%1</h3>\n").arg("Transaktionen von " + stud[i].getName() + " " + stud[i].getVorname());        //print stud-info
        out <<  "<table border=1 cellspacing=0 cellpadding=2\n>";     //https://stackoverflow.com/questions/19993869/cannot-move-to-next-page-to-print-html-content-with-qprinter

        // headers
        out << "<thead><tr bgcolor=#f0f0f0>";

        out << QString("<th>%1</th>").arg("Datum");
        out << QString("<th>%1</th>").arg("Grund");
        out << QString("<th>%1</th>").arg("Betrag");

        out << "</tr></thead>\n";

        for(int j = 0; j < stud[i].pay.size(); j++) {
            out << "<tr>";

            out << QString("<td bkcolor=0>%1</td>").arg(stud[i].pay[j].getDate());
            out << QString("<td bkcolor=0>%1</td>").arg(stud[i].pay[j].getReason());
            out << QString("<td bkcolor=0 style=\"text-align:right\">%1</td>").arg(QString::number(stud[i].pay[j].getAmount(), 'f', 2));

            out << "</tr>\n";
        }

        out << "<tr> </tr>\n <tr>";         //empty row
        out << "<td bkcolor=0></td>";
        out << "<td bkcolor=0 width=30%>Total: </td>";
        out << QString("<td bkcolor=0 style=\"text-align:right\">%1</td>").arg(QString::number(stud[i].getBalance(), 'f', 2));
        out << "</tr>";

        out <<  "</table>\n";

        if (i != stud.size() - 1) {
            out << "<div style=\"page-break-after:always\"></div>";     //new page
            qDebug() << "yees";
        }

        out <<  "</body>\n"
            "</html>\n";

        QTextDocument *document = new QTextDocument();
        document->setHtml(strStream);

        document->print(&printer);
        delete document;
    }
}

void Export::pdfSelected(QTableWidget *tableWidget, QVector<Student> &stud)
{
    QString strStream;                                          //https://stackoverflow.com/questions/3147030/qtableview-printing/4079676#4079676
    QTextStream out(&strStream);

    QItemSelectionModel *selections = tableWidget->selectionModel();    //get selected students
    QModelIndexList selected = selections->selectedRows();

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename + ".pdf");                   //add file ending

    if (selected.size() == 0) {
        message.critical(this, "Error", "Kein Schüler ausgewählt!");    //error if no student selected
    }

    for (int i = 0; i < selected.size(); i++)
    {
        int sel = selected[i].row();

        out <<  "<html>\n"
                "<head>\n"
                "<meta Content=\"Text/html; charset=Windows-1251\">\n"
             <<  QString("<title>%1</title>\n").arg("Export")
              <<  "</head>\n"
                  "<body bgcolor=#ffffff link=#5000A0>\n"
               <<  QString("<h3>%1</h3>\n").arg("Transaktionen von " + stud[sel].getName() + " " + stud[sel].getVorname());
        out <<  "<table border=1 cellspacing=0 cellpadding=2\n>";       //https://stackoverflow.com/questions/19993869/cannot-move-to-next-page-to-print-html-content-with-qprinter

        // headers
        out << "<thead><tr bgcolor=#f0f0f0>";

        out << QString("<th>%1</th>").arg("Datum");
        out << QString("<th>%1</th>").arg("Grund");
        out << QString("<th>%1</th>").arg("Betrag");

        out << "</tr></thead>\n";

        for (int j = 0; j < stud[sel].pay.size(); j++)
        {
            out << "<tr>";
            out << QString("<td bkcolor=0>%1</td>").arg(stud[sel].pay[j].getDate());
            out << QString("<td bkcolor=0>%1</td>").arg(stud[sel].pay[j].getReason());
            out << QString("<td bkcolor=0 style=\"text-align:right\">%1</td>").arg(QString::number(stud[sel].pay[j].getAmount(), 'f', 2));
            out << "</tr>\n";
        }
        out << "<tr> </tr>\n <tr>";                 //empty row
        out << "<td bkcolor=0></td>";
        out << "<td bkcolor=0 width=30%>Total: </td>";      //set width for wider reason cells
        out << QString("<td bkcolor=0 style=\"text-align:right\">%1</td>").arg(QString::number(stud[sel].getBalance(), 'f', 2));
        out << "</tr>";
        out <<  "</table>\n";

        if (i < selected.size() - 1) {
            out << "<div style=\"page-break-after:always\"></div>";         //new page
            qDebug() << "yees";
        }

        out <<  "</body>\n"
            "</html>\n";

        QTextDocument *document = new QTextDocument();
        document->setHtml(strStream);

        document->print(&printer);          //save
        delete document;
    }
}

void Export::excelOverView(QVector<Student> &stud, double total)
{
    QXlsx::Document xlsx;       //overview

    xlsx.write("A1", "Name", fatFont);       //headers
    xlsx.write("B1", "Vorname", fatFont);
    xlsx.write("C1", "Guthaben", fatFont);

    for (int i = 0; i < stud.size(); i++)
    {
        xlsx.write(i + 2, 1, stud[i].getName());
        xlsx.write(i + 2, 2, stud[i].getVorname());

        if (stud[i].getBalance() < 0) {
            xlsx.write(i + 2, 3, stud[i].getBalance(), redFont);        //red if negative
        }
        else {
            xlsx.write(i + 2, 3, stud[i].getBalance());
        }
    }

    xlsx.write(stud.size() + 3, 2, "Total:", fatFont);

    if (total < 0) {
        xlsx.write(stud.size() + 3, 3, total, fatRedFont);
    }
    else {
        xlsx.write(stud.size() + 3, 3, total, fatFont);
    }
    qDebug() << total;
    xlsx.saveAs(filename + ".xlsx");
}

void Export::excelAll(QVector<Student> &stud)
{
    QXlsx::Document xlsx;

    for (int i = 0; i < stud.size(); i++)
    {
        xlsx.addSheet(stud[i].getName());

        xlsx.write("A1", "Zahlungen von", fatFont);
        xlsx.write("B1", stud[i].getName(), fatFont);
        xlsx.write("C1", stud[i].getVorname(), fatFont);
        xlsx.write("A3", "Datum", fatFont);
        xlsx.write("B3", "Grund", fatFont);
        xlsx.write("C3", "Menge", fatFont);

        for (int j = 0; j < stud[i].pay.size(); j++)
        {
            xlsx.write(j + 4, 1, stud[i].pay[j].getDate());
            xlsx.write(j + 4, 2, stud[i].pay[j].getReason());

            if (stud[i].pay[j].getAmount() < 0) {
                xlsx.write(j + 4, 3, stud[i].pay[j].getAmount(), redFont);      //red if negative
            }
            else {
                xlsx.write(j + 4, 3, stud[i].pay[j].getAmount());
            }
        }

        xlsx.write(stud[i].pay.size() + 5, 2, "Total:", fatFont);

        if (stud[i].getBalance() < 0) {
            xlsx.write(stud[i].pay.size() + 5, 3, stud[i].getBalance(), fatRedFont);
        }
        else {
            xlsx.write(stud[i].pay.size() + 5, 3, stud[i].getBalance(), fatFont);
        }
    }
    xlsx.saveAs(filename + ".xlsx");
}

void Export::excelSelected(QVector<Student> &stud, QTableWidget* tableWidget)
{
    QXlsx::Document xlsx;

    QItemSelectionModel *selections = tableWidget->selectionModel();
    QModelIndexList selected = selections->selectedRows();

    if (selected.size() == 0) {
        message.critical(this, "Error", "Kein Schüler ausgewählt!");    //error if no student selected
    }

    for (int i = 0; i < selected.size(); i++)
    {
        int sel = selected[i].row();
        xlsx.addSheet(stud[sel].getName());

        xlsx.write("A1", "Zahlungen von", fatFont);
        xlsx.write("B1", stud[sel].getName(), fatFont);
        xlsx.write("C1", stud[sel].getVorname(), fatFont);

        xlsx.write("A3", "Datum", fatFont);
        xlsx.write("B3", "Grund", fatFont);
        xlsx.write("C3", "Menge", fatFont);

        for (int j = 0; j < stud[sel].pay.size(); j++)
        {
            xlsx.write(j + 4, 1, stud[sel].pay[j].getDate());
            xlsx.write(j + 4, 2, stud[sel].pay[j].getReason());
            if (stud[sel].pay[j].getAmount() < 0) {
                xlsx.write(j + 4, 3, stud[sel].pay[j].getAmount(), redFont);
            }
            else {
                xlsx.write(j + 4, 3, stud[sel].pay[j].getAmount());
            }
        }

        xlsx.write(stud[sel].pay.size() + 5, 2, "Total:", fatFont);

        if (stud[sel].getBalance() < 0) {
            xlsx.write(stud[sel].pay.size() + 5, 3, stud[sel].getBalance(), fatRedFont);
        }
        else {
            xlsx.write(stud[sel].pay.size() + 5, 3, stud[sel].getBalance(), fatFont);
        }
    }
    xlsx.saveAs(filename + ".xlsx");
}
